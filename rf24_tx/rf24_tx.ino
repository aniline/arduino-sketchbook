#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

/* Radio */
RF24 radio(8, 7);
byte address[][6] = { 0xF2, 0xF2, 0xF2, 0xF2, 0x01, 0x00 };

int sensorPin = A0;
int sensorValue = 0; /* Higher is blocked */
int prevSensorValue = 0;
int sensorDelta = 0;

void setup () {
  Serial.begin(57600);
  printf_begin();
  prevSensorValue = sensorValue = analogRead(sensorPin);

  radio.begin ();
  radio.setAutoAck(0);
  radio.setPALevel(RF24_PA_MIN);
  radio.setRetries(0,0);
  radio.openWritingPipe(address[0]);
  radio.printDetails ();
}

byte msg[][8] = { "Block", "Unblock"};
byte idx = 0;

void sendMsg (int idx) {
  if (!radio.writeFast( msg[idx],8,1)) {  printf("failed.\n\r");  }
  radio.txStandBy();
}

void loop () {
  sensorValue = analogRead(sensorPin);

  if (abs(prevSensorValue - sensorValue) > 700) {
    if (prevSensorValue > sensorValue) {
      printf("%4d : Off\n", sensorValue);
      sendMsg(1);
    } else {
      printf("%4d : On\n", sensorValue);
      sendMsg(0);
    }
    prevSensorValue = sensorValue;
  } else {
    printf("%4d\n", sensorValue);
  }

  delay(100);
}
