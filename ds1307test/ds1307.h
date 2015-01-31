#ifndef _DS1307RTC_H
#define _DS1307RTC_H

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

class Ds1307Rtc {
public:
  byte seconds;
  byte minutes;
  byte hour;
  byte hour24;
  byte am_pm;
  byte day;
  byte day_of_week;
  byte month;
  unsigned int year;

  byte getDate();
  byte getTime();

  void setDate(int year, byte month, byte day);
  void setTime(int hour24, byte minutes, byte seconds);
  void setDayOfWeek(byte dow);

  byte read(byte reg);
  byte write(byte reg, byte val);

  byte isRunning();
};


#endif
