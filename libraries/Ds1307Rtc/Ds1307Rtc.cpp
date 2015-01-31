#include <Arduino.h>
#include <Wire.h>
#include "Ds1307Rtc.h"

void Ds1307Rtc::setup () {
     Wire.begin();
}

byte Ds1307Rtc::getDate() {
  byte v;

  v = read(DS1307_regDate);
  day = 10 * ((v & 0x30) >> 4) + (v & 0xF);

  v = read(DS1307_regMonth);
  month = 10 * ((v & 0x10) >> 4) + (v & 0xF);

  v = read(DS1307_regYear);
  year = DS1307_YearOffset + (10 * ((v & 0xF0) >> 4) + (v & 0xF));

  v = read(DS1307_regDay);
  day_of_week = v & 0x7;

  return 0;
}

byte toBCD (byte v) {
  return ((v / 10) << 4) | (v % 10);
}

byte Ds1307Rtc::getTime() {
  byte v;

  v = read(DS1307_regSecond);

  if ((v & 0x80) != 0)
    return 1;

  seconds = 10 * ((v & 0x70) >> 4) + (v & 0xF);

  v = read(DS1307_regMinute);
  minutes = 10 * ((v & 0x70) >> 4) + (v & 0xF);

  v = read(DS1307_regHour);

  if (v & 0x40) { /* 12 Hour mode */
    hour = 10 * ((v & 0x10) >> 4) + (v & 0xF);
    am_pm = (v & 0x20) ? 1 : 0;
    if (hour != 12) {
      hour24 = (am_pm) ? (hour + 12) : (hour);
    } else {
      hour24 = (am_pm) ? hour : 0;
    }
  } else {
    hour24 = (10 * ((v & 0x30) >> 4)) + (v & 0xF);
    am_pm = (hour24 >= 12) ? 1 : 0;
    hour = (am_pm) ? (hour24 - 12) : hour24;
    if (hour == 0)
      hour = 12;
  }

  return 0;
}

void Ds1307Rtc::setDate(int year, byte month, byte day) {
  write(DS1307_regDate, toBCD(day));
  write(DS1307_regMonth, toBCD(month));
  write(DS1307_regYear, toBCD(year % 100));
}

void Ds1307Rtc::setTime(int hour24, byte minutes, byte seconds) {
  write(DS1307_regHour, toBCD(hour24)); /* Set 24 Hour */
  write(DS1307_regMinute, toBCD(minutes));
  write(DS1307_regSecond, toBCD(seconds)); /* Clear CH bit also */
}

void Ds1307Rtc::setDayOfWeek(byte dow) {
  write(DS1307_regDay, dow & 0x7);
}

byte Ds1307Rtc::read(byte reg) {
    byte reg_val;

    Wire.beginTransmission(DS1307_I2C);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_I2C, 1);
    while(Wire.available())
      reg_val = Wire.read();

    return reg_val;
}

byte Ds1307Rtc::write(byte reg, byte val) {
    Wire.beginTransmission(DS1307_I2C);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}
void Serial_info_byte(char *msg, byte v);

byte Ds1307Rtc::isRunning() {
    byte v;
    Serial_info_byte("regSecond ", v);
    return (v & 0x80) == 0;
}
