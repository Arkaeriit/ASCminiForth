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
    ret->forth_memory = malloc(FORTH_MEMORY_SIZE);
    ret->forth_memory_index = 0;
    ret->dic = amf_init_dic();
    amf_register_default_C_func(ret);
    ret->pos.current_word = IDLE_CURRENT_WORD;
    ret->pos.pos_in_word = IDLE_POS_IN_WORD;
    ret->base = 10;
    ret->running = true;
#if AMF_CLI_ARGS
    ret->argc = 0;
    ret->argv = NULL;
#endif
#if AMF_PROGRAMMING_TOOLS
    ret->exit_code = 0;
#endif
    return ret;
}

// Clean the code used by the dictionary
void amf_clean_state(forth_state_t* fs) {
    amf_clean_dic(fs->dic);
    free(fs->forth_memory);
    free(fs->loop_control->stack);
    free(fs->loop_control);
    free(fs->code->stack);
    free(fs->code);
    free(fs->data->stack);
    free(fs->data);
    free(fs);
}

// Puts the state back in an idle state, with all stacks empty and no word
// being executed.
static void __attribute__((unused)) idle_state(forth_state_t* fs) {
    fs->pos.current_word = IDLE_CURRENT_WORD;
    fs->pos.pos_in_word = IDLE_POS_IN_WORD;
    fs->data->stack_pointer = 0;
    fs->code->stack_pointer = 0;
    fs->loop_control->stack_pointer = 0;
}

#if AMF_STACK_BOUND_CHECKS
#define STACK_BOUND_CHECK(stack_name, condition)                               \
    if (fs->stack_name->stack_pointer condition) {                             \
        error_msg("Stack '%s' out of bound. Resetting state.\n", #stack_name); \
        idle_state(fs);                                                        \
    }                                                                           
#else
#define STACK_BOUND_CHECK(x...)
#endif

// Pops the last element from the data stack
amf_int_t amf_pop_data(forth_state_t* fs) {
    STACK_BOUND_CHECK(data, <1);
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
    STACK_BOUND_CHECK(data, >=DATA_STACK_SIZE);
}

// Push a code_pointer element on the code stack
void amf_push_code(forth_state_t* fs, amf_int_t p) {
    debug_msg("push code at index: %zi\n", fs->code->stack_pointer);
    fs->code->stack[fs->code->stack_pointer] = p;
    fs->code->stack_pointer++;
    STACK_BOUND_CHECK(code, >=CODE_STACK_SIZE);
}

// Pop a code_pointer element from the code stack
amf_int_t amf_pop_code(forth_state_t* fs) {
    STACK_BOUND_CHECK(code, <1);
    debug_msg("pop code at index: %zi\n", fs->code->stack_pointer - 1);
    amf_int_t ret = fs->code->stack[fs->code->stack_pointer - 1];
    fs->code->stack_pointer--;
    return ret;
}

// Push a new element in the loop stack
void amf_push_loop(forth_state_t* fs, amf_int_t w) {
    debug_msg("push loop at index: %zi\n", fs->loop_control->stack_pointer);
    fs->loop_control->stack[fs->loop_control->stack_pointer] = w;
    fs->loop_control->stack_pointer++;
    STACK_BOUND_CHECK(loop_control, >=LOOP_STACK_SIZE);
}

// Pops the last element from the loop stack
amf_int_t amf_pop_loop(forth_state_t* fs) {
    STACK_BOUND_CHECK(loop_control, <1);
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
        code_pointer_t previous_pos = amf_int_to_code_pointer(amf_pop_code(fs));
        fs->pos = previous_pos;
        entry_t e;
        amf_find(fs->dic, &e, NULL, fs->pos.current_word);
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
    if (fs->pos.current_word == IDLE_CURRENT_WORD &&   // Nothing to do, we are not executing code
        fs->pos.pos_in_word == IDLE_POS_IN_WORD) {
        debug_msg("Nothing to do, idleing.\n");
        return false;
    }
#warning "Check the size."
    if (fs->pos.pos_in_word >= fs->current_word_copy->size) {  // We return from the function as we reached the end of the word
        amf_exit(fs);
        return true;
    }
    // Otherwize, we run the part of the curent word we are pointing to
    debug_msg("Executing data at pos %li / %li.\n", fs->pos.pos_in_word, fs->current_word_copy->size);
    word_node_t current_node = fs->current_word_copy->content[fs->pos.pos_in_word];
    fs->pos.pos_in_word++;
    amf_executes_node(fs, &current_node);
    return true;
}

// Executes the content of a word_node
#if AMF_CATCH_SEGFAULTS
static error _amf_executes_node(forth_state_t* fs, struct word_node_s* node) {
#else
error amf_executes_node(forth_state_t* fs, struct word_node_s* node) {
#endif
    switch (node->type) {
        case normal_word:
            debug_msg("Calling hash %u from pos %zi.\n", node->content.hash, fs->pos.pos_in_word - 1);
            return amf_call_func(fs, node->content.hash);
        case raw_number:
            amf_push_data(fs, node->content.value);
            return OK;
    }
    error_msg("Invalid node type.\n");
    return impossible_error;
}

#if AMF_CATCH_SEGFAULTS
#include <setjmp.h>
#include <signal.h>
#define UNUSED(x) (void)(x)

error amf_executes_node(forth_state_t* fs, struct word_node_s* node) {
    // Prepare catching of segfaults
    static sigjmp_buf point;
    void handler(int sig, siginfo_t *dont_care, void *dont_care_either) {
        UNUSED(sig);    
        UNUSED(dont_care);    
        UNUSED(dont_care_either);    
        longjmp(point, 1);
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = SA_NODEFER;
    sa.sa_sigaction = handler;
    sigaction(SIGSEGV, &sa, NULL);

    // Execute the risky code
    error ret;
    if (setjmp(point) == 0) {
        ret = _amf_executes_node(fs, node);
    } else {
        error_msg("SEGFAULT. Executing node with hash %i\n", node->content.hash); // I assume that the degfault is is a normal word and not a raw_number. How bold...
#if AMF_STORE_NAME
    entry_t e;
    amf_find(fs->dic, &e, NULL, node->content.hash); // An error check have already been made in _execute_node
    error_msg("The word causing issue is %s\n", e.name);

#endif
        idle_state(fs);
        ret = segfault;
    }

    // Stop the fault catcher
    sa.sa_sigaction = NULL;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGSEGV, &sa, NULL);
    return ret;
}
#endif

// Run the interpreter until it finishes all calls
void amf_run(forth_state_t* fs) {
    while (amf_run_step(fs));
}

// Request some bytes from the forth memory and align the index
void amf_allot(forth_state_t* fs, size_t byte_requested) {
    fs->forth_memory_index += byte_requested;
    while(fs->forth_memory_index % sizeof(amf_int_t)) {
        fs->forth_memory_index++;
    }
#if AMF_STACK_BOUND_CHECKS
    if (fs->forth_memory_index > FORTH_MEMORY_SIZE) {
        error_msg("Forth memory overflowed by %i bytes.\n", fs->forth_memory_index - FORTH_MEMORY_SIZE);
    }
#endif
}

