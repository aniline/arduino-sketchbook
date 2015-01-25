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

char dtmf_xlat[17] = {
  'D', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', 'A', 'B', 'C'};

Dtmf8870::Dtmf8870() {
  cli_clk = 0;
  cli_loaded = 0;
  cli_last_digit_time = 0;
  memset(number, 0, MAX_CLI_DIGITS+1);
  number_ptr = 0;
  number_valid = false;
  time_counter_ams = 0;
  time_counter_s = 0;
}
void display_str(char *str);
void Dtmf8870::setup (int number_timeout, Dtmf8870_handler handler) {
  cli_number_timeout = (unsigned long)(number_timeout/1000);
  this->handler = handler;

  pinMode(CLI_CLK, INPUT);
  pinMode(CLI_D0, INPUT);
  pinMode(CLI_D1, INPUT);
  pinMode(CLI_D2, INPUT);
  pinMode(CLI_D3, INPUT);

  this->handler(DTMF_IDLE, '\0', "");
}


void Dtmf8870::loop () {
  int clk = digitalRead(CLI_CLK);
  int old_clk = cli_clk;
  cli_clk = clk;

  time_counter_ams ++;
  if (time_counter_ams >= 1000) {
    time_counter_ams = 0;
    time_counter_s ++;
    if (time_counter_s >= 3600) {
      time_counter_s = 0;
    }
  }

  if (clk && !old_clk) {
    int cli_value = digitalRead(CLI_D0) |
      (digitalRead(CLI_D1) << 1) |
      (digitalRead(CLI_D2) << 2) |
      (digitalRead(CLI_D3) << 3);

    cli_last_digit_time = time_counter_s;
    if (number_ptr < MAX_CLI_DIGITS) {
      handler(DTMF_TONE_READ, dtmf_xlat[cli_value], "");
      number_valid = true;
      number[number_ptr] = dtmf_xlat[cli_value];
      number[number_ptr+1] = '\0';
      number_ptr ++;
    }
  }

  int delta = (time_counter_s - cli_last_digit_time);

  if (number_valid && (delta > 2)) {
    handler(DTMF_NUMBERS_LATCHED, '\0', number);
    number[0] = '\0';
    number_ptr = 0;
    number_valid = false;
    handler(DTMF_IDLE, '\0', "");
  }
}

int Dtmf8870::counter_s() {
  return time_counter_s;
}

int Dtmf8870::counter_ms() {
  return time_counter_ams;
}
