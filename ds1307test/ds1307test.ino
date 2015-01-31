#include <Arduino.h>
#include <Ds1307Rtc.h>
#include <Wire.h>

Ds1307Rtc rtc;

void Serial_info_byte(char *msg, byte v) {
  char buf[32];
  snprintf(buf, 32, "%s: %02x\n", msg, v);
  Serial.print(buf);
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  if (!rtc.isRunning()) {
     Serial.print("Clock Not running\n");
  }
  /*
  rtc.setTime(12, 52, 00);
  rtc.setDate(2015, 1, 31);
  rtc.setDayOfWeek(7);
  */
}

void loop()
{
  byte rc = rtc.getTime();
  if (rc != 0) {
     Serial.print("L: Clock Not running\n");
  } else {
     char buf[32];
     snprintf(buf, 32, "T: %d:%02d:%02d %1d %d\n", rtc.hour, rtc.minutes, rtc.seconds, rtc.am_pm, rc);
     Serial.print(buf);

     rc = rtc.getDate();
     if (!rc) {
       snprintf(buf, 32, "D: %d:%d:%d (%d)\n", rtc.year, rtc.month, rtc.day, rtc.day_of_week);
       Serial.print(buf);
     }
  }

  delay(1000);
}
