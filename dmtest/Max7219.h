#ifndef _Max7219_H_
#define _Max7219_H_

#define cmd_noop	0x00

/* To set column register add this to col number */
#define cmd_col_base	0x01

#define cmd_mode	0x09
#define cmd_intensity	0x0a
#define cmd_scanlimit	0x0b
#define cmd_shutdown	0x0c
#define cmd_displaytest 0x0f

class Max7219
{
  private:
    byte cascade_num;

  public:
    Max7219(byte v_cascade_num);
    void init();
    void send(byte cmd, byte val);
    void setcol(byte col, byte val);
    void blit(byte *frame_buffer, int pan_offs);
};

#endif
