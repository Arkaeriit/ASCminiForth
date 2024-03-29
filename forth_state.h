#include "ASCminiForth.h"
#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "stdbool.h"

// Define what kind of content con be found in a word definition
enum amf_int_type {
    normal_word,                // Words found in the dictionary
    raw_number,                 // User-inputed word such as 3, 6, 8
};

// This struct rpresunt each words called in a word definition
typedef struct word_node_s {
    enum amf_int_type type;
    union {
        hash_t hash;            // To find the word in the dictionary
        amf_int_t value;        // When used as a raw number
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
    ssize_t stack_pointer;       // In both stacks, the poiter point to the first free element
} code_stack_t;

// The main stack
typedef struct {
    amf_int_t* stack;
    ssize_t stack_pointer;
} data_stack_t;

// The interpreter's state
typedef struct {
    // The two stack used by the interpreter
    data_stack_t* data;
    code_stack_t* code;
    // A special stack used for loop control flow
    data_stack_t* loop_control;
    // The forth memory used for allot and variables
    char* forth_memory;
    size_t forth_memory_index;
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
#if AMF_CLI_ARGS
    int argc;
    char** argv;
#endif
#if AMF_PROGRAMMING_TOOLS
    int exit_code;
#endif
} forth_state_t;

forth_state_t* amf_init_state(void);
void amf_clean_state(forth_state_t* fs);

amf_int_t amf_pop_data(forth_state_t* fs);
void amf_push_data(forth_state_t* fs, amf_int_t w);
void amf_push_code(forth_state_t* fs, code_pointer_t p);
code_pointer_t amf_pop_code(forth_state_t* fs);
void amf_push_loop(forth_state_t* fs, amf_int_t w);
amf_int_t amf_pop_loop(forth_state_t* fs);
amf_int_t amf_peek_loop(forth_state_t* fs);

bool amf_run_step(forth_state_t* fs);
error amf_executes_node(forth_state_t* fs, struct word_node_s* node);
void amf_run(forth_state_t* fs);
void amf_exit(forth_state_t* fs);

void amf_allot(forth_state_t* fs, size_t byte_requested);

#endif

