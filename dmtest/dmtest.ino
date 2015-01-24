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

void write_to_scroller (char *str) {
  memset(big_buf, 0, NUM_COLS*BUF_PAGES);
  int test_cols = write_string(big_buf, NUM_COLS*BUF_PAGES, str, 0);
  memcpy(big_buf+ (NUM_COLS*BUF_PAGES), big_buf, NUM_COLS);
}

int pan_pos = 0;

void scroll() {
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

/*
 * Caller ID (DTMF) section.
 */
#define CLI_CLK  2
#define CLI_D0   6
#define CLI_D1   5
#define CLI_D2   4
#define CLI_D3   3

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

void debug_str(char *str) {
    memset(buf, 0, NUM_COLS);
    write_string(buf, NUM_COLS, str, 0);
    m.blit(buf, 0);
}

void debug_numb(char *fmt, int a, int b, char c)
{
  char sbuf[512];
  sprintf(sbuf, fmt, a, b, c);
  debug_str(sbuf);
}

char dtmf_xlat[17] = {'D', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', 'A', 'B', 'C'};

void loop_read_cli () {
  static int clk_counter = 0;

  int clk = digitalRead(CLI_CLK);
  int old_clk = cli_clk;
  cli_clk = clk;

  if (clk && !old_clk) {
    clk_counter ++;
    if (clk_counter > 999)
        clk_counter =0;

    cli_value = digitalRead(CLI_D0) |
      (digitalRead(CLI_D1) << 1) |
      (digitalRead(CLI_D2) << 2) |
      (digitalRead(CLI_D3) << 3);

    debug_numb("%03d: %01d '%c'", clk_counter, cli_value, dtmf_xlat[cli_value]);

    cli_last_digit_time = (int)(millis()/1000);
    if (number_ptr < MAX_CLI_DIGITS) {
      number_valid = true;
      number[number_ptr] = dtmf_xlat[cli_value];
      number[number_ptr+1] = '\0';
      number_ptr ++;
    }
  }

  if (number_valid &&  (((int)(millis()/1000)) - cli_last_digit_time) > 2) {
    debug_str(number);
    number[0] = '\0';
    number_ptr = 0;
    number_valid = false;
  }
}

/* END CallerID */
// Uncomment the #define CLI_WIP to enable.
// #define CLI_WIP

void setup () {
  m.init();
  //m.shutdown();
  setup_Font5x7();
  write_to_scroller("AAPPI Rulez. Aappi's rules.");

#ifdef CLI_WIP
  setup_read_cli();
  memset(buf, 0, NUM_COLS);
  debug_str("CLI_TEST");
#endif
}

byte loop_clock = 0;

void loop () {
#ifdef CLI_WIP
  loop_read_cli();
  loop_clock = loop_clock + 1;
  delay(1);
#else
  scroll();
  delay(30);
#endif
}


