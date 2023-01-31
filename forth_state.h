#include "ASCminiForth.h"
#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "stdbool.h"

// Define what kind of content con be found in a word definition
enum amf_int_type {
    normal_word,                // Words found in the dictionary
    raw_number,                 // User-inputed word such as 3, 6, 8
    printed_string,             // String to print such as ." coucou"
    forth_string,               // Forth strings such as S" coucou"
};

// This struct rpresunt each words called in a word definition
typedef struct word_node_s {
    enum amf_int_type type;
    union {
        hash_t hash;            // To find the word in the dictionary
        amf_int_t value;        // When used as a raw number
        char* string;           // When used as a printd string or a raw_string
    } content;
} word_node_t;

// This union is used to represent the value put on the code stack
// This value can either be a word (optional data) or some indication
// about what piece of code we should be running
typedef union {
    struct {
        hash_t current_word;
        size_t pos_in_word; // Maybe change this type so that both members of the union are of the same size/
    } code;
    amf_int_t optional_data;
} code_pointer_t;

// The code stack
typedef struct {
    code_pointer_t* stack;
    size_t stack_pointer;       // In both stacks, the poiter point to the first free element
} code_stack_t;

// The main stack
typedef struct {
    amf_int_t* stack;
    size_t stack_pointer;
} data_stack_t;

// The interpreter's state
typedef struct {
    // The two stack used by the interpreter
    data_stack_t* data;
    code_stack_t* code;
    // The dictionary
    struct forth_dictionary_s* dic;
    // A copy of the word being processed, to ensure fast access
    struct user_word_s* current_word_copy;
    // The current word being processed, its hash and the position in it
    code_pointer_t pos;
    // The base internal variable used by words such as . and when inputing numbers
    amf_int_t base;
    // True by default, set to false when running exit when the code stack is empty
    bool running;
} forth_state_t;

forth_state_t* amf_init_state(void);
void amf_clean_state(forth_state_t* fs);

amf_int_t amf_pop_data(forth_state_t* fs);
void amf_push_data(forth_state_t* fs, amf_int_t w);
void amf_push_code(forth_state_t* fs, code_pointer_t p);
code_pointer_t amf_pop_code(forth_state_t* fs);

bool amf_run_step(forth_state_t* fs);
error amf_executes_node(forth_state_t* fs, struct word_node_s* node);
void amf_run(forth_state_t* fs);
void amf_exit(forth_state_t* fs);

#endif

