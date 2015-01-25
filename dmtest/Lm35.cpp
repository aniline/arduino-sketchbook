#include "Arduino.h"
#include "Lm35.h"

#define NUM_AVG 8
#define AVG_INT_SCALER 10

int avg_temp = 0;
int temperature_readings[NUM_AVG] = { };
int new_temp_index = 0;
boolean avg_window_ready = false;

void Lm35_setup () {
  for (int i=0; i<NUM_AVG; i++) {
   temperature_readings[i] = 0.0f;
  }
  new_temp_index = 0;
}

void Lm35_accumulateTemp (float t) {
  temperature_readings[new_temp_index] = round(t*AVG_INT_SCALER);

  if ((new_temp_index + 1) > NUM_AVG)
    avg_window_ready = true;

  new_temp_index = (new_temp_index + 1) % NUM_AVG;

  int _avg_temp = 0;
  for (int i=0; i<NUM_AVG; i++) {
    _avg_temp = _avg_temp + temperature_readings[i];
  }
  avg_temp = round(_avg_temp/NUM_AVG);
}

void Lm35_readAccumulate (int readings, int _delay) {
  int acc_temp = 0;
  for (int i=0; i<readings; i++) {
    acc_temp += analogRead(0);
    //delay(_delay);
  }
  float temperature_C = (acc_temp * 500.0) / (1023 * readings);
  Lm35_accumulateTemp (temperature_C);
}

int  Lm35_getTemp () {
  return round(avg_temp);
}

void Lm35_print(char *sbuf, int buflen) {
  int t_fraction = avg_temp % AVG_INT_SCALER;
  int t_whole = (avg_temp - t_fraction)/AVG_INT_SCALER;
  snprintf(sbuf, buflen, "%3d.%01d C", t_whole, t_fraction);
}
