#include <stdio.h>
#include <string.h>

#define FONT_LEN 480

unsigned char font[FONT_LEN] = {
   0x00, 0x01, 0x00, 0x3c, 0x12, 0x12, 0x3d, 0x00, 0x3e, 0x2a, 0x2a, 0x1d,
   0x00, 0x1c, 0x22, 0x22, 0x23, 0x00, 0x3e, 0x22, 0x22, 0x1d, 0x00, 0x3e,
   0x2a, 0x2a, 0x23, 0x00, 0x3e, 0x0a, 0x0a, 0x03, 0x00, 0x1c, 0x22, 0x2a,
   0x3b, 0x00, 0x3e, 0x08, 0x08, 0x3f, 0x00, 0x22, 0x3e, 0x23, 0x00, 0x30,
   0x20, 0x3f, 0x00, 0x3e, 0x08, 0x14, 0x23, 0x00, 0x3e, 0x20, 0x20, 0x21,
   0x00, 0x3e, 0x04, 0x08, 0x04, 0x3f, 0x00, 0x3e, 0x04, 0x08, 0x3f, 0x00,
   0x1c, 0x22, 0x22, 0x1d, 0x00, 0x3e, 0x0a, 0x0a, 0x05, 0x00, 0x1c, 0x22,
   0x32, 0x5d, 0x00, 0x3e, 0x0a, 0x0a, 0x35, 0x00, 0x24, 0x2a, 0x2a, 0x13,
   0x00, 0x02, 0x3e, 0x03, 0x00, 0x1e, 0x20, 0x20, 0x1f, 0x00, 0x1e, 0x20,
   0x18, 0x07, 0x00, 0x1e, 0x20, 0x10, 0x20, 0x1f, 0x00, 0x22, 0x14, 0x08,
   0x14, 0x23, 0x00, 0x02, 0x04, 0x38, 0x04, 0x03, 0x00, 0x32, 0x2a, 0x27,
   0x00, 0x38, 0x24, 0x1c, 0x21, 0x00, 0x3e, 0x24, 0x19, 0x00, 0x18, 0x24,
   0x25, 0x00, 0x18, 0x24, 0x3f, 0x00, 0x3c, 0x34, 0x2d, 0x00, 0x3c, 0x0a,
   0x0b, 0x00, 0x98, 0xa4, 0x7d, 0x00, 0x3e, 0x04, 0x39, 0x00, 0x3b, 0x00,
   0x80, 0x7b, 0x00, 0x3e, 0x08, 0x35, 0x00, 0x3f, 0x00, 0x3c, 0x04, 0x38,
   0x04, 0x39, 0x00, 0x3c, 0x04, 0x39, 0x00, 0x18, 0x24, 0x19, 0x00, 0xfc,
   0x24, 0x19, 0x00, 0x18, 0x24, 0xfd, 0x00, 0x3c, 0x08, 0x05, 0x00, 0x2c,
   0x24, 0x35, 0x00, 0x1e, 0x24, 0x25, 0x00, 0x3c, 0x20, 0x1c, 0x21, 0x00,
   0x1c, 0x20, 0x1d, 0x00, 0x1c, 0x20, 0x1c, 0x20, 0x3d, 0x00, 0x24, 0x18,
   0x25, 0x00, 0x9c, 0xa0, 0x7d, 0x00, 0x34, 0x2c, 0x2d, 0x00, 0x3c, 0x62,
   0x46, 0x3d, 0x00, 0x04, 0x7f, 0x00, 0x44, 0x62, 0x52, 0x4d, 0x00, 0x26,
   0x42, 0x4a, 0x37, 0x00, 0x38, 0x26, 0x70, 0x21, 0x00, 0x2e, 0x4a, 0x4a,
   0x33, 0x00, 0x3c, 0x4a, 0x4a, 0x33, 0x00, 0x02, 0x62, 0x1a, 0x07, 0x00,
   0x34, 0x4a, 0x4a, 0x35, 0x00, 0x24, 0x4a, 0x4a, 0x3d, 0x00, 0x04, 0xa2,
   0x12, 0x0d, 0x00, 0xbf, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x09, 0x00, 0x3c,
   0x42, 0x5a, 0x5d, 0x00, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x14, 0x3e,
   0x14, 0x3e, 0x15, 0x00, 0x14, 0x08, 0x3e, 0x08, 0x15, 0x00, 0x60, 0x18,
   0x07, 0x00, 0x06, 0x18, 0x61, 0x00, 0x21, 0x00, 0x61, 0x00, 0x08, 0x08,
   0x08, 0x09, 0x00, 0x7c, 0x83, 0x00, 0x82, 0x7d, 0x00, 0xfe, 0x83, 0x00,
   0x82, 0xff, 0x00, 0x10, 0x6c, 0x83, 0x00, 0x82, 0x6c, 0x11, 0x00, 0x28,
   0x28, 0x28, 0x29, 0x00, 0x08, 0x54, 0xfe, 0x54, 0x21, 0x00, 0x80, 0x80,
   0x80, 0x80, 0x81, 0x00, 0xff, 0x00, 0x25, 0x00, 0x65, 0x00, 0x08, 0x04,
   0x02, 0x04, 0x09, 0x00, 0x24, 0x10, 0x08, 0x25, 0x00, 0x34, 0x4a, 0x54,
   0x20, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

char chars[92] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789?!+@\"'#*/\\.,-()[]{}=$_|:;^%&";

typedef struct {
     unsigned short offset:12;
     unsigned short len:4;
} t_index;

t_index idx[256];

int mk_idx () {
     char cur_char;
     int char_idx = 0;
     int col_idx = 0;
     int char_font_idx = 0;
     int char_font_len = 0;
     int i;
     for (i = 0; i<256; i++) {
	  idx[i].offset = 0;
	  idx[i].len = 2;
     }

     cur_char = chars[char_idx];

     for (; col_idx < FONT_LEN; col_idx ++) {
	  char v = font[col_idx];
	  if (v & 1) {
	       char_font_len = (col_idx - char_font_idx) + 1;
	       idx[(int)cur_char].offset = char_font_idx;
	       idx[(int)cur_char].len = char_font_len;

	       char_idx ++;
	       char_font_idx = col_idx + 1;
	       cur_char = chars[char_idx];
	  }
     }

     printf("typedef struct {\n"
	    "      unsigned short offset:12;\n"
	    "      unsigned short len:4;\n"
	    "} t_index;\n\n"
	    "t_index idx[256] = {");

     for (i = 0; i<256; i++) {
	  if ((i%8) == 0) {
	       printf("\n");
	  }
	  printf("{ %3d, %d }, ", idx[i].offset, idx[i].len);
     }

     printf("\n};\n");
     return 0;
}

void blit_char(char v) {
     char b[65];
     int i, j;
     memset(b, '\0', 65);
     memset(b, '.', 64);
     for (i=0; i<idx[v].len; i++) {
	  unsigned char bv = 0xFE & font[idx[v].offset+i];
	  printf("Byte val = %02x\n", bv);
	  for (j=0; j<8; j++) {
	       if (bv & (1<<j)) {
		    b[(j*8)+i] = '*';
	       }
	  }
     }
     for (i=0;i<64;i++) {
	  if ((i % 8) == 0) printf("\n");
	  printf("%c", b[i]);
     }
     printf("\n");
     printf("\n");
}

int main () {
     int i;
     mk_idx();
     for (i=0; i<91; i++) {
	  printf("Char = %c\n", chars[i]);
	  blit_char(chars[i]);
	  usleep(200000);
     }
     return 0;
}
