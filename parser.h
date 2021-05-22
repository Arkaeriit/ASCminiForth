#include "ASCminiForth.h"
#ifndef AMF_SHELL_H
#define AMF_SHELL_H

typedef struct {
    forth_state_t* fs;
    char* buffer; //Store the characters being currentely processed
    char* custom_word_name; //Stores a copy of the name of the word being defined
    int pnt; //Pointer to the curent portiotion of buffer where we are writing
    bool in_word; //Are we writing a word or whitespace
    bool in_def;  //Are we writing a definition
    bool writing_definition_s_name; //Are we writing the first word of a definition (it's name)
    bool is_in_parenthesis; //are we in a ( xx ) comment
} parser_state_t;

parser_state_t* amf_init_parser(void);
void amf_clean_parser(parser_state_t* parse);
void amf_parse_char(parser_state_t* parse, char ch);
void amf_shell(void);

#endif

