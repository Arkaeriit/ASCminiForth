#include "forth_state.h"

forth_state_t* ft_init(void) {
    forth_state_t* ret = malloc(sizeof(forth_state_t));
    ret->data = malloc(sizeof(data_stack_t));
    ret->data->stack_pointer = 0;
    ret->data->stack = malloc(sizeof(word_t) * DATA_STACK_SIZE);
    ret->code = malloc(sizeof(code_stack_t));
    ret->code->stack_pointer = 0;
    ret->code->stack = malloc(sizeof(code_pointer_t) * CODE_STACK_SIZE);
    ret->dic = fd_init();
    return ret;
}

void ft_clean(forth_state_t* fs){
    fd_clean(fs->dic);
    free(fs->code->stack);
    free(fs->code);
    free(fs->data->stack);
    free(fs->data);
    free(fs);
}

