// -*- mode: c++ -*-

#include <Arduino.h>
#include "printf.h"

#define DIN 2
#define DEB 8

void dump_hex(const char *msg, unsigned char *buf, int len) {
     int i;
     printf_P(PSTR("%s"), msg);
     for (i = 0; i<len; i++) {
	  if ((i % 16) == 0) {
	       printf_P(PSTR("\n%03x: "), i);
	  }
	  printf_P(PSTR(" %02x"), buf[i]);
     }
     printf_P(PSTR("\n"));
}

void setup ()
{
  Serial.begin(115200);
  printf_begin();

  pinMode(DIN, INPUT);
  pinMode(DEB, OUTPUT);
}

struct t_cnd_msg {
  byte tag;
  byte len;

  byte month;
  byte day;
  byte hour;
  byte minute;

  char number[16];

  byte cksum;
} cnd_msg;

bool parse_cnd_msg(byte *buf, byte len, struct t_cnd_msg *msg) {
  byte *bp = buf;

  if (*bp != 0x4)
    goto Error;

  msg->tag = *bp;
  bp ++;

  msg->len = *bp;

  if (len < (msg->len + 3))
    goto Error;

  bp ++;

  msg->month = ((*bp-'0') * 10);
  bp ++;
  msg->month += (*bp-'0');
  bp ++;

  msg->day = ((*bp-'0') * 10);
  bp ++;
  msg->day += (*bp-'0');
  bp ++;

  msg->hour = ((*bp-'0') * 10);
  bp ++;
  msg->hour += (*bp-'0');
  bp ++;

  msg->minute = ((*bp-'0') * 10);
  bp ++;
  msg->minute += (*bp-'0');
  bp ++;

  memset(msg->number, 0, 16);
  memcpy(msg->number, bp, msg->len-8);
  bp += (msg->len-8);

  msg->cksum = *bp;

  return true;

Error:
  return false;
}

void doSeizure() {
  byte v, oldv = 0x4;
  byte bits = 0;
  byte driftlen = 0, maxdrift = 0;;

  printf_P(PSTR("Waiting.\r\n"));

  PORTB &= 0xFE;

  while (PIND & 0x4);
  PORTB |= 1;

  printf_P(PSTR("Siezure\r\n"));
  do {
    v = PIND & 0x4;
    bits += ((v ^ oldv)>>2);
    oldv = v;
  } while (bits < 10);

  PORTB &= 0xFE;

  delay(360);

  PORTB |= 1;
}

struct t_cnd_msg msg;
byte buf[32];

void loop ()
{
  byte *bp = buf;
  byte len = 0;
  int stc;
  memset(buf, 0, 32);

  doSeizure();
  while (PIND & 4);

  do {
     byte v = 0, aux = 0;

     /* Start bit */
     PORTB &= 0xFE;
     delayMicroseconds(414);
     delayMicroseconds(415);

     for (byte i=8; i>0; i--) {
       PORTB &= 0xFE;
       delayMicroseconds(415);

       PORTB |= 1;
       aux = (PIND & 0x4);
       delayMicroseconds(250);
       aux |= (PIND & 0x4);
       v = (v >> 1) | (aux << 5);
       delayMicroseconds(161);
     }

     *bp = v;
     bp ++;
     len ++;

     /* Stop bit */
     PORTB &= 0xFE;
     delayMicroseconds(414);
     delayMicroseconds(415);

     stc = 0;
     while ((PIND & 4) && (stc < 1200)) stc++;
     if (stc > 1200) {
       printf("stc = %d\r\n", stc);
     }
   } while (stc < 1200);
   *bp = 0;

   delay(200);
   while (!(PIND & 0x4));

//   if (len < 0x10) {
 //    len |= 0x10;
  // }

   printf("Len = %d\r\n", len);
   dump_hex("Stuff", buf, len);
   if (parse_cnd_msg(buf, len, &msg)) {
     printf_P(PSTR("Timestamp = %02d - %02d : %02d:%02d hours\r\n"), msg.month, msg.day, msg.hour, msg.minute);
     printf_P(PSTR("Calling line number = %s\r\n"), msg.number);
     printf_P(PSTR("Tag = %02x, Checksum = %02x, Len = %02x\r\n"), msg.tag, msg.cksum, msg.len);
   } else {
     printf_P(PSTR("Parse failed (Len = %d)\r\n"), len);
   }
}
