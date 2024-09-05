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

// When stored in the stack, the code pointer is serialized to an amf_int_t.
typedef struct {
    hash_t current_word;
    size_t pos_in_word;
} code_pointer_t;

#define AMF_POS_IN_WORD_MASK ((1 << AMF_WORD_CONTENT_SIZE_BITS) - 1)
#define IDLE_POS_IN_WORD     (size_t) (~0 & AMF_POS_IN_WORD_MASK)

static inline code_pointer_t amf_int_to_code_pointer(amf_int_t i) {
    code_pointer_t ret;
    ret.current_word = i & AMF_HASH_MASK;
    ret.pos_in_word = (i >> AMF_HASH_SIZE_BITS) & AMF_POS_IN_WORD_MASK;
    return ret;
}

static inline amf_int_t amf_code_pointer_to_int(code_pointer_t* code) {
    amf_int_t ret = code->pos_in_word & AMF_POS_IN_WORD_MASK;
    ret <<= AMF_HASH_SIZE_BITS;
    ret |= code->current_word & AMF_HASH_MASK;
    return ret;
}

// The interpreter's state
typedef struct {
    // A parser used for "evaluate" and the likes
    struct parser_state_s* parser;
    // The two stack used by the interpreter
    amf_stack_t* data;
    amf_stack_t* code;
    // A special stack used for loop control flow
    amf_stack_t* loop_control;
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
    amf_int_t argc;
    char** argv;
#endif
#if AMF_PROGRAMMING_TOOLS
    int exit_code;
#endif
} forth_state_t;

#if AMF_STACK_BOUND_CHECKS
#define STACK_BOUND_CHECK(fs, stack_name)                                      \
    if (!amf_state_state_valid(fs->stack_name)) {                              \
        error_msg("Stack '%s' out of bound. Resetting state.\n", #stack_name); \
        recover_from_error(fs);                                                \
    }                                                                           
#else
#define STACK_BOUND_CHECK(x...)
#endif

#define STACK_POP(fs, stack_name) ({               \
    debug_msg("pop on %s at index: %zi\n",         \
            #stack_name,                           \
            fs->stack_name->stack_pointer);        \
    amf_int_t ret = amf_stack_pop(fs->stack_name); \
    STACK_BOUND_CHECK(fs, stack_name);             \
    ret;                                           \
})

#define STACK_PUSH(fs, stack_name, w)       \
    debug_msg("push on %s at index: %zi\n", \
            #stack_name,                    \
            fs->stack_name->stack_pointer); \
    amf_stack_push(fs->stack_name, w);      \
    STACK_BOUND_CHECK(fs, stack_name);

forth_state_t* amf_init_state(struct parser_state_s* parser);
void amf_clean_state(forth_state_t* fs);

amf_int_t amf_pop_data(forth_state_t* fs);
void amf_push_data(forth_state_t* fs, amf_int_t w);
void amf_push_code(forth_state_t* fs, amf_int_t p);
amf_int_t amf_pop_code(forth_state_t* fs);
void amf_push_loop(forth_state_t* fs, amf_int_t w);
amf_int_t amf_pop_loop(forth_state_t* fs);
amf_int_t amf_peek_loop(forth_state_t* fs, int loop_depth);

bool amf_run_step(forth_state_t* fs);
error amf_executes_node(forth_state_t* fs, struct word_node_s* node);
void amf_run(forth_state_t* fs);
void amf_exit(forth_state_t* fs);

void amf_allot(forth_state_t* fs, size_t byte_requested);

#if AMF_STACK_TRACE
void amf_stack_trace(forth_state_t* fs);
#endif


#endif

