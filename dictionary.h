
#ifndef DICTIONARY_H
#define DICTIONARY_H

enum entry_type {
    C_word,      //Words defined in C
    FORTH_word,  //Words defined in Forth
    compile_word //Words that have effetct at compile time (such as : or ;)
};

typedef struct {
    enum entry_type type;
    union {
        runtime_callback_t C_func; //To define
        user_word_t F_word; //Content of wors defined in Forth
        compile_callback_t Compile_func; //To define or delete
    } func;
} entry_t;

typedef struct {
    entry_t* entries;
    size_t n_entries;
    size_t max;
} forth_dictionary;

#endif

