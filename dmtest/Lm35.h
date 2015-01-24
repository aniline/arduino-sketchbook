#ifndef _Lm35_H_
#define _Lm35_H_

void Lm35_setup ();
void Lm35_readAccumulate (int readings, int _delay);
int  Lm35_getTemp();
void Lm35_print(char *sbuf);

#endif
