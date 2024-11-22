#include "private_api.h"
#ifndef SEF_SHELL_H
#define SEF_SHELL_H

typedef void (*new_word_hook_t)(struct parser_state_s* parse);

typedef struct parser_state_s {
    forth_state_t* fs;
    char* buffer;                   // Store the characters being currentely processed
    char* new_word_buffer;          // Store the currently defined word
    char* custom_word_name;         // Stores a copy of the name of the word being defined
    new_word_hook_t new_word_hook;  // Function to call when reaching the end of a word
    new_word_hook_t end_block_hook; // Function to call when reaching the end of a block with " or ;
    sef_stack_t* hooks_stack;       // Stack used to store the previous hooks
    int pnt;                        // Index to the curent portiotion of buffer where we are writing
    bool in_word;                   // Are we writing a word or whitespace
    bool in_def;                    // Are we writing a definition
    char wait_until;                // Char we wait until to process a word. Set to 0 during interpretation
} parser_state_t;

parser_state_t* sef_init_parser(void);
void sef_clean_parser(parser_state_t * parse);
void sef_parser_parse_char(parser_state_t * parse, char ch);
bool sef_parser_is_compiling(parser_state_t* parse);

typedef void (*compile_callback_t)(parser_state_t*, const char*);
void sef_register_compile_time_word(parser_state_t* p, const char* name, compile_callback_t compile_func, char* payload);

#if SEF_USE_SOURCE_FILE
sef_error sef_register_file(parser_state_t * p, const char* filemane);
#endif

#endif

