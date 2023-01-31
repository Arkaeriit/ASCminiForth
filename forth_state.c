#include "forth_state.h"
#include "amf_debug.h"
#include "string.h"

// Init the interpreter
forth_state_t* amf_init_state(void) {
    forth_state_t* ret = malloc(sizeof(forth_state_t));
    ret->data = malloc(sizeof(data_stack_t));
    ret->data->stack_pointer = 0;
    ret->data->stack = malloc(sizeof(amf_int_t) * DATA_STACK_SIZE);
    ret->code = malloc(sizeof(code_stack_t));
    ret->code->stack_pointer = 0;
    ret->code->stack = malloc(sizeof(code_pointer_t) * CODE_STACK_SIZE);
    ret->loop_control = malloc(sizeof(data_stack_t));
    ret->loop_control->stack_pointer = 0;
    ret->loop_control->stack = malloc(sizeof(amf_int_t) * LOOP_STACK_SIZE);
    ret->dic = amf_init_dic();
    amf_register_default_C_func(ret);
    ret->pos.code.current_word = IDLE_CURRENT_WORD;
    ret->pos.code.pos_in_word = IDLE_POS_IN_WORD;
    ret->base = 10;
    ret->running = true;
    return ret;
}

// Clean the code used by the dictionary
void amf_clean_state(forth_state_t* fs) {
    amf_clean_dic(fs->dic);
    free(fs->loop_control->stack);
    free(fs->loop_control);
    free(fs->code->stack);
    free(fs->code);
    free(fs->data->stack);
    free(fs->data);
    free(fs);
}

// Pops the last element from the data stack
amf_int_t amf_pop_data(forth_state_t* fs) {
    debug_msg("pop data at index: %zi\n", fs->data->stack_pointer - 1);
    amf_int_t ret = fs->data->stack[fs->data->stack_pointer - 1];
    fs->data->stack_pointer--;
    return ret;
}

// Push a new element to the data stack
void amf_push_data(forth_state_t* fs, amf_int_t w) {
    debug_msg("push data at index: %zi\n", fs->data->stack_pointer);
    fs->data->stack[fs->data->stack_pointer] = w;
    fs->data->stack_pointer++;
}

// Push a code_pointer element on the code stack
void amf_push_code(forth_state_t* fs, code_pointer_t p) {
    debug_msg("push code at index: %zi\n", fs->code->stack_pointer);
    fs->code->stack[fs->code->stack_pointer] = p;
    fs->code->stack_pointer++;
}

// Pop a code_pointer element from the code stack
code_pointer_t amf_pop_code(forth_state_t* fs) {
    debug_msg("pop code at index: %zi\n", fs->code->stack_pointer - 1);
    code_pointer_t ret = fs->code->stack[fs->code->stack_pointer - 1];
    fs->code->stack_pointer--;
    return ret;
}

// Push a new element in the loop stack
void amf_push_loop(forth_state_t* fs, amf_int_t w) {
    debug_msg("push loop at index: %zi\n", fs->loop_control->stack_pointer);
    fs->loop_control->stack[fs->loop_control->stack_pointer] = w;
    fs->loop_control->stack_pointer++;
}

// Pops the last element from the loop stack
amf_int_t amf_pop_loop(forth_state_t* fs) {
    debug_msg("pop loop at index: %zi\n", fs->loop_control->stack_pointer - 1);
    amf_int_t ret = fs->loop_control->stack[fs->loop_control->stack_pointer - 1];
    fs->loop_control->stack_pointer--;
    return ret;
}

// Look at the last element from the loop stack
amf_int_t amf_peek_loop(forth_state_t* fs) {
    amf_int_t ret = fs->loop_control->stack[fs->loop_control->stack_pointer - 1];
    return ret;
}

// Return from a word_call
void amf_exit(forth_state_t* fs) {
    if (fs->code->stack_pointer > 0) {  // In a custom word
        debug_msg("Returning.\n");
        code_pointer_t previous_pos = amf_pop_code(fs);
        fs->pos = previous_pos;
        entry_t e;
        amf_find(fs->dic, &e, NULL, fs->pos.code.current_word);
        fs->current_word_copy = e.func.F_word;
    } else {    // In the shell/top level
        debug_msg("Exiting.\n");
        fs->running = false;
    }
}

// Run a single step of user Forth code, if needed
// Return false if there is nothing to do
// Return true if there is something to do
bool amf_run_step(forth_state_t* fs) {
    if (fs->pos.code.current_word == IDLE_CURRENT_WORD &&   // Nothing to do, we are not executing code
        fs->pos.code.pos_in_word == IDLE_POS_IN_WORD) {
        debug_msg("Nothing to do, idleing.\n");
        return false;
    }
    if (fs->pos.code.pos_in_word >= fs->current_word_copy->size) {  // We return from the function as we reached the end of the word
        amf_exit(fs);
        return true;
    }
    // Otherwize, we run the part of the curent word we are pointing to
    debug_msg("Executing data at pos %li / %li.", fs->pos.code.pos_in_word, fs->current_word_copy->size);
    word_node_t current_node = fs->current_word_copy->content[fs->pos.code.pos_in_word];
    fs->pos.code.pos_in_word++;
    amf_executes_node(fs, &current_node);
    return true;
}

// Executes the content of a word_node
error amf_executes_node(forth_state_t* fs, struct word_node_s* node) {
    switch (node->type) {
        case normal_word:
            debug_msg("Calling hash %u from pos %zi.\n", node->content.hash, fs->pos.code.pos_in_word - 1);
            return amf_call_func(fs, node->content.hash);
        case raw_number:
            amf_push_data(fs, node->content.value);
            return OK;
        case printed_string:
            amf_print_string(node->content.string);
            return OK;
        case forth_string:
            amf_push_data(fs, (amf_int_t) node->content.string);
            amf_push_data(fs, (amf_int_t) strlen(node->content.string));
            return OK;
    }
    error_msg("Invalid node type.\n");
    return impossible_error;
}

// Run the interpreter until it finishes all calls
void amf_run(forth_state_t* fs) {
    while (amf_run_step(fs));
}

