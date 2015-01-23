#include <SPI.h>
#include "Max7219.h"
#include "Font5x7.h"

#define NUM_MODULES 6
#define NUM_COLS (NUM_MODULES * 8)
#define BUF_PAGES 3
#define MAX_PAN ((BUF_PAGES) * NUM_COLS)

Max7219 m(NUM_MODULES);
byte buf[NUM_COLS];
byte big_buf[NUM_COLS * (BUF_PAGES+1)];

void make_test () {
  int test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, "AAPPI Rules.. Aappi's rules", 0);
  memcpy(big_buf+ (NUM_COLS*BUF_PAGES), big_buf, NUM_COLS);
}

int pan_pos = 0;

void blit_test() {
  m.blit(big_buf, pan_pos);

  pan_pos += 1;
  if (pan_pos > MAX_PAN)
    pan_pos = 0;
}

void clear () {
  for (int i = 0; i<8; i++) {
    m.setcol(i, 0);
  }
}

void setup () {
  m.init();
  setup_Font5x7();
  make_test();
}

void loop () {
  blit_test();
  delay(30);
}


