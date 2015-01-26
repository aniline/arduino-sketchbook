// -*- mode: c++ -*-

#include <SPI.h>
#include "Max7219.h"
#include "Dtmf8870.h"
#include "Lm35.h"
#include "Font5x7.h"

#define NUM_MODULES 6
#define NUM_COLS (NUM_MODULES * 8)
#define BUF_PAGES 2
#define MAX_PAN ((BUF_PAGES) * NUM_COLS)

Max7219 m(NUM_MODULES);
Dtmf8870 d;

byte buf[NUM_COLS];
byte big_buf[NUM_COLS * (BUF_PAGES+1)];

boolean scroller_active = false;

void write_to_scroller (char *str) {
  memset(big_buf, 0, NUM_COLS * (BUF_PAGES+1));
  int test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, str, 0);
  memcpy(big_buf+(NUM_COLS*BUF_PAGES), big_buf, NUM_COLS);
}

int pan_ticker = 0;
int pan_pos = 0;

void pan(int pan_dir) {
  m.blit(big_buf, pan_pos);
  pan_pos += 1;
  if (pan_pos > MAX_PAN)
    pan_pos = 0;
}

void scroll(int _delay) {
  if (scroller_active) {
    pan_ticker ++;
    if (pan_ticker >= _delay) {
      pan_ticker = 0;
      pan(1);
    }
  }
}

void clear () {
  for (int i = 0; i<8; i++) {
    m.setcol(i, 0);
  }
}

void display_str(char *str) {
  scroller_active = false;
  memset(buf, 0, NUM_COLS);
  write_string(buf, NUM_COLS, str, 0);
  m.blit(buf, 0);
}

/* Screensaver of sorts */
byte drift_pos = 0;
char drift_dir = 1;
void display_str_drifting(byte max_drift, char *str) {
  scroller_active = false;
  memset(buf, 0, NUM_COLS);
  write_string(buf, NUM_COLS, str, drift_pos);
  m.blit(buf, 0);

  drift_pos += drift_dir;
  if ((drift_pos >= max_drift) || (drift_pos <= 0)) {
    drift_dir *= -1;
  }
}

void display_str_scroller(char *str) {
  write_to_scroller(str);
  scroller_active = true;
}

/* Progress indicator */
const byte pattern[32] PROGMEM = {
  0x0c, 0x0e, 0x0c, 0x08, 0x10, 0x30, 0x70, 0x30,
  0x00, 0x02, 0x07, 0x0f, 0xf0, 0xe0, 0x40, 0x00,
  0x00, 0x40, 0xe0, 0xf0, 0x0f, 0x07, 0x02, 0x00,
  0x30, 0x70, 0x30, 0x10, 0x08, 0x0c, 0x0e, 0x0c
};

int fan_pattern_index = 0;
void fan_pattern() {
   memcpy_P(buf+((NUM_MODULES-1)*8), pattern+(fan_pattern_index*8), 8);
   fan_pattern_index = (fan_pattern_index + 1) % 4;
   m.blit(buf, 0);
}

enum {
  DISP_CLI,
  DISP_TEMP,
} display_mode = DISP_TEMP;

unsigned int t_counter = 0;
void loop_Temperature () {
  Lm35_readAccumulate(8, 70);
  if (display_mode == DISP_TEMP) {
    int delta = d.counter_s() - t_counter;
    if (delta > 2) {
      char sbuf[16];
      Lm35_print(sbuf, 16);
      display_str_drifting(10, sbuf);
      t_counter = d.counter_s();
    }
  }
}

int number_time = 0;
void loop_Dtmf () {
  d.loop();
  if ((display_mode == DISP_CLI) && ((d.counter_s() - number_time) > 6)) {
    display_mode = DISP_TEMP;
  }
}

void dtmf_handler(int state, char c, char * str) {
  char sbuf[17];
  switch (state) {
  case DTMF_IDLE:
    break;
  case DTMF_TONE_READ:
    number_time = d.counter_s();
    display_mode = DISP_CLI;
    fan_pattern();
    break;
  case DTMF_NUMBERS_LATCHED:
    number_time = d.counter_s();
    display_mode = DISP_CLI;
    snprintf(sbuf, MAX_CLI_DIGITS, "%s", str);
    display_str_scroller(sbuf);
    break;
  default:
    ;
  }
}

void setup () {
  m.init();
  setup_Font5x7();

  Lm35_setup();
  d.setup(1, dtmf_handler);

  memset(buf, 0, NUM_COLS);
  memset(big_buf, 0, (NUM_COLS*(BUF_PAGES+1)));
  m.blit(buf, 0);
  display_str("Starting");
}

void loop () {
  loop_Dtmf();
  loop_Temperature();
  scroll(30);
  delayMicroseconds(1000);
}




