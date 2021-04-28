#ifndef CONFIG_H
#define CONFIG_H

#include "inttypes.h"
typedef int32_t word_t;
#define WORD_PRINT PRIu32

#define CODE_STACK_SIZE 100
#define DATA_STACK_SIZE 100

//Dummy values to put in the state's code position to indicate that
//the interpreter is idling and not inside a function
#define IDLE_CURRENT_WORD ~0
#define IDLE_POS_IN_WORD  ~0

#include "stdio.h"
#include "stdarg.h"
static void __attribute__ ((unused)) error_msg(const char* msg){
    fprintf(stderr, "\033[31m%s\033[0m", msg);
}
static void __attribute__ ((unused)) debug_msg(const char* msg, ...){
    fprintf(stderr, "\033[36m");
    va_list arg;
    va_start(arg, msg);
    vfprintf(stderr, msg, arg);
    va_end(arg); 
    fprintf(stderr, "\033[0m");
}


#endif

