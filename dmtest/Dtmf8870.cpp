#include "Arduino.h"
#include "Dtmf8870.h"

#define DISABLE_DTMF /* Define to disable port config and reading */

/*
 * Caller ID (DTMF) section.
 */
#define CLI_CLK  2
#define CLI_D0   6
#define CLI_D1   5
#define CLI_D2   4
#define CLI_D3   3

const char dtmf_xlat[17] PROGMEM = {
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

void Dtmf8870::setup (int number_timeout, Dtmf8870_handler handler) {
  cli_number_timeout = number_timeout;
  this->handler = handler;
#ifndef DISABLE_DTMF
  pinMode(CLI_CLK, INPUT);
  pinMode(CLI_D0, INPUT);
  pinMode(CLI_D1, INPUT);
  pinMode(CLI_D2, INPUT);
  pinMode(CLI_D3, INPUT);
#endif

  this->handler(DTMF_IDLE, '\0', "");
}

#define SECONDS_CAP 30

int digit_to_current_s = 0;
byte second_tick = 0;

void Dtmf8870::loop () {
#ifndef DISABLE_DTMF
  int clk = digitalRead(CLI_CLK);
  int old_clk = cli_clk;
  cli_clk = clk;
#endif

  time_counter_ams ++;
  if (time_counter_ams >= 1000) {
    time_counter_ams = 0;
    time_counter_s ++;
    second_tick = 1;

    if (time_counter_s >= SECONDS_CAP) {
      time_counter_s = 0;
    }
  } else {
    second_tick = 0;
  }

  if (number_valid && second_ticked())
      digit_to_current_s ++;

#ifndef DISABLE_DTMF
  if (clk && !old_clk) {
    int cli_value = digitalRead(CLI_D0) |
      (digitalRead(CLI_D1) << 1) |
      (digitalRead(CLI_D2) << 2) |
      (digitalRead(CLI_D3) << 3);

    cli_last_digit_time = time_counter_s;
    if (number_ptr < MAX_CLI_DIGITS) {
      char xc = pgm_read_byte_near(dtmf_xlat + cli_value);
      handler(DTMF_TONE_READ, xc, "");
      number_valid = true;
      digit_to_current_s = 0;
      number[number_ptr] = xc;
      number[number_ptr+1] = '\0';
      number_ptr ++;
    }
  }
#endif

  if (number_valid) {
    if (digit_to_current_s > cli_number_timeout) {
      handler(DTMF_NUMBERS_LATCHED, '\0', number);
      number[0] = '\0';
      number_ptr = 0;
      number_valid = false;
      digit_to_current_s = 0;
      handler(DTMF_IDLE, '\0', "");
    }
  }
}

int Dtmf8870::counter_s() {
  return time_counter_s;
}

int Dtmf8870::counter_ms() {
  return time_counter_ams;
}

byte Dtmf8870::second_ticked() {
  return second_tick;
}
