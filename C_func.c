#include "amf_debug.h"
#include "C_func.h"
#include "string.h"
#include "stdio.h"
#include "utils.h"

#define UNUSED(x) (void)(x)

// Functions used to manipulate C_fun

// Register a new C function
void amf_register_cfunc(forth_state_t* fs, const char* name, C_callback_t func) {
    entry_t e;
    e.type = C_word;
    e.hash = amf_hash(name);
    e.func.C_func = func;
#if AMF_STORE_NAME
    e.name = malloc(strlen(name) + 1);
    strcpy(e.name, name);
#endif
    amf_add_elem(fs->dic, e);
}

// List of default C_func

// Stack manipulation

// swap
static void swap(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1);
    amf_push_data(fs, w2);
}

// rot
static void rot(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_int_t w3 = amf_pop_data(fs);
    amf_push_data(fs, w2);
    amf_push_data(fs, w1);
    amf_push_data(fs, w3);
}

// dup
static void dup(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_push_data(fs, w1);
    amf_push_data(fs, w1);
}

// drop
static void drop(forth_state_t* fs) {
    amf_pop_data(fs);
}

// r>
static void r_from(forth_state_t* fs) {
    code_pointer_t data = amf_pop_code(fs);
    amf_push_data(fs, data.optional_data);
}

// >r
static void to_r(forth_state_t* fs) {
    amf_int_t w = amf_pop_data(fs);
    code_pointer_t to_push = {.optional_data = w};
    amf_push_code(fs, to_push);
}

// roll
static void roll(forth_state_t* fs) {
    amf_int_t pos = amf_pop_data(fs);
    for (amf_int_t i=0; i<pos; i++) {
        to_r(fs);
    }
    amf_int_t to_top = amf_pop_data(fs);
    for (amf_int_t i=0; i<pos; i++) {
        r_from(fs);
    }
    amf_push_data(fs, to_top);
}

// pick
static void pick(forth_state_t* fs) {
    amf_int_t pos = amf_pop_data(fs);
    amf_push_data(fs,
            fs->data->stack[fs->data->stack_pointer - pos - 1]);
}

// Basic maths

// +
static void add(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 + w2);
}

// -
static void sub(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 - w1);
}

// *
static void mult(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 * w2);
}

// */
static void multDiv(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_int_t d3 = amf_pop_data(fs);
    amf_int_t tmp = d3 * w2;
    amf_push_data(fs, tmp / w1);
}

// */MOD
static void multDivMod(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_int_t d3 = amf_pop_data(fs);
    amf_int_t tmp = d3 * w2;
    amf_push_data(fs, tmp % w1);
    amf_push_data(fs, tmp / w1);
}

// /
static void Div(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 / w1);
}

// /MOD
static void divMod(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 % w1);
    amf_push_data(fs, w2 / w1);
}

// abs
static void abs_word(forth_state_t* fs) {
    amf_int_t w = amf_pop_data(fs);
    w = w < 0 ? -w : w;
    amf_push_data(fs, w);
}

// <
static void less_than(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w2 < w1);
}

// Boolean logic

// 0<
static void less0(forth_state_t* fs) {
    amf_push_data(fs, amf_pop_data(fs) < 0);
}

// 0= 
static void eq0(forth_state_t* fs) {
    amf_push_data(fs, amf_pop_data(fs) == 0);
}

// = 
static void eq(forth_state_t* fs) {
    amf_push_data(fs, amf_pop_data(fs) == amf_pop_data(fs));
}

// and
static void and(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 & w2);
}

// or
static void or(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 | w2);
}

// xor
static void xor(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    amf_int_t w2 = amf_pop_data(fs);
    amf_push_data(fs, w1 ^ w2);
}

// Flow control

#define CHECK_BEING_IN_WORD(fs)                                             \
    if (fs->pos.code.current_word == IDLE_CURRENT_WORD) {                   \
        error_msg("Control flow impossible outside of word definition.\n"); \
        return;                                                             \
    }                                                                        

// Check the i-th word in the current executed word against two hashes
#define CHECK_AGAINST_HASH(fs, i, hash1, hash2) ({                \
    (fs->current_word_copy->content[i].type == normal_word) &&    \
    ((fs->current_word_copy->content[i].content.hash == hash1) || \
    (fs->current_word_copy->content[i].content.hash == hash2)); }) 

// if
static void IF(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    amf_int_t w1 = amf_pop_data(fs);
    if (!w1) {  // If w1 is not true, we want to get to the next else or the next then
        hash_t else_hash = amf_hash("else");
        hash_t then_hash = amf_hash("then");
        hash_t if_hash = amf_hash("if");
        hash_t ELSE_hash = amf_hash("ELSE");
        hash_t THEN_hash = amf_hash("THEN");
        hash_t IF_hash = amf_hash("IF");
        size_t i = fs->pos.code.pos_in_word + 1;
        int if_depth = 1;
        while (if_depth) {
            if (CHECK_AGAINST_HASH(fs, i, else_hash, ELSE_hash) || CHECK_AGAINST_HASH(fs, i, then_hash, THEN_hash)) {
                if_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, if_hash, IF_hash)) {
                if_depth++;
            }
            i++;
        }
        fs->pos.code.pos_in_word = i;
    }
}

// else
// If we meet that word, it means that we were in an if block, thus we need to jump to the next then
static void ELSE(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    hash_t then_hash = amf_hash("then");
    hash_t if_hash = amf_hash("if");
    hash_t THEN_hash = amf_hash("THEN");
    hash_t IF_hash = amf_hash("IF");
    size_t i = fs->pos.code.pos_in_word + 1;
    int if_depth = 1;
    while (if_depth) {  // Note: not finding matching then cause a fault
        if (CHECK_AGAINST_HASH(fs, i, then_hash, THEN_hash)) {
            if_depth--;
        } else if (CHECK_AGAINST_HASH(fs, i, if_hash, IF_hash)) {
            if_depth++;
        }
        i++;
    }
    fs->pos.code.pos_in_word = i;
}

// then
static void then(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
};

// begin
static void begin(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
};

// until
static void until(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    if (!amf_pop_data(fs)) {
        // Jumping to the corresponding until
        hash_t begin_hash = amf_hash("begin");
        hash_t until_hash = amf_hash("until");
        hash_t BEGIN_hash = amf_hash("BEGIN");
        hash_t UNTIL_hash = amf_hash("UNTIL");
        size_t i = fs->pos.code.pos_in_word - 2;
        int loop_depth = 1;
        while (loop_depth) {    // Note: if no matching begin is found, there is a fault
            if (CHECK_AGAINST_HASH(fs, i, begin_hash, BEGIN_hash)) {
                loop_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, until_hash, UNTIL_hash)) {
                loop_depth++;
            }
            i--;
        }
        fs->pos.code.pos_in_word = i + 1;
    }
}

// do
static void DO(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    amf_int_t start_index = amf_pop_data(fs);
    amf_int_t end_index = amf_pop_data(fs);
    amf_push_loop(fs, end_index);
    amf_push_loop(fs, start_index);
}

// I
static void I(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    amf_push_data(fs, amf_peek_loop(fs));
}

// +loop
static void plus_loop(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    amf_int_t current_index = amf_pop_loop(fs);
    amf_int_t end_index = amf_pop_loop(fs);
    amf_int_t increment = amf_pop_data(fs);
    debug_msg("loop % 4li % 4li % 4li % 4zi.\n", current_index, end_index, increment, fs->loop_control->stack_pointer);
    current_index += increment;
    if ( ((increment >= 0) && (current_index < end_index)) || ((increment < 0) && (current_index >= end_index)) ) { // Increment of 0 should do an infinite loop
        amf_push_loop(fs, end_index);
        amf_push_loop(fs, current_index);
        // Jumping to the corresponding do
        hash_t do_hash = amf_hash("do");
        hash_t loop_hash = amf_hash("loop");
        hash_t plus_loop_hash = amf_hash("+loop");
        hash_t DO_hash = amf_hash("DO");
        hash_t LOOP_hash = amf_hash("LOOP");
        hash_t plus_LOOP_hash = amf_hash("+LOOP");
        size_t i = fs->pos.code.pos_in_word - 2;
        int loop_depth = 1;
        while (loop_depth) {    // Note: if no matching begin is found, there is a fault
            if (CHECK_AGAINST_HASH(fs, i, do_hash, DO_hash)) {
                loop_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, loop_hash, LOOP_hash) || CHECK_AGAINST_HASH(fs, i, plus_loop_hash, plus_LOOP_hash)) {
                loop_depth++;
            }
            i--;
        }
        fs->pos.code.pos_in_word = i + 2;
    }
}

// loop TODO: When macro will be available, this should be made with one
static void loop(forth_state_t* fs) {
    amf_push_data(fs, 1);
    plus_loop(fs);
}

// Memory management

// cells
static void cells(forth_state_t* fs) {
    amf_push_data(fs, amf_pop_data(fs) * sizeof(amf_int_t));
}

// here
// This word is here fore compatibility with other Forth dialect but as the memory management of this dialect is different, it doesn't make sense to have a here word
static void here(forth_state_t* fs) {
    UNUSED(fs);
}

// free
static void FREE(forth_state_t* fs) {
    free((void *) amf_pop_data(fs));
}

// allot
static void allot(forth_state_t* fs) {
    amf_int_t size = amf_pop_data(fs);
    amf_push_data(fs, (amf_int_t) malloc(size));
}

// @
static void fetch(forth_state_t* fs) {
    amf_int_t* addr = (amf_int_t *) amf_pop_data(fs);
    amf_push_data(fs, *addr);
}

// !
static void store(forth_state_t* fs) {
    amf_int_t* addr = (amf_int_t *) amf_pop_data(fs);
    amf_int_t data = amf_pop_data(fs);
    *addr = data;
}

// c@
static void cfetch(forth_state_t* fs) {
    char* addr = (char *) amf_pop_data(fs);
    amf_push_data(fs, (amf_int_t) * addr);
}

// c!
static void cstore(forth_state_t* fs) {
    char* addr = (char *) amf_pop_data(fs);
    amf_int_t data = amf_pop_data(fs);
    *addr = (char) data;
}


// C strings

// print
static void put_str(forth_state_t* fs) {
    char* str = (char *) amf_pop_data(fs);
    amf_print_string(str);
}

// strlen
static void str_len(forth_state_t* fs) {
    char* str = (char *) amf_pop_data(fs);
    amf_push_data(fs, (amf_int_t) strlen(str));
}


// Misc

// .
static void printNum(forth_state_t* fs) {
    amf_int_t w1 = amf_pop_data(fs);
    char buff[AMF_MAX_NUMBER_DIGIT];
    char* str = amf_base_format(w1, buff, fs->base);
    amf_print_string(str);
    amf_print_string(" ");
}

// emit
static void emit(forth_state_t* fs) {
    amf_int_t w = amf_pop_data(fs);
    amf_output(w);
}

// key
static void key(forth_state_t* fs) {
    amf_int_t w = amf_input();
    amf_push_data(fs, w);
}

// exit
static void exit_word(forth_state_t* fs) {
    amf_exit(fs);
}

// abort
static void abort_word(forth_state_t* fs) {
    fs->running = false;
}

// CR
static void cr(forth_state_t* fs) {
    UNUSED(fs);
    amf_output('\n');
}

// base
static void base(forth_state_t* fs) {
    amf_int_t* base_pnt = &fs->base;
    amf_push_data(fs, (amf_int_t) base_pnt);
}

struct c_func_s {
    const char* name;
    void (*func)(forth_state_t*);
};

struct c_func_s all_default_c_func[] = {
    // Stack manipulation
    {"swap", swap},
    {"rot", rot},
    {"dup", dup},
    {"drop", drop},
    {">r", to_r},
    {"r>", r_from},
    {"roll", roll},
    {"pick", pick},
    // Basic math
    {"+", add},
    {"-", sub},
    {"*", mult},
    {"*/", multDiv},
    {"*/mod", multDivMod},
    {"/", Div},
    {"/mod", divMod},
    {"abs", abs_word},
    {"<", less_than},
    // Boolean logic
    {"0<", less0},
    {"0=", eq0},
    {"=", eq},
    {"and", and},
    {"or", or},
    {"xor", xor},
    // Flow control
    {"if", IF},
    {"else", ELSE},
    {"then", then},
    {"begin", begin},
    {"until", until},
    {"do", DO},
    {"i", I},
    {"+loop", plus_loop},
    {"loop", loop},
    // Memory management
    {"allot", allot},
    {"cells", cells},
    {"here", here},
    {"free", FREE},
    {"@", fetch},
    {"!", store},
    {"c@", cfetch},
    {"c!", cstore},
    // C strings
    {"print", put_str},
    {"strlen", str_len},
    // Misc
    {".", printNum},
    {"emit", emit},
    {"key", key},
    {"exit", exit_word},
    {"abort", abort_word},
    {"cr", cr},
    {"base", base},
};

// Register all the default C_func
void amf_register_default_C_func(forth_state_t* fs) {
    for (size_t i = 0; i < sizeof(all_default_c_func) / sizeof(struct c_func_s); i++) {
        const char* name = all_default_c_func[i].name;
        amf_register_cfunc(fs, name, all_default_c_func[i].func);
#if AMF_CASE_INSENSITIVE == 0   // Register upper case version of the name as well.
        char name_upper[strlen(name) + 1];
        for (size_t j = 0; j <= strlen(name); j++) {
            if ('a' <= name[j] && name[j] <= 'z') {
                name_upper[j] = name[j] - ('a' - 'A');
            } else {
                name_upper[j] = name[j];
            }
        }
        amf_register_cfunc(fs, name_upper, all_default_c_func[i].func);
#endif
    }
}

