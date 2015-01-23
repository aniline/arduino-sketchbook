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

#define CLI_CLK  2
#define CLI_D0   3
#define CLI_D1   4
#define CLI_D2   5
#define CLI_D3   6

void setup_read_cli () {
  pinMode(CLI_CLK, INPUT);
  pinMode(CLI_D0, INPUT);
  pinMode(CLI_D1, INPUT);
  pinMode(CLI_D2, INPUT);
  pinMode(CLI_D3, INPUT);
}

int cli_clk = 0;
int cli_loaded = 0;
int cli_value = 0;

int cli_last_digit_time = 0;

#define MAX_CLI_DIGITS 16

char number[MAX_CLI_DIGITS +1] = "";
byte number_ptr = 0;
boolean number_valid = false;

void loop_read_cli () {
  int clk = digitalRead(CLI_CLK);
  if (clk && !cli_clk) {
    cli_value = digitalRead(CLI_D0) |
      (digitalRead(CLI_D1) << 1) |
      (digitalRead(CLI_D2) << 2) |
      (digitalRead(CLI_D3) << 3);
    cli_last_digit_time = (int)(millis()/1000);
    if (number_ptr < MAX_CLI_DIGITS) {
      number[number_ptr] = cli_value + '0';
      number[number_ptr+1] = '\0';
      number_ptr ++;
    }
  }
  cli_clk = clk;

  if (number_valid &&  (((int)(millis()/1000)) - cli_last_digit_time) > 2) {
    memset(buf, 0, NUM_COLS);
    write_string(buf, NUM_COLS, number, 0);
  //  m.blit(buf, 0);
    number[0] = '\0';
    number_valid = false;
  }
}

void setup () {
  m.init();
//  m.send(cmd_shutdown, 0);

  setup_Font5x7();
  make_test();
  setup_read_cli();
}

byte loop_clock = 0;

void loop () {
  if ((loop_clock % 40) == 0) {
    blit_test();
  }
  loop_read_cli();
  delay(1);
  loop_clock = loop_clock + 1;
}


