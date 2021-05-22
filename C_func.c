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

// Stack manipulation

// swap
static void swap(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1);
    amf_push_data(fs, w2);
}

// rot
static void rot(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    word_t w3 = amf_pop_data(fs);
    amf_push_data(fs, w2);
    amf_push_data(fs, w1);
    amf_push_data(fs, w3);
}

// dup
static void dup(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    amf_push_data(fs, w1);
    amf_push_data(fs, w1);
}

// drop
static void drop(forth_state_t* fs){
    amf_pop_data(fs);
}

// Basic maths

// +
static void add(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 + w2);
}

// *
static void mult(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 * w2);
}

// */
static void multDiv(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    word_t d3 = amf_pop_data(fs);
    word_t tmp = d3 * w2;
    amf_push_data(fs, tmp / w1);
}

// */MOD
static void multDivMod(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    word_t d3 = amf_pop_data(fs);
    word_t tmp = d3 * w2;
    amf_push_data(fs, tmp % w1);
    amf_push_data(fs, tmp / w1);
}

// /
static void Div(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 / w1);
}

// /MOD
static void divMod(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    word_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 % w1);
    amf_push_data(fs, w2 / w1);
}

// Boolean logic

// 0<
static void less0(forth_state_t* fs){
    amf_push_data(fs, amf_pop_data(fs) < 0);
}

// 0= 
static void eq0(forth_state_t* fs){
    amf_push_data(fs, amf_pop_data(fs) == 0);
}

// = 
static void eq(forth_state_t* fs){
    amf_push_data(fs, amf_pop_data(fs) == amf_pop_data(fs));
}

// Flow control

// if
static void IF(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    if(!w1){ //If w1 is not true, we want to get to the next else or the next then
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

// Misc

// .
static void printNum(forth_state_t* fs){
    word_t w1 = amf_pop_data(fs);
    amf_print_num(w1);
}

// exit
static void exit_word(forth_state_t* fs){
	amf_exit(fs);
}

// CR
static void cr(forth_state_t* fs){
    amf_output('\n');
}

//Register all the default C_func
void amf_register_default_C_func(forth_state_t* fs){
    // Stack manipulation
    amf_register_cfunc(fs, "swap", swap);
    amf_register_cfunc(fs, "rot", rot);
    amf_register_cfunc(fs, "dup", dup);
    amf_register_cfunc(fs, "drop", drop);
    // Basic math
    amf_register_cfunc(fs, "+", add);
    amf_register_cfunc(fs, "*", mult);
    amf_register_cfunc(fs, "*/", multDiv);
    amf_register_cfunc(fs, "*/mod", multDivMod);
    amf_register_cfunc(fs, "/", Div);
    amf_register_cfunc(fs, "/mod", divMod);
    // Boolean logic
    amf_register_cfunc(fs, "0<", less0);
    amf_register_cfunc(fs, "0=", eq0);
    amf_register_cfunc(fs, "=", eq);
    // Flow control
    amf_register_cfunc(fs, "if", IF);
    amf_register_cfunc(fs, "else", ELSE);
    amf_register_cfunc(fs, "then", then);
    // Misc
    amf_register_cfunc(fs, ".", printNum);
    amf_register_cfunc(fs, "exit", exit_word);
    amf_register_cfunc(fs, "cr", cr);
}

