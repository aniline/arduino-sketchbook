// -*- mode: c++ -*-

#include <Arduino.h>
#include <SPI.h>
#include "Max7219.h"
#include "Font5x7.h"

//#define TEST

#define NUM_MODULES 6
#define NUM_COLS (NUM_MODULES * 8)
#define BUF_PAGES 2
#define MAX_PAN ((BUF_PAGES) * NUM_COLS)
#define HARD_MAX_DRIFT NUM_COLS
#define TYP_MAX_DRIFT 8

/*
 * Messages of the form <timeout>,<option>,<message> from ESP8266 over its HSPI.
 * Capped at 32 bytes altogether.
 */
#define MAX_SPI_MSG 32
#define MAX_MSG 29 /* Typical */
byte ss_stat = LOW;

#define DISP_NORMAL 0
#define DISP_SCROLL 1
#define DISP_DRIFT  2

struct {
  byte timeout;
  byte option;
  char msg[MAX_MSG];
  bool valid;
  bool new_val;
} esp_data;

Max7219 m(NUM_MODULES);

byte buf[NUM_COLS];
byte big_buf[NUM_COLS * (BUF_PAGES+1)];
boolean scroller_active = false;
boolean drift_active = false;

/* Render to the bigger back buffer */
void write_to_scroller (char *str) {
  memset(big_buf, 0, NUM_COLS * (BUF_PAGES+1));
  int test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, str, 0);
  memcpy(big_buf+(NUM_COLS*BUF_PAGES), big_buf, NUM_COLS);
}

int pan_pos = 0;
/* Blit portion of the back buffer */
void pan(int pan_dir) {
  m.blit(big_buf, pan_pos);
  pan_pos += 1;
  if (pan_pos > MAX_PAN)
    pan_pos = 0;
}

unsigned long scroll_millis = 0;
/* Pan by one column of dots when the ticker
 * crosses _delay * 10 milliseconds */
void scroll(int _delay) {
  if (scroller_active) {
    if ((millis() - scroll_millis) > _delay) {
      scroll_millis = millis();
      pan(1);
    }
  }
}

/* Clear the display */
void clear () {
  for (int i = 0; i<8; i++) {
    m.setcol(i, 0);
  }
}

/* Display ...  string ?  */
void display_str(char *str) {
  scroller_active = false;
  drift_active = false;
  memset(buf, 0, NUM_COLS);
  write_string(buf, NUM_COLS, str, 0);
  m.blit(buf, 0);
}

/* Screensaver of sorts, for messages like 'clocks' */
char drift_pos = 0;
char drift_dir = 1;

/* A guessed width of clock; should use the write_string
 * return value to calculate this */
byte max_drift = 20;
char drift_str[MAX_MSG] = "";

void display_str_drifting(char *str) {
#if 0
  drift_pos = 0;
  drift_dir = 1;
#endif
  int test_cols;
  if (str == NULL) return;
  strncpy(drift_str, str, MAX_MSG -1);

  test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, drift_str, 0);

  if (test_cols < NUM_COLS) {
    max_drift = (NUM_COLS - test_cols);
    if (max_drift > HARD_MAX_DRIFT) {
      max_drift = HARD_MAX_DRIFT;
    }
    if (max_drift < TYP_MAX_DRIFT) {
      max_drift = TYP_MAX_DRIFT;
    }
    if (drift_pos > max_drift) {
      drift_pos = max_drift;
      drift_dir = -1;
    }
  }

  scroller_active = false; /* You are out */
  drift_active = true; /* They are in */
}

unsigned long drift_millis = 0;
/* Drift delay is in seconds */
void drift(int _delay) {
  if (!drift_active) return;

  if ((millis() - drift_millis) < _delay) {
    return;
  }
  drift_millis = millis();

  drift_pos += drift_dir;
  if (drift_pos > max_drift) {
    drift_dir = -1;
    drift_pos = max_drift;
  }
  if (drift_pos < 0) {
    drift_dir = 1;
    drift_pos = 0;
  }

  memset(buf, 0, NUM_COLS);
  write_string(buf, NUM_COLS, drift_str, drift_pos);
  m.blit(buf, 0);
}

void display_str_scroller(char *str) {
  write_to_scroller(str);
  pan_pos = 0;
  scroller_active = true;
  drift_active = false;
}

/* Slave mode for clocking by ESP module */
#define SCK   13
#define MOSI  12
#define MISO  11
#define SS    10

void SPI_Slave_Begin () {
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);
  pinMode(SS, INPUT);
  SPCR = _BV(SPE);
}

byte SPI_Slave_Read (byte value, byte *valid) {
  if (!valid)
    return 0;
  /* PINB & 4 is for checking for rising SS and ignoring SPDR value */
  if (PINB & 4) {
    *valid = 0;
    return 0;
  }
  SPDR = value;
  while(!(SPSR & (1<<SPIF)) && !(PINB & 4)) delayMicroseconds(10);
  *valid = !(PINB & 4);
  return SPDR;
}

byte parseByte(char *str, byte def) {
  char *e;
  byte v;
  v = strtol(str, &e, 10);
  if (e == str) {
    v = def;
  }
  return v;
}

byte parseEspData (char *str)
{
     bool d_valid = false;
     char *msg, *opt, *timeout = strtok(str, ",");
     esp_data.new_val = true;

     if (timeout == NULL)
	  goto Error;
     opt = strtok(NULL, ",");
     if (opt == NULL)
	  goto Error;
     msg = strtok(NULL, ",");
     if (msg == NULL)
	  goto Error;

     d_valid = true;
     {
	  esp_data.timeout = parseByte(timeout, 5);
	  esp_data.option = parseByte(opt, 1);
	  memset(esp_data.msg, 0, MAX_MSG);
	  strncpy(esp_data.msg, msg, MAX_MSG-1);
     }
Error:
     if (!d_valid) {
	  esp_data.timeout = 1;
	  esp_data.option = 0;
	  memset(esp_data.msg, 0, MAX_MSG);
     }
     esp_data.valid = d_valid;
}

void collectESPData() {
  byte ss = digitalRead(SS);
  esp_data.new_val = false;
  if ((ss_stat == HIGH) && (ss == LOW)) {
     char esp_msg[MAX_SPI_MSG+1];
     byte valid = 1, ss_now = ss, i = 0;
     for (; (i<MAX_SPI_MSG) && (valid); i++) {
        esp_msg[i] = SPI_Slave_Read(0, &valid);
        if (!valid) esp_msg[i] = 0; /* SS went high */
     }
     for (byte j = i; j <= MAX_SPI_MSG; j++) {
        esp_msg[j] = 0;
     }
     parseEspData(esp_msg);
     ss = ss_now;
  }
  ss_stat = ss;
}

/* ------------------------------------------------------------------------ */
void setup () {
  Serial.begin(115200);
  memset(buf, 0, NUM_COLS);
  memset(big_buf, 0, (NUM_COLS*(BUF_PAGES+1)));

  setup_Font5x7();
  m.init();
  m.blit(buf, 0);

  SPI_Slave_Begin();

#ifdef TEST
  esp_data.new_val = true;
  esp_data.option = DISP_SCROLL;
  esp_data.timeout = 100;
  memset(esp_data.msg, 0, MAX_MSG);
  strcpy(esp_data.msg, "Scroller");
#endif
}

unsigned long _millis = 0;
#ifdef TEST
int switchtest = 0;
#endif

void loop () {
#ifndef TEST
  collectESPData();
#endif

  if (esp_data.new_val) {
    esp_data.new_val = false;
    if (esp_data.option == DISP_SCROLL)
	 display_str_scroller(esp_data.msg);
    else if (esp_data.option == DISP_DRIFT)
         display_str_drifting(esp_data.msg);
    else
	 display_str(esp_data.msg);
    _millis = millis();
  }
  if ((esp_data.timeout != 0) && ((millis() - _millis) > (esp_data.timeout * 1000))) {
       scroller_active = drift_active = false;
       display_str("");
       esp_data.timeout = 0;
  }
  scroll(70);
  drift(1000);

#ifdef TEST
  if ((switchtest == 0) && (millis() >= 60000)) {
    switchtest = 1;
    esp_data.new_val = true;
    esp_data.option = DISP_DRIFT;
    esp_data.timeout = 65;
    memset(esp_data.msg, 0, MAX_MSG);
    strcpy(esp_data.msg, "Driftking");
  }
  if ((switchtest == 1) && (millis() >= 10000)) {
    switchtest = 2;
    esp_data.new_val = true;
    esp_data.option = DISP_DRIFT;
    esp_data.timeout = 65;
    memset(esp_data.msg, 0, MAX_MSG);
    strcpy(esp_data.msg, "Driftqueen");
  }
#endif
}
