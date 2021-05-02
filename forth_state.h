#include "ASCminiForth.h"
#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "stdlib.h"
#include "stdbool.h"

//Define what kind of content con be found in a word definition
enum word_type {
    normal_word,   //Words found in the dictionary
    raw_number, //User-inputed word such as 3, 6, 8
};

//This struct rpresunt each words called in a word definition
typedef struct word_node_s {
    enum word_type type;
    union {
        hash_t hash;        //To find the word in the dictionary
        word_t value;       //When used as a raw number
    } content;
} word_node_t;

//This union is used to represent the value put on the code stack
//Thos value can either be a word (optional data) or some indication
//about what piece of code we should be running
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
    //The two stack used by the interpreter
    data_stack_t* data;
    code_stack_t* code;
    //The dictionary
    struct forth_dictionary_s* dic;
    //A copy of the word being processed, to ensure fast access
	struct user_word_s* current_word_copy;
    //The current word being processed, its hash and the position in it
	code_pointer_t pos;
    //True by default, set to false when running exit when the code stack is empty
    bool running;
} forth_state_t;

forth_state_t* amf_init_state(void);
void amf_clean_state(forth_state_t* fs);

word_t amf_pop_data(forth_state_t* fs);
void amf_push_data(forth_state_t* fs, word_t w);
void amf_push_code(forth_state_t* fs, code_pointer_t p);
code_pointer_t amf_pop_code(forth_state_t* fs);

bool amf_run_step(forth_state_t* fs);
void amf_executes_node(forth_state_t* fs, struct word_node_s* node);
void amf_run(forth_state_t* fs);
void amf_exit(forth_state_t* fs);

#endif

