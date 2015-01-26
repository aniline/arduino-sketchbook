#include "Arduino.h"
#include "Lm35.h"

#define NUM_AVG 8
#define AVG_INT_SCALER 10

int avg_temp = 0;
int temperature_readings[NUM_AVG] = { };
int new_temp_index = 0;

void Lm35_setup () {
  analogReference(INTERNAL); /* Use internal 1.1v ref */
  for (int i=0; i<NUM_AVG; i++) {
    temperature_readings[i] = 0.0f;
  }
  new_temp_index = 0;
}

void Lm35_accumulateTemp (float t) {
  temperature_readings[new_temp_index] = round(t*AVG_INT_SCALER);
  new_temp_index = (new_temp_index + 1) % NUM_AVG;

  int _avg_temp = 0;
  for (int i=0; i<NUM_AVG; i++) {
    _avg_temp = _avg_temp + temperature_readings[i];
  }
  avg_temp = round(_avg_temp/NUM_AVG);
}

int acc_temp = 0;
byte acc_counter = 0;
void Lm35_readAccumulate (int readings, int _delay) {
  acc_temp += analogRead(0);
  acc_counter ++;
  if (acc_counter >= readings) {
    float temperature_C = (acc_temp * 110.0) / (1023 * readings);
    Lm35_accumulateTemp(temperature_C);
    acc_temp = 0;
    acc_counter = 0;
  }
}

int  Lm35_getTemp () {
  return avg_temp; /* ^C Multiplied by AVG_INT_SCALER */
}

void Lm35_print(char *sbuf, int buflen) {
  int t_fraction = avg_temp % AVG_INT_SCALER;
  int t_whole = (avg_temp - t_fraction)/AVG_INT_SCALER;
  snprintf(sbuf, buflen, "%3d.%01d C", t_whole, t_fraction);
}
