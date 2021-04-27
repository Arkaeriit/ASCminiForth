#include "ASCminiForth.h"
#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "stdlib.h"

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
    size_t stack_pointer; //In both stacks, the poiter point to the first free element
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
    struct forth_dictionary_s* dic;
} forth_state_t;

forth_state_t* amf_init_state(void);
void amf_clean_state(forth_state_t* fs);

word_t amf_pop_data(forth_state_t* fs);
void amf_push_data(forth_state_t* fs, word_t w);


#endif

