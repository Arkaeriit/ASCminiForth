#include "C_func.h"
#include "stdio.h"

//Functions used to manipulate C_fun

//Call a known valid C_func from the dictionary
void amf_call_cfunc(forth_state_t* fs, hash_t hash){
    entry_t e;
    fd_find(fs->dic, &e, NULL, hash);
    e.func.C_func(fs);
}

//Register a new C function
void amf_register_cfunc(forth_state_t* fs, const char* name, C_callback func){
    entry_t e;
    e.type = C_word;
    e.hash = forth_hash(name);
    e.func.C_func = func;
    fs_add_elem(fs->dic, e);
}

//List of default C_func
static void add(forth_state_t* fs){
    word_t d1 = ft_pop_data(fs);
    word_t d2 = ft_pop_data(fs);
    ft_push_data(d1 + d2);
}

static void printNum(forth_state_t* fs){
    word_t d1 = ft_pop_data(fs);
    printf(WORD_PRINT " ",d1);
}

static void push1(forth_state_t* fs){
    ft_push_data(1);
}

//Register all the default C_func
void amf_register_default_C_func(forth_state_t* fs){
    cf_register_cfunc(fs, "+", add);
    cf_register_cfunc(fs, ".", printNum);
    cf_register_cfunc(fs, "1", push1);
}

