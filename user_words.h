#include "ASCminiForth.h"
#ifndef USER_WORDS_H
#define USER_WORDS_H

#include "parser.h"

// This struct represent an user-define word. It is a list of all the
// word_node that will be called when the word is called.
typedef struct user_word_s {
    word_node_t* content;
    size_t size;
} user_amf_int_t;

error amf_compile_user_word(struct forth_dictionary_s* fd, const char* name, size_t subword_n, char** subwords, int base, hash_t force_hash);
error amf_compile_string(struct forth_dictionary_s* fs, const char* name, const char* str, int base, hash_t force_hash);
word_node_t amf_compile_node(const char* str, int base);
bool amf_is_delimiter(char ch);
void amf_clean_user_word(user_amf_int_t * w);

error amf_compile_constant(const char* name, forth_state_t* fs);
error amf_register_defer(const char* name, forth_state_t* fs);

#endif

