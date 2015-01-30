#ifndef _DS1307_H
#define _DS1307_H

#define DS1307_I2C 0x68

#define DS1307_regSecond    0x00
#define DS1307_regMinute    0x01
#define DS1307_regHour      0x02
#define DS1307_regDay       0x03
#define DS1307_regDate      0x04
#define DS1307_regMonth     0x05
#define DS1307_regYear      0x06
#define DS1307_regControl   0x07

#define DS1307_YearOffset   2000

typedef struct {
  byte seconds;
  byte minutes;
  byte hour;
  byte hour24;
  byte am_pm;
} DS1307_Time;

typedef struct {
  byte day;
  byte day_of_week;
  byte month;
  unsigned int year;
} DS1307_Date;

byte DS1307_read(byte reg);
byte DS1307_write(byte reg, byte val);
byte DS1307_getDate(DS1307_Date *d);
byte DS1307_getTime(DS1307_Time *t);

void DS1307_setDate(int year, byte month, byte day);
void DS1307_setTime(int hour24, byte minutes, byte seconds);
void DS1307_setDayOfWeek(byte dow);

#endif
