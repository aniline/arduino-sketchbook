#include <arduino.h>
#include <Wire.h>
#include "ds1307.h"

void Serial_info_byte(char *msg, byte v) {
  char buf[32];
  snprintf(buf, 32, "%s: %02x\n", msg, v);
  Serial.print(buf);
}

void DS1307_Setup () {
  DS1307_write(DS1307_regControl, 0x00); /* No squarewave out and set to low */
  DS1307_write(DS1307_regSecond, 0x00); /* Start oscillator */
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  byte v = DS1307_read(DS1307_regSecond);
  if (v & 0x80) {
     Serial.print("Clock Not running\n");
     Serial_info_byte("CH:Seconds", v);
     DS1307_Setup();
  }
  // DS1307_setTime(0, 31, 0);
  // DS1307_setDate(2015, 1, 30);
  // DS1307_setDayOfWeek(7);
}

void loop()
{
  DS1307_Time t;
  DS1307_Date d;

  byte rc = DS1307_getTime(&t);
  if (rc) {
     Serial.print("Clock Not running\n");
  } else {
     char buf[32];
     snprintf(buf, 32, "T: %d:%02d:%02d %1d\n", t.hour, t.minutes, t.seconds, t.am_pm);
     Serial.print(buf);

     rc = DS1307_getDate(&d);
     if (!rc) {
       snprintf(buf, 32, "D: %d:%d:%d (%d)\n", d.year, d.month, d.day, d.day_of_week);
       Serial.print(buf);
     }
  }

  delay(1000);
}
