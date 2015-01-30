#ifndef _Dtmf8870_H
#define _Dtmf8870_H

#define DTMF_IDLE             0
#define DTMF_TONE_READ        1
#define DTMF_NUMBERS_LATCHED  2

#define MAX_CLI_DIGITS 16

typedef void (*Dtmf8870_handler)(int, char, char*);

class Dtmf8870 {
private:
  Dtmf8870_handler handler;
  int cli_clk;
  int cli_loaded;

  int cli_number_timeout;
  int cli_last_digit_time;
  int time_counter_ams;
  int time_counter_s;

  char number[MAX_CLI_DIGITS +1];
  byte number_ptr;
  boolean number_valid;

public:
  Dtmf8870();
  void setup (int number_timeout, Dtmf8870_handler handler);
  void loop ();

  int counter_ms();
  int counter_s();

  byte second_ticked();
};

#endif
