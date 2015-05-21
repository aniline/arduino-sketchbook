#ifndef _Font5x7_H_
#define _Font5x7_H_

int setup_Font5x7();
int write_letter(byte *buffer, int buffer_size, char v, int offs);
int write_string(byte *buffer, int buffer_size, char *str, int blit_offs);

#endif
