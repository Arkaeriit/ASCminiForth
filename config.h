#ifndef CONFIG_H
#define CONFIG_H

#include "stdint.h"
typedef int32_t word_t;
#define WORD_PRINT PRIi32

#define CODE_STACK_SIZE 100
#define DATA_STACK_SIZE 100

#include "stdio.h"
static void __attribute__ ((unused)) error_msg(const char* msg){
    fprintf(stderr, "%s", msg);
}

#endif

