#include "ASCminiForth.h"

static const char* word_delimiters = " \t\n\r";

void shell(void){
    printf("Starting ASCminiFORTH.h");
    forth_state_t* fs = amf_init_state();
    char buffer[100];
    while(true){

    }
