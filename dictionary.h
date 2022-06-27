#include "ASCminiForth.h"
#ifndef DICTIONARY_H
#define DICTIONARY_H

// Describes the kind of data that can be put inthe dictionary
enum entry_type {
    C_word,                     // Words defined in C
    FORTH_word,                 // Words defined in Forth
    compile_word                // Words that have effetct at compile time (such as : or ;)
};

// This structure represent the entries in the dictionary
typedef struct {
    union {
        C_callback_t C_func;                // To define
        user_amf_int_t* F_word;             // Content of wors defined in Forth
        compile_callback_t Compile_func;    //To define or delete
    } func;
    enum entry_type type;
    hash_t hash;
#if AMF_STORE_NAME
    char* name;
#endif
} entry_t;

// This structure represent the dictionary. Should be used as a bynamic array. The values should be sorted 
typedef struct forth_dictionary_s {
    entry_t* entries;
    size_t n_entries;
    size_t max;
} forth_dictionary_t;

forth_dictionary_t* amf_init_dic(void);
void amf_display_dictionary(forth_dictionary_t* dic);
void amf_clean_dic(forth_dictionary_t* fd);
error amf_find(forth_dictionary_t* fd, entry_t* e, size_t* index, hash_t hash);
error amf_add_elem(forth_dictionary_t* fd, entry_t e);

error amf_call_name(forth_state_t* fs, const char* name);
error amf_call_func(forth_state_t* fs, hash_t hash);

#endif

