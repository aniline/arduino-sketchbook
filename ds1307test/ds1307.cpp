#include "arduino.h"
#include "ds1307.h"
#include "Wire.h"

byte DS1307_getDate(DS1307_Date *d) {
  if (!d) return -1;
  byte v;

  v = DS1307_read(DS1307_regDate);
  d->day = 10 * ((v & 0x30) >> 4) + (v & 0xF);

  v = DS1307_read(DS1307_regMonth);
  d->month = 10 * ((v & 0x10) >> 4) + (v & 0xF);

  v = DS1307_read(DS1307_regYear);
  d->year = DS1307_YearOffset + (10 * ((v & 0xF0) >> 4) + (v & 0xF));

  v = DS1307_read(DS1307_regDay);
  d->day_of_week = v & 0x7;

  return 0;
}

byte toBCD (byte v) {
  return ((v / 10) << 4) | (v % 10);
}

byte DS1307_getTime(DS1307_Time *t) {
  if (!t) return -1;

  byte v;
  v = DS1307_read(DS1307_regSecond);

  if (v & 0x80)
    return -1;

  t->seconds = 10 * ((v & 0x70) >> 4) + (v & 0xF);

  v = DS1307_read(DS1307_regMinute);
  t->minutes = 10 * ((v & 0x70) >> 4) + (v & 0xF);

  v = DS1307_read(DS1307_regHour);

  if (v & 0x40) { /* 12 Hour mode */
    t->hour = 10 * ((v & 0x10) >> 4) + (v & 0xF);
    t->am_pm = (v & 0x20) ? 1 : 0;
    if (t->hour != 12) {
      t->hour24 = (t->am_pm) ? (t->hour + 12) : (t->hour);
    } else {
      t->hour24 = (t->am_pm) ? t->hour : 0;
    }
  } else {
    t->hour24 = (10 * ((v & 0x30) >> 4)) + (v & 0xF);
    t->am_pm = (t->hour24 >= 12) ? 1 : 0;
    t->hour = (t->am_pm) ? (t->hour24 - 12) : t->hour24;
    if (t->hour == 0)
      t->hour = 12;
  }

  return 0;
}

void DS1307_setDate(int year, byte month, byte day) {
  DS1307_write(DS1307_regDate, toBCD(day));
  DS1307_write(DS1307_regMonth, toBCD(month));
  DS1307_write(DS1307_regYear, toBCD(year % 100));
}

void DS1307_setTime(int hour24, byte minutes, byte seconds) {
  DS1307_write(DS1307_regHour, toBCD(hour24)); /* Set 24 Hour */
  DS1307_write(DS1307_regMinute, toBCD(minutes));
  DS1307_write(DS1307_regSecond, toBCD(seconds)); /* Clear CH bit also */
}

void DS1307_setDayOfWeek(byte dow) {
  DS1307_write(DS1307_regDay, dow & 0x7);
}

byte DS1307_read(byte reg) {
    byte reg_val;

    Wire.beginTransmission(DS1307_I2C);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_I2C, 1);
    while(Wire.available())
      reg_val = Wire.read();

    return reg_val;
}

byte DS1307_write(byte reg, byte val) {
    Wire.beginTransmission(DS1307_I2C);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}
