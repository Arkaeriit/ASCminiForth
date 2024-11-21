#include "forth_state.h"
#include "sef_debug.h"
#include "string.h"

// Init the interpreter
forth_state_t* sef_init_state(struct parser_state_s* parser) {
    forth_state_t* ret = malloc(sizeof(forth_state_t));
    ret->parser = parser;
    ret->data = sef_stack_init(SEF_DATA_STACK_SIZE);
    ret->code = sef_stack_init(SEF_CODE_STACK_SIZE);
    ret->forth_memory = malloc(FORTH_MEMORY_SIZE);
    ret->forth_memory_index = 0;
    ret->pad = malloc(SEF_PAD_SIZE);
    ret->dic = sef_init_dic();
    sef_register_default_C_func(ret);
    ret->pos.current_word = SEF_IDLE_CURRENT_WORD;
    ret->pos.pos_in_word = SEF_IDLE_POS_IN_WORD;
    ret->base = 10;
    ret->running = true;
#if SEF_CLI_ARGS
    ret->argc = 0;
    ret->argv = NULL;
#endif
#if SEF_PROGRAMMING_TOOLS
    ret->exit_code = 0;
#endif
    return ret;
}

// Clean the code used by the dictionary
void sef_clean_state(forth_state_t* fs) {
    sef_clean_dic(fs->dic);
    free(fs->pad);
    free(fs->forth_memory);
    sef_stack_free(fs->code);
    sef_stack_free(fs->data);
    free(fs);
}

// Puts the state back in an idle state, with all stacks but the data stack
// empty and no word being executed.
void sef_quit(forth_state_t* fs) {
    fs->pos.current_word = SEF_IDLE_CURRENT_WORD;
    fs->pos.pos_in_word = SEF_IDLE_POS_IN_WORD;
    fs->code->stack_pointer = 0;
}

// Like quit but also reset the data stack. Display the stack trace if needed.
void sef_abort(forth_state_t* fs) {
#if SEF_STACK_TRACE
    sef_stack_trace(fs);
#endif
    sef_quit(fs);
    fs->data->stack_pointer = 0;
}

#if SEF_STACK_BOUND_CHECKS
#define STACK_BOUND_CHECK(fs, stack_name)                                      \
    if (!sef_state_state_valid(fs->stack_name)) {                              \
        error_msg("Stack '%s' out of bound. Resetting state.\n", #stack_name); \
        sef_abort(fs);                                                         \
    }                                                                           
#else
#define STACK_BOUND_CHECK(x...)
#endif

#define STACK_POP(fs, stack_name) ({               \
    debug_msg("pop on %s at index: %zi\n",         \
            #stack_name,                           \
            fs->stack_name->stack_pointer);        \
    sef_int_t ret = sef_stack_pop(fs->stack_name); \
    STACK_BOUND_CHECK(fs, stack_name);             \
    ret;                                           \
})                                                  

#define STACK_PUSH(fs, stack_name, w)       \
    debug_msg("push on %s at index: %zi\n", \
            #stack_name,                    \
            fs->stack_name->stack_pointer); \
    sef_stack_push(fs->stack_name, w);      \
    STACK_BOUND_CHECK(fs, stack_name);       

// Pops the last element from the data stack
sef_int_t sef_pop_data(forth_state_t* fs) {
    return STACK_POP(fs, data);
}

// Push a new element to the data stack
void sef_push_data(forth_state_t* fs, sef_int_t w) {
    STACK_PUSH(fs, data, w);
}

// Push a code_pointer element on the code stack
void sef_push_code(forth_state_t* fs, sef_int_t p) {
    STACK_PUSH(fs, code, p);
}

// Pop a code_pointer element from the code stack
sef_int_t sef_pop_code(forth_state_t* fs) {
    return STACK_POP(fs, code);
}

// Look at the last element from the loop stack
sef_int_t sef_peek_loop(forth_state_t* fs, int loop_depth) {
    return sef_stack_peek(fs->code, loop_depth * 2);
}

// Return from a word_call
void sef_exit(forth_state_t* fs) {
    if (fs->code->stack_pointer > 0) {  // In a custom word
        debug_msg("Returning.\n");
        code_pointer_t previous_pos = sef_int_to_code_pointer(sef_pop_code(fs));
        fs->pos = previous_pos;
        entry_t e;
        sef_find(fs->dic, &e, NULL, fs->pos.current_word);
        fs->current_word_copy = e.func.F_word;
    } else {    // In the shell/top level
        debug_msg("Exiting.\n");
        fs->running = false;
    }
}

// Run a single step of user Forth code, if needed
// Return false if there is nothing to do
// Return true if there is something to do
bool sef_run_step(forth_state_t* fs) {
    if (fs->pos.current_word == SEF_IDLE_CURRENT_WORD &&   // Nothing to do, we are not executing code
        fs->pos.pos_in_word == SEF_IDLE_POS_IN_WORD) {
        debug_msg("Nothing to do, idleing.\n");
        return false;
    }
    if (fs->pos.pos_in_word >= fs->current_word_copy->size) {  // We return from the function as we reached the end of the word
        sef_exit(fs);
        return true;
    }
    // Otherwize, we run the part of the curent word we are pointing to
    debug_msg("Executing data at pos %li / %li.\n", fs->pos.pos_in_word, fs->current_word_copy->size);
    word_node_t current_node = fs->current_word_copy->content[fs->pos.pos_in_word];
    fs->pos.pos_in_word++;
    sef_executes_node(fs, &current_node);
    return true;
}

// Executes the content of a word_node
#if SEF_CATCH_SEGFAULTS
static error _sef_executes_node(forth_state_t* fs, struct word_node_s* node) {
#else
error sef_executes_node(forth_state_t* fs, struct word_node_s* node) {
#endif
    switch (node->type) {
        case normal_word:
            debug_msg("Calling hash %u from pos %zi.\n", node->content.hash, fs->pos.pos_in_word - 1);
            return sef_call_func(fs, node->content.hash);
        case raw_number:
            sef_push_data(fs, node->content.value);
            return OK;
    }
    error_msg("Invalid node type.\n");
    return impossible_error;
}

#if SEF_CATCH_SEGFAULTS
#include <setjmp.h>
#include <signal.h>
#define UNUSED(x) (void)(x)

static sigjmp_buf point;
static void segfault_handler(int sig, siginfo_t *dont_care, void *dont_care_either) {
    UNUSED(sig);
    UNUSED(dont_care);
    UNUSED(dont_care_either);
    longjmp(point, 1);
}

error sef_executes_node(forth_state_t* fs, struct word_node_s* node) {
    // Prepare catching of segfaults
    struct sigaction sa;
    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = SA_NODEFER;
    sa.sa_sigaction = segfault_handler;
    sigaction(SIGSEGV, &sa, NULL);

    // Execute the risky code
    error ret;
    if (setjmp(point) == 0) {
        ret = _sef_executes_node(fs, node);
    } else {
        error_msg("SEGFAULT. Executing node with hash %i\n", node->content.hash); // I assume that the degfault is is a normal word and not a raw_number. How bold...
#if SEF_STORE_NAME
    entry_t e;
    sef_find(fs->dic, &e, NULL, node->content.hash); // An error check have already been made in _execute_node
    error_msg("The word causing issue is %s\n", e.name);

#endif
        sef_abort(fs);
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
void sef_run(forth_state_t* fs) {
    while (sef_run_step(fs));
}

// Request some bytes from the forth memory and align the index
void sef_allot(forth_state_t* fs, size_t byte_requested) {
    fs->forth_memory_index += byte_requested;
#if SEF_STACK_BOUND_CHECKS
    if (fs->forth_memory_index > FORTH_MEMORY_SIZE) {
        error_msg("Forth memory overflowed by %i bytes.\n", fs->forth_memory_index - FORTH_MEMORY_SIZE);
    }
#endif
}

#if SEF_STACK_TRACE
static void display_element_in_stack_trace(forth_state_t* fs, code_pointer_t code) {
    hash_t word_hash = code.current_word;
    entry_t e;
    if (sef_find(fs->dic, &e, NULL, word_hash) == OK) {
#if SEF_STORE_NAME
        error_msg(" * %s\n", e.name);
#else
        error_msg(" * %"PRIxPTR"\n", e.hash);
#endif
    } else {
        error_msg(" * ???\n");
    }
}

void sef_stack_trace(forth_state_t* fs) {
    error_msg("Stack trace:\n");
    display_element_in_stack_trace(fs, fs->pos);
    for (ssize_t _pointer=((ssize_t)fs->code->stack_pointer)-1; _pointer>0; _pointer--) {
        size_t stack_pointer = _pointer;
        sef_int_t stack_element = fs->code->stack[stack_pointer];
        display_element_in_stack_trace(fs, sef_int_to_code_pointer(stack_element));
    }
}
#endif

