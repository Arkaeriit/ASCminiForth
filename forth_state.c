#include "forth_state.h"

//Init the interpreter
forth_state_t* amf_init_state(void) {
    forth_state_t* ret = malloc(sizeof(forth_state_t));
    ret->data = malloc(sizeof(data_stack_t));
    ret->data->stack_pointer = 0;
    ret->data->stack = malloc(sizeof(word_t) * DATA_STACK_SIZE);
    ret->code = malloc(sizeof(code_stack_t));
    ret->code->stack_pointer = 0;
    ret->code->stack = malloc(sizeof(code_pointer_t) * CODE_STACK_SIZE);
    ret->dic = amf_init_dic();
    amf_register_default_C_func(ret);
    return ret;
}

//Clean the code used by the dictionary
void amf_clean_state(forth_state_t* fs){
    amf_clean_dic(fs->dic);
    free(fs->code->stack);
    free(fs->code);
    free(fs->data->stack);
    free(fs->data);
    free(fs);
}

//Pops the last element from the data stack
word_t amf_pop_data(forth_state_t* fs){
    word_t ret = fs->data->stack[fs->data->stack_pointer-1];
    fs->data->stack_pointer--;
    return ret;
}

//Push a new element to the data stack
void amf_push_data(forth_state_t* fs, word_t w){
    fs->data->stack[fs->data->stack_pointer] = w;
    fs->data->stack_pointer++;
}

