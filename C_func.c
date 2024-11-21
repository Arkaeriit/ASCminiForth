#include "sef_debug.h"
#include "C_func.h"
#include "string.h"
#include "stdio.h"
#include "utils.h"

#define UNUSED(x) (void)(x)
#define FORTH_TRUE ((sef_int_t) ~0)
#define FORTH_BOOL(x) ((x) ? FORTH_TRUE : 0)

// Functions used to manipulate C_fun

// Register a new C function
void sef_register_cfunc(forth_state_t* fs, const char* name, C_callback_t func) {
    entry_t e;
    e.type = C_word;
    e.hash = sef_hash(name);
    e.func.C_func = func;
#if SEF_STORE_NAME
    e.name = malloc(strlen(name) + 1);
    strcpy(e.name, name);
#endif
    sef_add_elem(fs->dic, e, name);
}

// List of default C_func

// Stack manipulation

// swap
static void swap(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1);
    sef_push_data(fs, w2);
}

// rot
static void rot(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_int_t w3 = sef_pop_data(fs);
    sef_push_data(fs, w2);
    sef_push_data(fs, w1);
    sef_push_data(fs, w3);
}

// dup
static void DUP(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_push_data(fs, w1);
    sef_push_data(fs, w1);
}

// drop
static void drop(forth_state_t* fs) {
    sef_pop_data(fs);
}

// r>
static void r_from(forth_state_t* fs) {
    sef_int_t data = sef_pop_code(fs);
    sef_push_data(fs, data);
}

// >r
static void to_r(forth_state_t* fs) {
    sef_int_t w = sef_pop_data(fs);
    sef_push_code(fs, w);
}

// roll
static void roll(forth_state_t* fs) {
    sef_int_t pos = sef_pop_data(fs);
    for (sef_int_t i=0; i<pos; i++) {
        to_r(fs);
    }
    sef_int_t to_top = sef_pop_data(fs);
    for (sef_int_t i=0; i<pos; i++) {
        r_from(fs);
    }
    sef_push_data(fs, to_top);
}

// pick
static void pick(forth_state_t* fs) {
    sef_int_t pos = sef_pop_data(fs);
    sef_push_data(fs,
            fs->data->stack[fs->data->stack_pointer - pos - 1]);
}

// depth
static void depth(forth_state_t* fs) {
    sef_push_data(fs, fs->data->stack_pointer);
}

// Basic maths

// +
static void add(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1 + w2);
}

// -
static void sub(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w2 - w1);
}

// SM/REM
static void sm_slash_rem(forth_state_t* fs) {
    // C gives symetric division
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_int_t quotient = w2 / w1;
    sef_int_t rem = w2 % w1;
    sef_push_data(fs, rem);
    sef_push_data(fs, quotient);
}

// FM/MOD
static void fm_slash_mod(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_int_t quotient = w2 / w1;
    sef_int_t rem = w2 % w1;
    if (w1 * w2 < 0 && rem != 0) {
        rem *= -1;
        rem += rem >= 0 ? 1 : -1;
        quotient += quotient >= 0 ? 1 : -1;
    }
    sef_push_data(fs, rem);
    sef_push_data(fs, quotient);
}

// U/MOD
static void u_slash_mod(forth_state_t* fs) {
    sef_unsigned_t w1 = (sef_unsigned_t) sef_pop_data(fs);
    sef_unsigned_t w2 = (sef_unsigned_t) sef_pop_data(fs);
    sef_unsigned_t quotient = w2 / w1;
    sef_unsigned_t rem = w2 % w1;
    sef_push_data(fs, (sef_int_t) rem);
    sef_push_data(fs, (sef_int_t) quotient);
}

// *
static void mult(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1 * w2);
}

// abs
static void abs_word(forth_state_t* fs) {
    sef_int_t w = sef_pop_data(fs);
    w = w < 0 ? -w : w;
    sef_push_data(fs, w);
}

// <
static void less_than(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, FORTH_BOOL(w2 < w1));
}

// U<
static void u_less_than(forth_state_t* fs) {
    sef_unsigned_t w1 = (sef_unsigned_t) sef_pop_data(fs);
    sef_unsigned_t w2 = (sef_unsigned_t) sef_pop_data(fs);
    sef_push_data(fs, FORTH_BOOL(w2 < w1));
}

// Boolean logic

// 0<
static void less0(forth_state_t* fs) {
    sef_push_data(fs, FORTH_BOOL(sef_pop_data(fs) < 0));
}

// 0= 
static void eq0(forth_state_t* fs) {
    sef_push_data(fs, FORTH_BOOL(sef_pop_data(fs) == 0));
}

// = 
static void eq(forth_state_t* fs) {
    sef_push_data(fs, FORTH_BOOL(sef_pop_data(fs) == sef_pop_data(fs)));
}

// and
static void and(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1 & w2);
}

// or
static void or(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1 | w2);
}

// xor
static void xor(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    sef_push_data(fs, w1 ^ w2);
}

// Flow control

#define CHECK_BEING_IN_WORD(fs)                                             \
    if (fs->pos.current_word == SEF_IDLE_CURRENT_WORD) {                    \
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
    sef_int_t w1 = sef_pop_data(fs);
    if (!w1) {  // If w1 is not true, we want to get to the next else or the next then
        hash_t else_hash = sef_hash("else");
        hash_t then_hash = sef_hash("then");
        hash_t if_hash = sef_hash("if");
        hash_t ELSE_hash = sef_hash("ELSE");
        hash_t THEN_hash = sef_hash("THEN");
        hash_t IF_hash = sef_hash("IF");
        size_t i = fs->pos.pos_in_word;
        int if_depth = 1;
        while (if_depth) {
            if (CHECK_AGAINST_HASH(fs, i, if_hash, IF_hash)) {
                if_depth++;
            } else if (CHECK_AGAINST_HASH(fs, i, then_hash, THEN_hash)) {
                if_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, else_hash, ELSE_hash) && (if_depth == 1)) { // Elses are a valid exit point only if we are not in a nested if
                if_depth--;
            }
            i++;
        }
        fs->pos.pos_in_word = i;
    }
}

// else
// If we meet that word, it means that we were in an if block, thus we need to jump to the next then
static void ELSE(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    hash_t then_hash = sef_hash("then");
    hash_t if_hash = sef_hash("if");
    hash_t THEN_hash = sef_hash("THEN");
    hash_t IF_hash = sef_hash("IF");
    size_t i = fs->pos.pos_in_word;
    int if_depth = 1;
    while (if_depth) {  // Note: not finding matching then cause a fault
        if (CHECK_AGAINST_HASH(fs, i, then_hash, THEN_hash)) {
            if_depth--;
        } else if (CHECK_AGAINST_HASH(fs, i, if_hash, IF_hash)) {
            if_depth++;
        }
        i++;
    }
    fs->pos.pos_in_word = i;
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
    if (!sef_pop_data(fs)) {
        // Jumping to the corresponding until
        hash_t begin_hash = sef_hash("begin");
        hash_t until_hash = sef_hash("until");
        hash_t BEGIN_hash = sef_hash("BEGIN");
        hash_t UNTIL_hash = sef_hash("UNTIL");
        size_t i = fs->pos.pos_in_word - 2;
        int loop_depth = 1;
        while (loop_depth) {    // Note: if no matching begin is found, there is a fault
            if (CHECK_AGAINST_HASH(fs, i, begin_hash, BEGIN_hash)) {
                loop_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, until_hash, UNTIL_hash)) {
                loop_depth++;
            }
            i--;
        }
        fs->pos.pos_in_word = i + 1;
    }
}

// do
static void DO(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    sef_int_t start_index = sef_pop_data(fs);
    sef_int_t end_index = sef_pop_data(fs);
    sef_push_code(fs, end_index);
    sef_push_code(fs, start_index);
}

// I
static void I(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    sef_push_data(fs, sef_peek_loop(fs, 0));
}

// J
static void J(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    sef_push_data(fs, sef_peek_loop(fs, 1));
}

// +loop
static void plus_loop(forth_state_t* fs) {
    CHECK_BEING_IN_WORD(fs);
    sef_unsigned_t current_index = sef_pop_code(fs);
    sef_unsigned_t end_index = sef_pop_code(fs);
    sef_int_t increment = sef_pop_data(fs);
    debug_msg("loop % 4li % 4li % 4li % 4zi.\n", current_index, end_index, increment, fs->code->stack_pointer);
    current_index += increment;
    if ( ((increment >= 0) && (current_index < end_index)) || ((increment < 0) && (current_index >= end_index)) ) { // Increment of 0 should do an infinite loop
        sef_push_code(fs, end_index);
        sef_push_code(fs, current_index);
        // Jumping to the corresponding do
        hash_t do_hash = sef_hash("do");
        hash_t q_do_hash = sef_hash("?do");
        hash_t plus_loop_hash = sef_hash("+loop");
        hash_t DO_hash = sef_hash("DO");
        hash_t Q_DO_hash = sef_hash("?DO");
        hash_t plus_LOOP_hash = sef_hash("+LOOP");
        size_t i = fs->pos.pos_in_word - 2;
        int loop_depth = 1;
        while (loop_depth) {    // Note: if no matching do is found, there is a fault
            if (CHECK_AGAINST_HASH(fs, i, do_hash, DO_hash) || CHECK_AGAINST_HASH(fs, i, q_do_hash, Q_DO_hash)) {
                loop_depth--;
            } else if (CHECK_AGAINST_HASH(fs, i, plus_loop_hash, plus_LOOP_hash)) {
                loop_depth++;
            }
            i--;
        }
        fs->pos.pos_in_word = i + 2;
    }
}

// unloop
static void unloop(forth_state_t* fs) {
    sef_pop_code(fs);
    sef_pop_code(fs);
}

// leave
static void leave(forth_state_t* fs) {
    unloop(fs);
    hash_t do_hash = sef_hash("do");
    hash_t q_do_hash = sef_hash("?do");
    hash_t plus_loop_hash = sef_hash("+loop");
    hash_t DO_hash = sef_hash("DO");
    hash_t Q_DO_hash = sef_hash("?DO");
    hash_t plus_LOOP_hash = sef_hash("+LOOP");
    size_t i = fs->pos.pos_in_word;
    int loop_depth = 1;
    while (loop_depth) {
        if (CHECK_AGAINST_HASH(fs, i, do_hash, DO_hash) || CHECK_AGAINST_HASH(fs, i, q_do_hash, Q_DO_hash)) {
            loop_depth++;
        } else if (CHECK_AGAINST_HASH(fs, i, plus_loop_hash, plus_LOOP_hash)) {
            loop_depth--;
        }
        i++;
    }
    fs->pos.pos_in_word = i;
}

// ?do
static void question_do(forth_state_t* fs) {
    sef_int_t w1 = sef_pop_data(fs);
    sef_int_t w2 = sef_pop_data(fs);
    if (w1 == w2) {
        sef_push_code(fs, w2);
        sef_push_code(fs, w1);
        leave(fs);
    } else {
        sef_push_data(fs, w2);
        sef_push_data(fs, w1);
        DO(fs);
    }
}

// of
static void of(forth_state_t* fs) {
    sef_int_t test_case = sef_pop_data(fs);
    sef_int_t value = sef_pop_data(fs);
    if (test_case != value) {
        hash_t endof_hash = sef_hash("endof");
        hash_t ENDOF_hash = sef_hash("ENDOF");
        hash_t endcase_hash = sef_hash("endcase");
        hash_t ENDCASE_hash = sef_hash("ENDCASE");
        hash_t case_hash = sef_hash("case");
        hash_t CASE_hash = sef_hash("CASE");
        int case_depth = 0;
        size_t i = fs->pos.pos_in_word;
        while ((!CHECK_AGAINST_HASH(fs, i, endof_hash, ENDOF_hash)) || (case_depth > 0)) {
            if (CHECK_AGAINST_HASH(fs, i, case_hash, CASE_hash)) {
                case_depth++;
            } else if (CHECK_AGAINST_HASH(fs, i, endcase_hash, ENDCASE_hash)) {
                case_depth--;
            }
            i++;
        }
        fs->pos.pos_in_word = i+1;
        sef_push_data(fs, value);
    }
}

// endof
static void endof(forth_state_t* fs) {
    hash_t endcase_hash = sef_hash("endcase");
    hash_t ENDCASE_hash = sef_hash("ENDCASE");
    hash_t case_hash = sef_hash("case");
    hash_t CASE_hash = sef_hash("CASE");
    int case_depth = 1;
    size_t i = fs->pos.pos_in_word;
    while (case_depth) {
        if (CHECK_AGAINST_HASH(fs, i, case_hash, CASE_hash)) {
            case_depth++;
        } else if (CHECK_AGAINST_HASH(fs, i, endcase_hash, ENDCASE_hash)) {
            case_depth--;
        }
        i++;
    }
    fs->pos.pos_in_word = i;
}

// Memory management

// cells
static void cells(forth_state_t* fs) {
    sef_push_data(fs, sef_pop_data(fs) * sizeof(sef_int_t));
}

// here
static void here(forth_state_t* fs) {
    sef_push_data(fs, ((sef_int_t) fs->forth_memory) + fs->forth_memory_index);
}

// allot
static void allot(forth_state_t* fs) {
    sef_int_t size = sef_pop_data(fs);
    sef_allot(fs, size);
}

// allocate
static void allocate(forth_state_t* fs) {
    sef_int_t size = sef_pop_data(fs);
    char* mem = malloc(size);
    sef_push_data(fs, (sef_int_t) mem);
    sef_push_data(fs, mem == NULL);
}

// free
static void FREE(forth_state_t* fs) {
    free((void *) sef_pop_data(fs));
    sef_push_data(fs, 0);
}

// resize
static void resize(forth_state_t* fs) {
    sef_int_t new_size = sef_pop_data(fs);
    void* mem = (void*) sef_pop_data(fs);
    void* new_mem = realloc(mem, new_size);
    sef_push_data(fs, (sef_int_t) new_mem);
    sef_push_data(fs, new_mem == NULL);
}

// @
static void fetch(forth_state_t* fs) {
    sef_int_t* addr = (sef_int_t *) sef_pop_data(fs);
    sef_push_data(fs, *addr);
}

// !
static void store(forth_state_t* fs) {
    sef_int_t* addr = (sef_int_t *) sef_pop_data(fs);
    sef_int_t data = sef_pop_data(fs);
    *addr = data;
}

// c@
static void cfetch(forth_state_t* fs) {
    char* addr = (char *) sef_pop_data(fs);
    sef_push_data(fs, (sef_int_t) * addr);
}

// c!
static void cstore(forth_state_t* fs) {
    char* addr = (char *) sef_pop_data(fs);
    sef_int_t data = sef_pop_data(fs);
    *addr = (char) data;
}

// unused
static void unused(forth_state_t* fs) {
    sef_push_data(fs, FORTH_MEMORY_SIZE - fs->forth_memory_index);
}


// C strings

// print
static void put_str(forth_state_t* fs) {
    char* str = (char *) sef_pop_data(fs);
    sef_print_string(str);
}

// strlen
static void str_len(forth_state_t* fs) {
    char* str = (char *) sef_pop_data(fs);
    sef_push_data(fs, (sef_int_t) strlen(str));
}

#if SEF_CLI_ARGS
// Arguments

// argc
static void argc(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) &fs->argc);
}

// arg
static void arg(forth_state_t* fs) {
    sef_int_t index = sef_pop_data(fs);
    sef_push_data(fs, (sef_int_t) fs->argv[index]);
    sef_push_data(fs, (sef_int_t) strlen(fs->argv[index]));
}
#endif

#if SEF_FILE
// File manipulation

enum file_modes {
    m_ro = 1,
    m_wo = 2,
    m_rw = 3,
};

static const char* file_modes_to_create(enum file_modes fm) {
    switch (fm) {
        case m_wo:
            return "w";
        case m_rw:
            return "w+";
        default:
            error_msg("Invalid mode for file creation.");
            return NULL;
    }
}

static const char* file_modes_to_open(enum file_modes fm) {
    switch (fm) {
        case m_wo:
            return "a";
        case m_rw:
            return "a+";
        case m_ro:
            return "r";
        default:
            error_msg("Invalid mode for file opening.");
            return NULL;
    }
}

// r/o
static void ro(forth_state_t* fs) {
    sef_push_data(fs, m_ro);
}

// w/o
static void wo(forth_state_t* fs) {
    sef_push_data(fs, m_wo);
}

// r/w
static void rw(forth_state_t* fs) {
    sef_push_data(fs, m_rw);
}

// create-file and open-file generic
static void file_action(forth_state_t* fs, const char* (*mode_f)(enum file_modes)) {
    enum file_modes mode = sef_pop_data(fs);
    sef_int_t filename_size = sef_pop_data(fs);
    const char* filename_forth = (const char*) sef_pop_data(fs);
    char filename[filename_size+1]; // Convert forth string to C string
    memcpy(filename, filename_forth, filename_size);
    filename[filename_size] = 0;
    FILE* ret = fopen(filename, mode_f(mode));
    sef_push_data(fs, (sef_int_t) ret);
    sef_push_data(fs, ret == NULL);
}

// create-file
static void create_file(forth_state_t* fs) {
    file_action(fs, file_modes_to_create);
}

// open-file
static void open_file(forth_state_t* fs) {
    file_action(fs, file_modes_to_open);
}

// close-file
static void close_file(forth_state_t* fs) {
    FILE* f = (FILE*) sef_pop_data(fs);
    fclose(f);
    sef_push_data(fs, 0);
}

// read-file
static void read_file(forth_state_t* fs) {
    FILE* f = (FILE*) sef_pop_data(fs);
    size_t size = sef_pop_data(fs);
    char* dest = (char*) sef_pop_data(fs);
    size_t ret = fread(dest, 1, size, f);
    sef_push_data(fs, ret);
    sef_push_data(fs, ret <= 0);
}

// write-file
static void write_file(forth_state_t* fs) {
    FILE* f = (FILE*) sef_pop_data(fs);
    size_t size = sef_pop_data(fs);
    char* source = (char*) sef_pop_data(fs);
    size_t written = fwrite(source, 1, size, f);
    sef_push_data(fs, written != size);
}

// read-line
static void read_line(forth_state_t* fs) {
    FILE* f = (FILE*) sef_pop_data(fs);
    size_t size = sef_pop_data(fs);
    char* dest = (char*) sef_pop_data(fs);
    size_t dest_index = 0;
    bool eof = false;
    for (size_t i=0; i<size; i++) {
        int c = getc(f);
        switch (c) {
            case EOF:
                eof = true;
                goto endloop;
            case '\n':
                dest[dest_index] = c;
                goto endloop;
            default:
                dest[dest_index++] = c;
                break;
        }
    }
endloop:
    sef_push_data(fs, dest_index);
    if (dest_index == 0 && eof) { // Bad, no char were written; flag = false, ior = ~0
        sef_push_data(fs, 0);
        sef_push_data(fs, FORTH_TRUE);
    } else { // Good: flag true, ior = 0
        sef_push_data(fs, 1);
        sef_push_data(fs, 0);
    }
}

// write-line
static void write_line(forth_state_t* fs) {
    FILE* f = (FILE*) sef_pop_data(fs);
    size_t size = sef_pop_data(fs);
    const char* source = (char*) sef_pop_data(fs);
    size_t written = fwrite(source, size, 1, f);
    written += fwrite("\n", 1, 1, f);
    sef_push_data(fs, written != (size + 1));
}

// stdin
static void _stdin(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) stdin);
}

// stdout
static void _stdout(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) stdout);
}

// stderr
static void _stderr(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) stderr);
}
#endif

#if SEF_PROGRAMMING_TOOLS
// Programming tools

// exit-code
static void exit_code(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) &fs->exit_code);
}

// bye
static void bye(forth_state_t* fs) {
    fs->running = false;
    fs->pos.pos_in_word = SEF_IDLE_POS_IN_WORD;
    fs->pos.current_word = SEF_IDLE_CURRENT_WORD;
}

// words
static void words(forth_state_t* fs) {
    sef_display_dictionary(fs->dic);
}
#endif

// Misc

// emit
static void emit(forth_state_t* fs) {
    sef_int_t w = sef_pop_data(fs);
    sef_output(w);
}

// key
static void key(forth_state_t* fs) {
    sef_int_t w = sef_input();
    sef_push_data(fs, w);
}

// exit
static void exit_word(forth_state_t* fs) {
    sef_exit(fs);
}

// CR
static void cr(forth_state_t* fs) {
    UNUSED(fs);
    sef_output('\n');
}

// base
static void base(forth_state_t* fs) {
    sef_int_t* base_pnt = &fs->base;
    sef_push_data(fs, (sef_int_t) base_pnt);
}

// execute
static void execute(forth_state_t* fs) {
    hash_t exec_tocken = sef_pop_data(fs);
    sef_call_func(fs, exec_tocken);
}

// Parse a word and ensure that the execution will stop right after it.
// This is needed to ensure correct behavior when using evaluate recursively.
static void extra_safe_parse(forth_state_t* fs, char c) {
    code_pointer_t pos_copy = fs->pos;
    fs->pos.current_word = SEF_IDLE_CURRENT_WORD;
    fs->pos.pos_in_word = SEF_IDLE_POS_IN_WORD;
    struct sef_compiled_forth_word_s* current_word_copy = fs->current_word_copy;

    sef_parse_char(fs->parser, c);

    fs->pos = pos_copy;
    fs->current_word_copy = current_word_copy;
}

// evaluate
static void evaluate(forth_state_t* fs) {
    size_t len = (size_t) sef_pop_data(fs);
    const char* str = (const char *) sef_pop_data(fs);
    for (size_t i=0; i<len; i++) {
        extra_safe_parse(fs, str[i]);
    }
}

// Recurse
static void recurse(forth_state_t* fs) {
    sef_call_func(fs, fs->pos.current_word);
}

// pad
static void pad(forth_state_t* fs) {
    sef_push_data(fs, (sef_int_t) fs->pad);
}

// defer@
static void defer_fetch(forth_state_t* fs) {
    hash_t exec_tocken = (hash_t) sef_pop_data(fs);
    entry_t entry;
    if (sef_find(fs->dic, &entry, NULL, exec_tocken) != OK || entry.type != alias) {
        error_msg("Using defer@ on invalid value.");
        sef_abort(fs);
    }
    sef_push_data(fs, (sef_int_t) entry.func.alias_to);
}

// defer!
static void defer_store(forth_state_t* fs) {
    hash_t alias_token = (hash_t) sef_pop_data(fs);
    hash_t alias_to_token = (hash_t) sef_pop_data(fs);
    if (sef_set_alias(fs->dic, alias_token, alias_to_token, "from defer!") != OK) {
        error_msg("Using defer! on invalid values.");
    }
}

// Given a string, return true if i's a valid query for environment?
// Also put in the return pointer the constant to put on the stack.
static bool environment_reply(const char* query, size_t size, sef_int_t* ret) {
    if (!strncmp(query, "/COUNTED-STRING", size)) {
        *ret = 0xFF;
    } else if (!strncmp(query, "/HOLD", size)) {
#warning "Use a constant"
        *ret = 64;
    } else if (!strncmp(query, "/PAD", size)) {
        *ret = SEF_PAD_SIZE;
    } else if (!strncmp(query, "ADDRESS-UNIT-BITS", size)) {
        *ret = sizeof(char) * 8;
    } else if (!strncmp(query, "FLOORED", size)) {
        *ret = false; // We use sm/rem to define other math functions
    } else if (!strncmp(query, "MAX-CHAR", size)) {
        *ret = 0xFF;
    } else if (!strncmp(query, "MAX-D", size)) {
        *ret = ((sef_unsigned_t) ~0) >> 1;
    } else if (!strncmp(query, "MAX-N", size)) {
        *ret = ((sef_unsigned_t) ~0) >> 1;
    } else if (!strncmp(query, "MAX-U", size)) {
        *ret = ~0;
    } else if (!strncmp(query, "MAX-UD", size)) {
        *ret = ~0;
    } else if (!strncmp(query, "RETURN-STACK-CELLS", size)) {
        *ret = SEF_CODE_STACK_SIZE;
    } else if (!strncmp(query, "STACK-CELLS", size)) {
        *ret = SEF_DATA_STACK_SIZE;
    } else {
        return false;
    }
    return true;
}

// environment?
static void environment_query(forth_state_t* fs) {
    size_t size = (size_t) sef_pop_data(fs);
    const char* str = (const char*) sef_pop_data(fs);
    sef_int_t ret;
    bool query_ret = environment_reply(str, size, &ret);
    sef_push_data(fs, FORTH_BOOL(query_ret));
    if (query_ret) {
        sef_push_data(fs, ret);
    }
}


struct c_func_s {
    const char* name;
    void (*func)(forth_state_t*);
};

struct c_func_s all_default_c_func[] = {
    // Stack manipulation
    {"swap", swap},
    {"rot", rot},
    {"dup", DUP},
    {"drop", drop},
    {">r", to_r},
    {"r>", r_from},
    {"roll", roll},
    {"pick", pick},
    {"depth", depth},
    // Basic math
    {"+", add},
    {"-", sub},
    {"*", mult},
    {"sm/rem", sm_slash_rem},
    {"fm/mod", fm_slash_mod},
    {"u/mod", u_slash_mod},
    {"abs", abs_word},
    {"<", less_than},
    {"u<", u_less_than},
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
    {"j", J},
    {"+loop", plus_loop},
    {"unloop", unloop},
    {"leave", leave},
    {"?do", question_do},
    {"of", of},
    {"endof", endof},
    // Memory management
    {"allot", allot},
    {"cells", cells},
    {"here", here},
    {"allocate", allocate},
    {"free", FREE},
    {"resize", resize},
    {"@", fetch},
    {"!", store},
    {"c@", cfetch},
    {"c!", cstore},
    {"unused", unused},
    // C strings
    {"print", put_str},
    {"strlen", str_len},
#if SEF_CLI_ARGS
    // Arguments
    {"argc", argc},
    {"arg", arg},
#endif    
#if SEF_FILE
    // File manipulation
#warning TODO: test and document
    {"r/o", ro},
    {"r/w", rw},
    {"w/o", wo},
    {"create-file", create_file},
    {"open-file", open_file},
    {"close-file", close_file},
    {"read-file", read_file},
    {"write-file", write_file},
    {"read-line", read_line},
    {"write-line", write_line},
    {"stdin", _stdin},
    {"stdout", _stdout},
    {"stderr", _stderr},
#endif
#if SEF_PROGRAMMING_TOOLS
    // Programming tools
    {"exit-code", exit_code},
    {"bye", bye},
    {"words", words},
#endif
    // Misc
    {"emit", emit},
    {"key", key},
    {"exit", exit_word},
    {"abort", sef_abort},
    {"quit", sef_quit},
    {"cr", cr},
    {"base", base},
    {"execute", execute},
    {"(evaluate)", evaluate},
    {"recurse", recurse},
    {"pad", pad},
    {"defer@", defer_fetch},
    {"defer!", defer_store},
    {"environment?", environment_query},
};

// Register all the default C_func
void sef_register_default_C_func(forth_state_t* fs) {
    for (size_t i = 0; i < sizeof(all_default_c_func) / sizeof(struct c_func_s); i++) {
        const char* name = all_default_c_func[i].name;
        sef_register_cfunc(fs, name, all_default_c_func[i].func);
    }
}

