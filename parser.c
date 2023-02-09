#include "amf_debug.h"
#include "parser.h"
#include "stdio.h"
#include "string.h"

static void run_next_word_hook(parser_state_t* p);
static void register_compile_time_words(parser_state_t* p);

parser_state_t* amf_init_parser(void) {
    parser_state_t* ret = malloc(sizeof(parser_state_t));
    ret->fs = amf_init_state();
    register_compile_time_words(ret);
    ret->buffer = malloc(PARSER_BUFFER_SIZE);
    ret->new_word_buffer = malloc(PARSER_BUFFER_SIZE);
    ret->custom_word_name = malloc(PARSER_CUSTOM_NAME_SIZE);
    ret->new_word_hook = run_next_word_hook;
    ret->pnt = 0;
    ret->in_word = false;
    ret->in_def = false;
    ret->is_in_parenthesis = false;
    ret->is_between_quotes = false;
    ret->is_last_escaped = false;
    ret->wait_for_new_line = false;
    ret->in_defining_constant = false;
    ret->in_defining_variable = false;
    amf_init_io();
#if AMF_REGISTER_FORTH_FUNC
    extern const char* forth_func;
    for (size_t i=0; i<strlen(forth_func); i++) {
        amf_parse_char(ret, forth_func[i]);
    }
#endif
    //amf_display_dictionary(ret->fs->dic);
    return ret;
}

void amf_clean_parser(parser_state_t* parse) {
    amf_clean_io();
    free(parse->custom_word_name);
    free(parse->new_word_buffer);
    free(parse->buffer);
    amf_clean_state(parse->fs);
    free(parse);
}

void amf_parse_char(parser_state_t* parse, char ch) {
    if (parse->wait_for_new_line) {
        if (ch == '\n') {
            parse->wait_for_new_line = false;
        } else {
            return;
        }
    }
    if (ch == '\\' && !parse->is_last_escaped) {
        parse->is_last_escaped = true;
        return;
    }
    if (parse->is_in_parenthesis) {
        if (ch == ')') {
            parse->is_in_parenthesis = false;
        }
        return;
    }
    if (ch == '"' && !parse->is_last_escaped) {
        parse->is_between_quotes = !parse->is_between_quotes;
    }
    if (amf_is_delimiter(ch) && !parse->is_between_quotes) {
        if (parse->is_last_escaped) {
            parse->wait_for_new_line = true;
            parse->is_last_escaped = false;
            return;
        }
        parse->in_word = false;
        parse->buffer[parse->pnt] = 0;
        entry_t compile_time_entry;
        if (amf_find(parse->fs->dic, &compile_time_entry, NULL, amf_hash(parse->buffer)) == OK) {
            if (compile_time_entry.type == compile_word) {
                compile_time_entry.func.compile_func(parse);
            } else {
                parse->new_word_hook(parse);
            }
        } else {
            parse->new_word_hook(parse);
        }
    } else {
        parse->in_word = true;
        parse->buffer[parse->pnt] = ch;
        parse->pnt++;
    }
    parse->is_last_escaped = false;
}

void amf_parse_string(parser_state_t* parse, const char* s) {
    for (size_t i=0; i<strlen(s); i++) {
        amf_parse_char(parse, s[i]);
    }
}

void amf_shell(void) {
    amf_print_string("Starting the ASCminiForth shell.\n");
    parser_state_t* parse = amf_init_parser();
    while (parse->fs->running) {
        amf_parse_char(parse, amf_input());
    }
    amf_clean_parser(parse);
}

#if AMF_USE_SOURCE_FILE
error amf_register_file(parser_state_t* p, const char* filemane) {
    FILE* f = fopen(filemane, "r");
    if (f == NULL) {
        return invalid_file;
    }
    int ch = fgetc(f);
    if (ch == '#') { // We ignore the starting shebang
        p->wait_for_new_line = true;
    }
    while (ch != EOF) {
        amf_parse_char(p, ch);
        ch = fgetc(f);
    }
    fclose(f);
    return OK;
}
#endif

/* ---------------------------- Next words hooks ---------------------------- */

// This hook is the parser's default one, it tries to run the buffer
static void run_next_word_hook(parser_state_t* p) {
    p->pnt = 0;
    word_node_t node_to_exe = amf_compile_node(p->buffer, p->fs->base);
    error execute_rc = amf_executes_node(p->fs, &node_to_exe);
    if (execute_rc == not_found) {
        error_msg("Calling word %s which is not defined.\n", p->buffer);
    } else if (execute_rc != OK) {
        warn_msg("Error n°%i when calling word %s\n", execute_rc, p->buffer);
    }
    amf_run(p->fs);
}

// This hook is the one to use during a definition, add whitespace between words
static void in_def_hook(parser_state_t* p) {
    p->pnt = 0;
    strcat(p->new_word_buffer, p->buffer);
    strcat(p->new_word_buffer, " ");
}

// Hook to use when about to write a words name, first to run when defining a
// new word
static void definition_name_hook(parser_state_t* p) {
    p->pnt = 0;
    strcpy(p->custom_word_name, p->buffer);
    p->new_word_buffer[0] = 0;
    p->new_word_hook = in_def_hook;
}

// This hook is meant to read name of a variable or constant
static void var_const_hook(parser_state_t* p) {
    p->pnt = 0;
#warning TODO: join them
    error (*var_const_def_cb)(const char* name, forth_state_t* fs) =
        (p->in_defining_variable ? amf_compile_variable : 
          (p->in_defining_constant ? amf_compile_constant :
            ( ({error_msg("Calling war_const_hook while defining neither a variable nor a constant.\n"); NULL;}) )));
    var_const_def_cb(p->buffer, p->fs);
    p->in_defining_variable = false;
    p->in_defining_constant = false;
    p->new_word_hook = run_next_word_hook;
}

/* --------------------------- Compile time words --------------------------- */

// (
static void open_par(parser_state_t* p) {
    if (!p->is_between_quotes) {
        p->is_in_parenthesis = true;
        p->pnt--;
    }
}

// :
static void colon(parser_state_t* p) {
    if (p->in_def) {
        error_msg("Using : in a definition is not allowed.\n");
    } else {
        p->in_def = true;
        p->pnt = 0;
        p->new_word_hook = definition_name_hook;
    }
}

// ;
static void semi_colon(parser_state_t* p) {
    if (p->in_def) {
        p->in_def = false;
        p->buffer[p->pnt] = 0;
        p->pnt = 0;
        amf_compile_string(p->fs->dic, p->custom_word_name, p->new_word_buffer, p->fs->base);
        p->new_word_hook = run_next_word_hook;
    } else {
        error_msg("Using ; outside of a definition is not allowed.\n");
    }
}

// constant
static void _constant(parser_state_t* p) {
    p->in_defining_constant = true;
    p->new_word_hook = var_const_hook;
    p->pnt = 0;
}

// variable
static void _variable(struct parser_state_s* p) {
    p->in_defining_variable = true;
    p->new_word_hook = var_const_hook;
    p->pnt = 0;
}

// Register a compile time word
void amf_register_compile_time_word(parser_state_t* p, const char* name, compile_callback_t compile_func) {
    entry_t e;
    e.type = compile_word;
    e.hash = amf_hash(name);
    e.func.compile_func = compile_func;
#if AMF_STORE_NAME
    e.name = malloc(strlen(name) + 1);
    strcpy(e.name, name);
#endif
    amf_add_elem(p->fs->dic, e);
}

struct compile_func_s {
    const char* name;
    compile_callback_t func;
};

struct compile_func_s all_default_compile_words[] = {
    {"(", open_par},
    {":", colon},
    {";", semi_colon},
    {"constant", _constant},
    {"variable", _variable},
};

// Register the previously defined words
static void register_compile_time_words(parser_state_t* p) {
    for (size_t i = 0; i < sizeof(all_default_compile_words) / sizeof(struct compile_func_s); i++) {
        const char* name = all_default_compile_words[i].name;
        amf_register_compile_time_word(p, name, all_default_compile_words[i].func);
#if AMF_CASE_INSENSITIVE == 0   // Register upper case version of the name as well.
        char name_upper[strlen(name) + 1];
        for (size_t j = 0; j <= strlen(name); j++) {
            if ('a' <= name[j] && name[j] <= 'z') {
                name_upper[j] = name[j] - ('a' - 'A');
            } else {
                name_upper[j] = name[j];
            }
        }
        amf_register_compile_time_word(p, name_upper, all_default_compile_words[i].func);
#endif
    }
}

