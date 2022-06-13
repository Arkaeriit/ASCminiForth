#include "parser.h"
#include "stdio.h"
#include "string.h"

parser_state_t *amf_init_parser(void) {
    parser_state_t *ret = malloc(sizeof(parser_state_t));
    ret->fs = amf_init_state();
    ret->buffer = malloc(PARSER_BUFFER_SIZE);
    ret->custom_word_name = malloc(PARSER_CUSTOM_NAME_SIZE);
    ret->pnt = 0;
    ret->in_word = false;
    ret->in_def = false;
    ret->writing_definition_s_name = false;
    ret->is_in_parenthesis = false;
    ret->is_between_quotes = false;
    ret->is_last_escaped = false;
    ret->wait_for_new_line = false;
    amf_init_io();
    return ret;
}

void amf_clean_parser(parser_state_t * parse) {
    amf_clean_io();
    free(parse->custom_word_name);
    free(parse->buffer);
    amf_clean_state(parse->fs);
    free(parse);
}

void amf_parse_char(parser_state_t * parse, char ch) {
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
    if (ch == '"' && !parse->is_last_escaped) {
        parse->is_between_quotes = !parse->is_between_quotes;
    }
    if (ch == '(') {
        parse->is_in_parenthesis = true;
    } else if (parse->is_in_parenthesis) {
        if (ch == ')') {
            parse->is_in_parenthesis = false;
        }
    } else if (ch == ':' && !parse->is_between_quotes) {
        if (!parse->in_def) {
            parse->in_def = true;
            parse->writing_definition_s_name = true;
        } else {
            //error
        }
    } else if (ch == ';' && !parse->is_between_quotes) {
        if (parse->in_def) {
            parse->in_def = false;
            parse->buffer[parse->pnt] = 0;
            parse->in_word = false;
            parse->pnt = 0;
            amf_compile_string(parse->fs->dic, parse->custom_word_name, parse->buffer);
        } else {
            //error
        }
    } else if (amf_is_delimiter(ch) && !parse->is_between_quotes) {
		if (parse->is_last_escaped) {
			parse->wait_for_new_line = true;
		}
        if (parse->in_word) {
            if (parse->writing_definition_s_name) {
                parse->buffer[parse->pnt] = 0;
                parse->in_word = false;
                parse->pnt = 0;
                parse->writing_definition_s_name = false;
                strcpy(parse->custom_word_name, parse->buffer);
            } else if (parse->in_def) {
                parse->buffer[parse->pnt] = ch;
                parse->pnt++;
            } else {
                parse->buffer[parse->pnt] = 0;
                parse->in_word = false;
                parse->pnt = 0;
                word_node_t node_to_exe = amf_compile_node(parse->buffer);
                amf_executes_node(parse->fs, &node_to_exe);
                amf_run(parse->fs);
            }
        } else if (parse->in_def && !parse->writing_definition_s_name) {
            parse->buffer[parse->pnt] = ch;
            parse->pnt++;
        }
    } else {
        parse->in_word = true;
        parse->buffer[parse->pnt] = ch;
        parse->pnt++;
    }
	parse->is_last_escaped = false;
}

void amf_shell(void) {
    amf_print_string("Starting the ASCminiForth shell.\n");
    parser_state_t *parse = amf_init_parser();
    while (parse->fs->running) {
        amf_parse_char(parse, amf_input());
    }
    amf_clean_parser(parse);
}

#if AMF_USE_SOURCE_FILE
error amf_register_file(parser_state_t * p, const char *filemane) {
    FILE *f = fopen(filemane, "r");
    if (f == NULL) {
        return invalid_file;
    }
    int ch = fgetc(f);
    while (ch != EOF) {
        amf_parse_char(p, ch);
        ch = fgetc(f);
    }
    fclose(f);
    return OK;
}
#endif
