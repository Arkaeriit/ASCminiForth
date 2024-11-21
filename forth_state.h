#include "SEForth.h"
#ifndef FORTH_STATE_H
#define FORTH_STATE_H

#include "stdbool.h"

// Define what kind of content con be found in a word definition
enum sef_int_type {
    normal_word,                // Words found in the dictionary
    raw_number,                 // User-inputed word such as 3, 6, 8
};

// This struct rpresunt each words called in a word definition
typedef struct word_node_s {
    enum sef_int_type type;
    union {
        hash_t hash;            // To find the word in the dictionary
        sef_int_t value;        // When used as a raw number
    } content;
} word_node_t;

// When stored in the stack, the code pointer is serialized to an sef_int_t.
typedef struct {
    hash_t current_word;
    size_t pos_in_word;
} code_pointer_t;

#define SEF_POS_IN_WORD_MASK ((1 << SEF_WORD_CONTENT_SIZE_BITS) - 1)
#define IDLE_POS_IN_WORD     (size_t) (~0 & SEF_POS_IN_WORD_MASK)

static inline code_pointer_t sef_int_to_code_pointer(sef_int_t i) {
    code_pointer_t ret;
    ret.current_word = i & SEF_HASH_MASK;
    ret.pos_in_word = (i >> SEF_HASH_SIZE_BITS) & SEF_POS_IN_WORD_MASK;
    return ret;
}

static inline sef_int_t sef_code_pointer_to_int(code_pointer_t* code) {
    sef_int_t ret = code->pos_in_word & SEF_POS_IN_WORD_MASK;
    ret <<= SEF_HASH_SIZE_BITS;
    ret |= code->current_word & SEF_HASH_MASK;
    return ret;
}

// The interpreter's state
typedef struct {
    // A parser used for "evaluate" and the likes
    struct parser_state_s* parser;
    // The two stack used by the interpreter
    sef_stack_t* data;
    sef_stack_t* code;
    // The forth memory used for allot and variables
    char* forth_memory;
    size_t forth_memory_index;
    char* pad;
    // The dictionary
    struct forth_dictionary_s* dic;
    // A copy of the word being processed, to ensure fast access
    struct user_word_s* current_word_copy;
    // The current word being processed, its hash and the position in it
    code_pointer_t pos;
    // The base internal variable used by words such as . and when inputing numbers
    sef_int_t base;
    // True by default, set to false when running exit when the code stack is empty
    bool running;
#if SEF_CLI_ARGS
    sef_int_t argc;
    char** argv;
#endif
#if SEF_PROGRAMMING_TOOLS
    int exit_code;
#endif
} forth_state_t;

forth_state_t* sef_init_state(struct parser_state_s* parser);
void sef_clean_state(forth_state_t* fs);

sef_int_t sef_pop_data(forth_state_t* fs);
void sef_push_data(forth_state_t* fs, sef_int_t w);
void sef_push_code(forth_state_t* fs, sef_int_t p);
sef_int_t sef_pop_code(forth_state_t* fs);
sef_int_t sef_peek_loop(forth_state_t* fs, int loop_depth);
void sef_quit(forth_state_t* fs);
void sef_abort(forth_state_t* fs);

bool sef_run_step(forth_state_t* fs);
error sef_executes_node(forth_state_t* fs, struct word_node_s* node);
void sef_run(forth_state_t* fs);
void sef_exit(forth_state_t* fs);

void sef_allot(forth_state_t* fs, size_t byte_requested);

#if SEF_STACK_TRACE
void sef_stack_trace(forth_state_t* fs);
#endif


#endif

