#ifndef SEF_IO_H
#define SEF_IO_H

#include "SEForth.h"

char sef_input(void);
void sef_output(char ch);
void sef_init_io(void);
void sef_clean_io(void);

void sef_print_string(const char* str);

#endif

