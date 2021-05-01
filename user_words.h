#include "ASCminiForth.h"
#ifndef USER_WORDS_H
#define USER_WORDS_H

//Types to change/remove in the future
typedef int compile_callback_t;

//Define what kind of content con be found in a word definition
enum word_type {
    normal_word,   //Words found in the dictionary
    raw_number, //User-inputed word such as 3, 6, 8
};

//This struct rpresunt each words called in a word definition
typedef struct {
    enum word_type type;
    union {
        hash_t hash;        //To find the word in the dictionary
        word_t value;       //When used as a raw number
    } content;
} word_node_t;

//This struct represent an user-define word. It is a list of all the
//word_node that will be called when the word is called.
typedef struct user_word_s {
    word_node_t* content;
    size_t size;
} user_word_t;

error amf_compile_user_word(struct forth_dictionary_s* fd, const char* name, size_t subword_n, const char** subwords);
error amf_compile_string(struct forth_dictionary_s* fs, const char* name, const char* str);
bool amf_is_delimiter(char ch);

#endif

