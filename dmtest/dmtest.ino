// -*- mode: c++ -*-

#include <SPI.h>
#include "Max7219.h"
#include "Dtmf8870.h"
#include "Lm35.h"
#include "Font5x7.h"

#define NUM_MODULES 6
#define NUM_COLS (NUM_MODULES * 8)
#define BUF_PAGES 3
#define MAX_PAN ((BUF_PAGES) * NUM_COLS)

Max7219 m(NUM_MODULES);

byte buf[NUM_COLS];
byte big_buf[NUM_COLS * (BUF_PAGES+1)];

void write_to_scroller (char *str) {
  memset(big_buf, 0, NUM_COLS*BUF_PAGES);
  int test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, str, 0);
  memcpy(big_buf+ (NUM_COLS*BUF_PAGES), big_buf, NUM_COLS);
}

int pan_pos = 0;

void scroll(int _delay) {
  m.blit(big_buf, pan_pos);

  pan_pos += 1;
  if (pan_pos > MAX_PAN)
    pan_pos = 0;

  delay(_delay);
}

void clear () {
  for (int i = 0; i<8; i++) {
    m.setcol(i, 0);
  }
}

void display_str(char *str) {
    memset(buf, 0, NUM_COLS);
    write_string(buf, NUM_COLS, str, 0);
    m.blit(buf, 0);
}

void loop_Temperature () {
  char sbuf[256];
  Lm35_readAccumulate(28, 70);
  Lm35_print(sbuf);
  display_str(sbuf);
  delay(40);
}

byte loop_clock = 0;
void loop_Dtmf () {
  Dtmf8870_loop_read();
  loop_clock = loop_clock + 1;
  delay(1);
}

void setup () {
  m.init();
  setup_Font5x7();

  write_to_scroller("AAPPI Rulez. Aappi's rules.");

  Lm35_setup();

  // Dtmf8870_setup();
  // memset(buf, 0, NUM_COLS);
  // display_str("CLI_TEST");
}

void loop () {
  // loop_Dtmf();
  // scroll (30);
  loop_Temperature();
}


