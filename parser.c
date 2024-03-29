#include "amf_debug.h"
#include "utils.h"
#include "parser.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

static void run_next_word_hook(parser_state_t* p);
static void invalid_hook(parser_state_t* p);
static void register_compile_time_words_list(parser_state_t* p);
static void macro(parser_state_t* p, const char* payload);

parser_state_t* amf_init_parser(void) {
    parser_state_t* ret = malloc(sizeof(parser_state_t));
    ret->fs = amf_init_state();
    register_compile_time_words_list(ret);
    ret->buffer = malloc(PARSER_BUFFER_SIZE);
    ret->new_word_buffer = malloc(PARSER_BUFFER_SIZE);
    ret->custom_word_name = malloc(PARSER_CUSTOM_NAME_SIZE);
    ret->new_word_hook = run_next_word_hook;
    ret->end_block_hook = invalid_hook;
    ret->pnt = 0;
    ret->in_word = false;
    ret->in_def = false;
    ret->is_in_parenthesis = false;
    ret->is_between_quotes = false;
    ret->wait_for_new_line = false;
    amf_init_io();
    extern const char* base_forth_func;
    amf_parse_string(ret, base_forth_func);
#if AMF_FILE
    extern const char* file_forth_func;
    amf_parse_string(ret, file_forth_func);
#endif
#if AMF_STRING
    extern const char* string_forth_func;
    amf_parse_string(ret, string_forth_func);
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
    if (parse->is_in_parenthesis) {
        if (ch == ')') {
            parse->is_in_parenthesis = false;
        }
        return;
    }
    if (parse->is_between_quotes) {
        if (ch == '"') {
            parse->buffer[parse->pnt] = 0;
            parse->is_between_quotes = false;
            parse->end_block_hook(parse);
        } else {
            parse->buffer[parse->pnt] = ch;
            parse->pnt++;
        }
    } else if (amf_is_delimiter(ch)) {
        if (!parse->in_word) {
            return;
        }
        parse->in_word = false;
        parse->buffer[parse->pnt] = 0;
        entry_t compile_time_entry;
        if (amf_find(parse->fs->dic, &compile_time_entry, NULL, amf_hash(parse->buffer)) == OK) {
            if (compile_time_entry.type == compile_word) {
                compile_time_entry.func.compile_func.func(parse, compile_time_entry.func.compile_func.payload);
                return;
            }
        }
        parse->new_word_hook(parse);
    } else {
        parse->in_word = true;
        parse->buffer[parse->pnt] = ch;
        parse->pnt++;
    }
}

void amf_parse_string(parser_state_t* parse, const char* s) {
    for (size_t i=0; i<strlen(s); i++) {
        amf_parse_char(parse, s[i]);
    }
}

int amf_shell(void) {
    amf_print_string("Starting the ASCminiForth shell.\n");
    parser_state_t* parse = amf_init_parser();
    while (parse->fs->running) {
        char ch = amf_input();
        if (ch == 4) { // End of transmission
            break;
        }
        amf_parse_char(parse, ch);
    }
    int rc = 0;
#if AMF_PROGRAMMING_TOOLS
    rc = parse->fs->exit_code;
#endif
    amf_clean_parser(parse);
    return rc;
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

#define PUSH_HOOK(p, hook_name)                                           \
    code_pointer_t to_push = {.optional_data = (amf_int_t) p->hook_name}; \
    amf_push_code(p->fs, to_push)                                          

#define POP_HOOK(p, hook_name) p->hook_name = (new_word_hook_t) amf_pop_code(p->fs).optional_data

#define UNUSED(x) (void)(x)

// Error when no hooks registered
static void invalid_hook(parser_state_t* p) {
    UNUSED(p);
    error_msg("Invalid hook should not be run.\n");
}

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
    amf_compile_constant_or_variable(p->buffer, p->fs, p->in_defining_constant);
    p->new_word_hook = run_next_word_hook;
}

// Replace the new word with its exec token
static void get_exec_token_hook(parser_state_t* p) {
    hash_t hash = amf_hash(p->buffer);
    char buffer[64];
    char* hash_as_string = amf_base_format(hash, buffer, p->fs->base);
    strcpy(p->buffer, hash_as_string);
    POP_HOOK(p, new_word_hook);
    p->new_word_hook(p);
}

// Register a string
static void register_string_hook(parser_state_t* p) {
    const char* str = p->buffer + 2; // As of now, the string always start 3 chars after the initializing word. But the first whitespace have been ignored.
    size_t size = strlen(str);
    hash_t str_id = amf_register_string(p->fs->dic, str, size);
    char string_type = p->buffer[0];
    char tmp[AMF_MAX_NUMBER_DIGIT];
    snprintf(p->buffer, PARSER_BUFFER_SIZE, "%s", amf_base_format(str_id, tmp, p->fs->base));
    p->new_word_hook(p);
    snprintf(p->buffer, strlen("execute")+1, "execute");
    p->new_word_hook(p);
    switch (string_type) {
        case 's':
        case 'S':
            break;
        case '.':
            snprintf(p->buffer, strlen("type")+1, "type");
            p->new_word_hook(p);
            break;
        default:
            error_msg("Unknown string type %c\n", string_type);
    }
    POP_HOOK(p, end_block_hook);
}

// Register a normal word definition
static void register_def_hook(parser_state_t* p) {
    amf_compile_string(p->fs->dic, p->custom_word_name, p->new_word_buffer, p->fs->base);
}

// Register a macro
static void register_macro_hook(parser_state_t* p) {
    char* payload = malloc(strlen(p->new_word_buffer)+1);
    strcpy(payload, p->new_word_buffer);
    debug_msg("macroing '%s' as '%s'\n", payload, p->custom_word_name);
    amf_register_compile_time_word(p, p->custom_word_name, macro, payload);
}

// Register a string-macro
static void string_macro_hook(parser_state_t* p) {
    strcpy(p->custom_word_name, p->buffer);
    size_t macro_size = amf_pop_data(p->fs);
    const char* macro_content = (const char*) amf_pop_data(p->fs);
    memcpy(p->new_word_buffer, macro_content, macro_size);
    p->new_word_buffer[macro_size] = 0;
    register_macro_hook(p);
    POP_HOOK(p, new_word_hook);
    p->pnt = 0;
}

/* --------------------------- Compile time words --------------------------- */

// (
static void open_par(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    if (!p->is_between_quotes) {
        p->is_in_parenthesis = true;
        p->pnt--;
    }
}

#define NOT_IN_DEF(p, name)                                            \
    if (p->in_def) {                                                   \
        error_msg("Using %s in a definition is not allowed.\n", name); \
        return;                                                        \
    }                                                                   
        

// :
static void colon(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    NOT_IN_DEF(p, ":");
    p->in_def = true;
    p->pnt = 0;
    p->new_word_hook = definition_name_hook;
    PUSH_HOOK(p, end_block_hook);
    p->end_block_hook = register_def_hook;
}

// :macro
static void colon_macro(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    NOT_IN_DEF(p, ":macro");
    p->in_def = true;
    p->pnt = 0;
    p->new_word_hook = definition_name_hook;
    PUSH_HOOK(p, end_block_hook);
    p->end_block_hook = register_macro_hook;
}

// ;
static void semi_colon(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    if (p->in_def) {
        p->in_def = false;
        p->buffer[p->pnt] = 0;
        p->pnt = 0;
        p->end_block_hook(p);
        POP_HOOK(p, end_block_hook);
        p->new_word_hook = run_next_word_hook;
    } else {
        error_msg("Using ; outside of a definition is not allowed.\n");
    }
}

// constant
static void _constant(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    NOT_IN_DEF(p, "constant");
    p->in_defining_constant = true;
    p->new_word_hook = var_const_hook;
    p->pnt = 0;
}

// variable
static void _variable(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    NOT_IN_DEF(p, "variable");
    p->in_defining_constant = false;
    p->new_word_hook = var_const_hook;
    p->pnt = 0;
}

// '
static_assert(sizeof(hash_t) <= sizeof(amf_int_t), "To handle execution tokens, hashes should fit in a cell.");
static void single_quote(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    PUSH_HOOK(p, new_word_hook);
    p->new_word_hook = get_exec_token_hook;
    p->pnt = 0;
}

// ." s"
static void any_string(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    p->is_between_quotes = true;
    PUSH_HOOK(p, end_block_hook);
    p->end_block_hook = register_string_hook;
}

/* \ */
static void backslash(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    p->wait_for_new_line = true;
    p->pnt = 0;
}

// macro-string
static void macro_string(parser_state_t* p, const char* payload) {
    UNUSED(payload);
    NOT_IN_DEF(p, "macro-string");
    p->in_defining_constant = false;
    p->pnt = 0;
    PUSH_HOOK(p, new_word_hook);
    p->new_word_hook = string_macro_hook;
}

// Generic word used by macros
static void macro(parser_state_t* p, const char* payload) {
    p->pnt = 0;
    for (size_t i=0; i<strlen(payload); i++) {
        amf_parse_char(p, payload[i]);
    }
}

// Register a compile time word
void amf_register_compile_time_word(parser_state_t* p, const char* name, compile_callback_t compile_func, char* payload) {
    entry_t e;
    e.type = compile_word;
    e.hash = amf_hash(name);
    e.func.compile_func.func = compile_func;
    e.func.compile_func.payload = payload;
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
    {":macro", colon_macro},
    {";", semi_colon},
    {"constant", _constant},
    {"variable", _variable},
    {"'", single_quote},
    {"s\"", any_string},
    {".\"", any_string},
    {"\\", backslash},
    {"macro-string", macro_string},
};

// Register the previously defined words
static void register_compile_time_words_list(parser_state_t* p) {
    for (size_t i = 0; i < sizeof(all_default_compile_words) / sizeof(struct compile_func_s); i++) {
        const char* name = all_default_compile_words[i].name;
        amf_register_compile_time_word(p, name, all_default_compile_words[i].func, NULL);
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

