#ifndef AMF_IO_H
#define AMF_IO_H

#include "SEForth.h"

char amf_input(void);
void amf_output(char ch);
void amf_init_io(void);
void amf_clean_io(void);

void amf_print_string(const char* str);

#endif

