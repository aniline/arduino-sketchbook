#include "SPI.h"
#include "Arduino.h"
#include "Max7219.h"

Max7219::Max7219(byte v_cascade_num)
{
     cascade_num = v_cascade_num;
}

void Max7219::init()
{
     SPI.begin();
     SPI.setDataMode(SPI_MODE0);
     SPI.setBitOrder(MSBFIRST);

     send(cmd_scanlimit, 0x07);
     send(cmd_mode, 0x00);
     send(cmd_shutdown, 0x01);
     send(cmd_displaytest, 0x00);
     send(cmd_intensity, 0x00);
}

void Max7219::shutdown()
{
    send(cmd_shutdown, 0x00);
    SPI.end();
}

/* Repeats the command for all the modules */
void Max7219::send(byte cmd, byte val)
{
     digitalWrite(SS, LOW);
     for (byte i=0; i<cascade_num; i++)
     {
	  SPI.transfer(cmd);
	  SPI.transfer(val);
     }
     digitalWrite(SS, HIGH);
}

/* Set column to value for module on which the column is, col can be > 7 */
void Max7219::setcol(byte col, byte val)
{
     byte chip = col / 8;
     byte chip_col = col % 8;

     digitalWrite(SS, LOW);

     SPI.transfer(cmd_col_base + chip_col);
     SPI.transfer(val);

     for (byte i=0; i<chip; i++) {
	  SPI.transfer(cmd_noop);
	  SPI.transfer(0);
     }

     digitalWrite(SS, HIGH);
}

void Max7219::blit(byte *frame_buffer, int pan_offs) {
  for (byte col = 0; col < 8; col++) {
    digitalWrite(SS, LOW);
    for (byte chip = 0, chip_offs = (cascade_num - 1) * 8; chip < cascade_num; chip++, chip_offs -= 8) {
      int real_offs = chip_offs + col + pan_offs;
      SPI.transfer(cmd_col_base + col);
      SPI.transfer(frame_buffer[real_offs]);
    }
    digitalWrite(SS, HIGH);
  }
}
