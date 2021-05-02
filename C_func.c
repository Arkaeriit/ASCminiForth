#include "C_func.h"
#include "stdio.h"

//Functions used to manipulate C_fun

//Call a known valid C_func from the dictionary
void amf_call_cfunc(forth_state_t* fs, hash_t hash){
    entry_t e;
    amf_find(fs->dic, &e, NULL, hash);
    e.func.C_func(fs);
}

//Register a new C function
void amf_register_cfunc(forth_state_t* fs, const char* name, C_callback_t func){
    entry_t e;
    e.type = C_word;
    e.hash = amf_hash(name);
    e.func.C_func = func;
    amf_add_elem(fs->dic, e);
}

//List of default C_func

// +
static void add(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    amf_push_data(fs, d1 + d2);
}

// .
static void printNum(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    printf("%" WORD_PRINT " ",d1);
}

// *
static void mult(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    amf_push_data(fs, d1 * d2);
}

// */
static void multDiv(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    word_t d3 = amf_pop_data(fs);
    double_word_t tmp = d3 * d2;
    amf_push_data(fs, tmp / d1);
}

// */MOD
static void multDivMod(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    word_t d3 = amf_pop_data(fs);
    double_word_t tmp = d3 * d2;
    amf_push_data(fs, tmp / d1);
    amf_push_data(fs, tmp % d1);
}

// /
static void Div(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    amf_push_data(fs, d2 / d1);
}

// /MOD
static void divMod(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    word_t d2 = amf_pop_data(fs);
    amf_push_data(fs, d2 / d1);
    amf_push_data(fs, d2 % d1);
}

// exit
static void exit_word(forth_state_t* fs){
	amf_exit(fs);
}

// if
static void IF(forth_state_t* fs){
    word_t d1 = amf_pop_data(fs);
    if(!d1){ //If d1 is not true, we want to get to the next else or the next then
        hash_t else_hash = amf_hash("else");        
        hash_t then_hash = amf_hash("then");        
        size_t i=fs->pos.code.pos_in_word+1;
        while(fs->current_word_copy->content[i].content.hash != else_hash && fs->current_word_copy->content[i].content.hash != then_hash){
           i++;
        } 
        fs->pos.code.pos_in_word = i + 1;
    }
}

// else
// If we meet that word, it means that we were in an if block, thus we need to jump to the next then
static void ELSE(forth_state_t* fs){
    hash_t then_hash = amf_hash("then");        
    size_t i=fs->pos.code.pos_in_word+1;
    while(fs->current_word_copy->content[i].content.hash != then_hash){
       i++;
    } 
    fs->pos.code.pos_in_word = i + 1;
}

// then
static void then(forth_state_t* fs){};

//Register all the default C_func
void amf_register_default_C_func(forth_state_t* fs){
    amf_register_cfunc(fs, "+", add);
    amf_register_cfunc(fs, ".", printNum);
    amf_register_cfunc(fs, "*", mult);
    amf_register_cfunc(fs, "*/", multDiv);
    amf_register_cfunc(fs, "*/mod", multDivMod);
    amf_register_cfunc(fs, "/", Div);
    amf_register_cfunc(fs, "/mod", divMod);
    amf_register_cfunc(fs, "exit", exit_word);
    amf_register_cfunc(fs, "if", IF);
    amf_register_cfunc(fs, "else", ELSE);
    amf_register_cfunc(fs, "then", then);
}

