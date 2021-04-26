#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "config.h"
#include "stdlib.h"
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
} code_stack_t;

//The main stack
typedef struct {
    word_t* stack;
    size_t stack_pointer;
} data_stack_t;

//The interpreter's state
typedef struct {
    data_stack_t* data;
    code_stack_t* code;
    forth_dictionary_t* dic;
} forth_state_t;

forth_state_t* ft_init(void);
void ft_clean(forth_state_t* fs);


#endif

