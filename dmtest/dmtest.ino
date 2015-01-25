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

void display_str_scroller(char *str) {
  write_to_scroller(str);
  scroller_active = true;
}

enum {
  DISP_CLI,
  DISP_TEMP,
} 
display_master = DISP_TEMP;

void loop_Temperature () {
  static int clk = 0;

  Lm35_readAccumulate(28, 70);
  if (display_master == DISP_TEMP) {
    int delta = (millis()/1000) - clk;
    if (delta > 2) {
      char sbuf[16];
      Lm35_print(sbuf, 16);
      display_str(sbuf);
      clk = millis();
    }
  }
}


int number_time = 0;
void loop_Dtmf () {
  d.loop();

  if ((display_master == DISP_CLI) && ((d.counter_s() - number_time) > 6)) {
    ;
  }
}

void dtmf_handler(int state, char c, char * str) {
  char sbuf[32];
  switch (state) {
  case DTMF_IDLE:
    break;
  case DTMF_TONE_READ:
    number_time = d.counter_s();
    display_master = DISP_CLI;
    sprintf(sbuf, "TONE %c", c);
    display_str(sbuf);
    break;
  case DTMF_NUMBERS_LATCHED:
    number_time = d.counter_s();
    display_master = DISP_CLI;
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
  d.setup(2000, dtmf_handler);

  memset(buf, 0, NUM_COLS);
  memset(big_buf, 0, (NUM_COLS*(BUF_PAGES+1)));
  m.blit(buf, 0);

  display_str("Starting.");
}

void loop () {
  loop_Dtmf();
  // loop_Temperature();
  scroll(30);
  delay(1);
}




