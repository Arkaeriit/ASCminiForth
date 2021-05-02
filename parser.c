#include "parser.h"
#include "string.h"

parser_state_t* amf_init_parser(void){
    parser_state_t* ret = malloc(sizeof(parser_state_t));
    ret->fs = amf_init_state();
    ret->buffer = malloc(PARSER_BUFFER_SIZE);
    ret->custom_word_name = malloc(PARSER_CUSTOM_NAME_SIZE);
    ret->pnt = 0;
    ret->in_word = false;
    ret->in_def = false;
    return ret;
}

void amf_clean_parser(parser_state_t* parse){
    free(parse->custom_word_name);
    free(parse->buffer);
    amf_clean_state(parse->fs);
    free(parse);
}

void amf_parse_char(parser_state_t* parse, char ch){
    if(ch == ':'){
        if(!parse->in_def){
            parse->in_def = true;
            parse->writing_definition_s_name = true;
        }else{
            //error
        }
    }else if(ch == ';'){
        if(parse->in_def){
            parse->in_def = false;
            parse->buffer[parse->pnt] = 0;
            parse->in_word = false;
            parse->pnt = 0;
            amf_compile_string(parse->fs->dic, parse->custom_word_name, parse->buffer);
        }else{
            //error
        }
    }else if(amf_is_delimiter(ch)){
        if(parse->in_word){
            if(parse->writing_definition_s_name){
                parse->buffer[parse->pnt] = 0;
                parse->in_word = false;
                parse->pnt = 0;
                parse->writing_definition_s_name = false;
                strcpy(parse->custom_word_name, parse->buffer);
            }else if(parse->in_def){
                parse->buffer[parse->pnt] = ch;
                parse->pnt++;
            }else{
                parse->buffer[parse->pnt] = 0;
                parse->in_word = false;
                parse->pnt = 0;
                word_node_t node_to_exe = amf_compile_node(parse->buffer);
                amf_executes_node(parse->fs, &node_to_exe);
                amf_run(parse->fs);
            }
        }else if(parse->in_def && !parse->writing_definition_s_name){
            parse->buffer[parse->pnt] = ch;
            parse->pnt++;
        }
    }else{
        parse->in_word = true;
        parse->buffer[parse->pnt] = ch;
        parse->pnt++;
    }        
}

void amf_shell(void){
    parser_state_t* parse = amf_init_parser();
    while(parse->fs->running){
        amf_parse_char(parse, getchar());
    }
    amf_clean_parser(parse);
}

