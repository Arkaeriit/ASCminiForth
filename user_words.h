#include "ASCminiForth.h"
#ifndef USER_WORDS_H
#define USER_WORDS_H

//Types to change in the future
typedef int special_t;
typedef int compile_callback_t;

//Define what kind of content con be found in a word definition
enum word_type {
    normal_word, //Words found in the dictionary
    special_words, //Other words such as 1, ." coucou" or flow control
};

//This struct rpresunt each words called in a word definition
typedef struct {
    enum word_type type;
    union {
        hash_t hash;        //To find the word in the dictionary
        special_t* special; //To do the needed special effect. To define
    } content;
} word_node_t;

//This struct represent an user-define word. It is a list of all the
//word_node that will be called when the word is called.
typedef struct user_word_s {
    word_node_t* content;
    size_t size;
} user_word_t;

#endif

