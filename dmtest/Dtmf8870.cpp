#include "Arduino.h"
#include "Dtmf8870.h"

/*
 * Caller ID (DTMF) section.
 */
#define CLI_CLK  2
#define CLI_D0   6
#define CLI_D1   5
#define CLI_D2   4
#define CLI_D3   3

void Dtmf8870_setup () {
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

void display_str(char *str);

void debug_numb(char *fmt, int a, int b, char c)
{
  char sbuf[512];
  sprintf(sbuf, fmt, a, b, c);
  display_str(sbuf);
}

char dtmf_xlat[17] = {'D', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', 'A', 'B', 'C'};

void Dtmf8870_loop_read () {
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
    display_str(number);
    number[0] = '\0';
    number_ptr = 0;
    number_valid = false;
  }
}
