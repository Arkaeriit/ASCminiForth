#include "ASCminiForth.h"
#ifndef USER_WORDS_H
#define USER_WORDS_H

#include "parser.h"
typedef void (*compile_callback_t)(parser_state_t*);

// This struct represent an user-define word. It is a list of all the
// word_node that will be called when the word is called.
typedef struct user_word_s {
    word_node_t* content;
    size_t size;
} user_amf_int_t;

error amf_compile_user_word(struct forth_dictionary_s* fd, const char* name, size_t subword_n, char** subwords, int base);
error amf_compile_string(struct forth_dictionary_s* fs, const char* name, const char* str, int base);
word_node_t amf_compile_node(const char* str, int base);
bool amf_is_delimiter(char ch);
void amf_clean_user_word(user_amf_int_t * w);

error amf_compile_constant_or_variable(const char* name, forth_state_t* fs, bool is_constant);

#endif

