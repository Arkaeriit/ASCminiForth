#include "ASCminiForth.h"
#ifndef DICTIONARY_H
#define DICTIONARY_H

// Describes the kind of data that can be put in the dictionary
enum entry_type {
    C_word,                     // Words defined in C
    FORTH_word,                 // Words defined in Forth
    compile_word,               // Words that have effect at compile time (such as : or ;)
    constant,                   // Words that are just putting a constant on the stack
    variable,                   // Words that are aliased to a memory address
    string,                     // Strings stored in the dictionary 
};

// This structure represent the entries in the dictionary
typedef struct {
    union {
        C_callback_t C_func;                // To define
        user_amf_int_t* F_word;             // Content of words defined in Forth
        struct {                            // Function to call on the parser
            compile_callback_t func;
            char* payload; // Extra argument to the compile func, should be freeable
        } compile_func;
        amf_int_t constant;                 // Value written in hard
        amf_int_t* variable;                // Data on the Forth memory
        struct {                            // Strings stored in the dictionary
            char* data;
            size_t size;
        } string;
    } func;
    enum entry_type type;
    hash_t hash;
#if AMF_STORE_NAME
    char* name;
#endif
} entry_t;

// This structure represent the dictionary. Should be used as a dynamic array. The values should be sorted 
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
hash_t amf_unused_special_hash(forth_dictionary_t* fd);
hash_t amf_register_string(forth_dictionary_t* fd, const char* str, size_t size);

error amf_call_name(forth_state_t* fs, const char* name);
error amf_call_func(forth_state_t* fs, hash_t hash);

#endif

