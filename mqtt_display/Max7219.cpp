#include "Arduino.h"
#include "Max7219.h"

Max7219::Max7219(byte v_cascade_num)
{
     cascade_num = v_cascade_num;
}

/* Bit bang pins */
#define BANG_CLK  4
#define BANG_DATA 3
#define BANG_CS   2

void _SPI_Transfer(byte val) {
     digitalWrite(BANG_CLK, LOW);
     shiftOut(BANG_DATA, BANG_CLK, MSBFIRST, val);
     digitalWrite(BANG_CLK, LOW);
}

void Max7219::init()
{
     pinMode(BANG_CLK, OUTPUT);
     pinMode(BANG_DATA, OUTPUT);
     pinMode(BANG_CS, OUTPUT);
     digitalWrite(BANG_CS, HIGH);
     digitalWrite(BANG_DATA, LOW);
     digitalWrite(BANG_CLK, LOW);
     
     send(cmd_scanlimit, 0x07);
     send(cmd_mode, 0x00);
     send(cmd_shutdown, 0x01);
     send(cmd_displaytest, 0x00);
     send(cmd_intensity, 0x00);
}

void Max7219::shutdown()
{
    send(cmd_shutdown, 0x00);
}

/* Repeats the command for all the modules */
void Max7219::send(byte cmd, byte val)
{
     digitalWrite(BANG_CS, LOW);
     for (byte i=0; i<cascade_num; i++)
     {
	  _SPI_Transfer(cmd);
	  _SPI_Transfer(val);
     }
     digitalWrite(BANG_CS, HIGH);
}

/* Set column to value for module on which the column is, col can be > 7 */
void Max7219::setcol(byte col, byte val)
{
     byte chip = col / 8;
     byte chip_col = col % 8;

     digitalWrite(BANG_CS, LOW);

     _SPI_Transfer(cmd_col_base + chip_col);
     _SPI_Transfer(val);

     for (byte i=0; i<chip; i++) {
	  _SPI_Transfer(cmd_noop);
	  _SPI_Transfer(0);
     }

     digitalWrite(BANG_CS, HIGH);
}

void Max7219::blit(byte *frame_buffer, int pan_offs) {
  for (byte col = 0; col < 8; col++) {
    digitalWrite(BANG_CS, LOW);
    for (byte chip = 0, chip_offs = (cascade_num - 1) * 8; chip < cascade_num; chip++, chip_offs -= 8) {
      int real_offs = chip_offs + col + pan_offs;
      _SPI_Transfer(cmd_col_base + col);
      _SPI_Transfer(frame_buffer[real_offs]);
    }
    digitalWrite(BANG_CS, HIGH);
  }
}
