// -*- mode: c++ -*-

#include "Arduino.h"
#include <SPI.h>
#include "printf.h"

#define BUFSIZE 128

byte m_buf[2][BUFSIZE];
byte w_buf = 0;
byte w_index = 0;
byte w_busy = 0;

/* Lossy */
void copy_switch(void *b, byte *l) {
  byte oldSREG;
  byte len;

  oldSREG = SREG;
  cli();
  while (w_busy);
  w_busy = 1;

  len = w_index;
  w_index = 0;
  w_buf ^= 1;

  SREG = oldSREG;

  w_busy = 0;

  memcpy(b, m_buf[w_buf ^ 1], len);
  *l = len;
}

void setup() {
    Serial.begin(115200);
    printf_begin();
    DDRB  |= 0b00010000; /* PB4 out (OUT - MISO), PB3 (IN - MOSI), PB5 SCK IN  */
    PORTB |= 0b00000100; /* Pull up PB2 (~SS, D10) */
    SPCR  |= 0b11000100; /* Int enable (d7), SPI enable (d6), Slave (d4=0), Phase trailing edge (d2) */
    SPSR  |= 0b10000000; /* Int flag */
    sei();
}

byte line_chars = 0;

void loop() {
  if(w_index > 16){
    byte s_buf [BUFSIZE];
    byte s_len;
    byte i;
    copy_switch(s_buf, &s_len);

    for (i = 0; i<BUFSIZE; i++) {
      byte v = s_buf[i];
      if ((v < 32) || (v > 128)) {
        v = '.';
      }
      if (i >= (s_len-1))
        v = 0;
      s_buf[i] = v;
    }
    line_chars += s_len;

    Serial.write(s_buf, s_len);
    if (line_chars >= 64) {
      line_chars = 0;
      Serial.println("");
    }
  }
}

ISR(SPI_STC_vect){
  cli();
  while (w_busy);
  w_busy = 1;

  if (w_index < BUFSIZE) {
    m_buf[w_buf][w_index] = SPDR;
    w_index ++;
  }

  w_busy = 0;
  SPSR = (1 << SPIF);
  sei();
}
