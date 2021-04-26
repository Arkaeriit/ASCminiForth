#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "dictionary.h"

//This union is used to represent the value put on the code stack
typedef union {
    struct {
        hash_t current_word;
        size_t pos_in_word;
    } code;
    word_t optional_data;
} code_pointer_t;

//The code stack
typedef struct {
    code_pointer_t* stack;
    size_t stack_pointer;
} code_stack;

//The main stack
typedef struct {
    word_t* stack;
    size_t stack_pointer;
} data_stack;



#endif

