
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "user_words.h"
#include "stdlib.h"

//Describes the kind of data that can be put inthe dictionary
enum entry_type {
    C_word,      //Words defined in C
    FORTH_word,  //Words defined in Forth
    compile_word //Words that have effetct at compile time (such as : or ;)
};

//This structure represent the entries in the dictionary
typedef struct {
    union {
        runtime_callback_t C_func; //To define
        user_word_t F_word; //Content of wors defined in Forth
        compile_callback_t Compile_func; //To define or delete
    } func;
    enum entry_type type;
    hash_t hash;
} entry_t;

//This structure represent the dictionary. Should be used as a bynamic array. The values should be sorted 
typedef struct {
    entry_t* entries;
    size_t n_entries;
    size_t max;
} forth_dictionary_t;

forth_dictionary_t* fd_init(void);
void fd_clean(forth_dictionary_t* fd);
void fd_add_elem(entry_t e);

#endif

