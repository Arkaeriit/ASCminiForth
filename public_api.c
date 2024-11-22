#include "private_api.h"

forth_state_t* sef_init(void) {
    return sef_init_parser()->fs;
}

void sef_free(forth_state_t* state) {
    sef_clean_parser(state->parser);
}

sef_error sef_parse_file(forth_state_t* state, const char* filename) {
    return sef_register_file(state->parser, filename);
}

#ifdef SEF_CLI_ARGS
void sef_feed_arguments(forth_state_t* state, int argc, char** argv) {
    state->argc = argc;
    state->argv = argv;
}
#endif

#ifdef SEF_PROGRAMMING_TOOLS
int sef_exit_code(const forth_state_t* state) {
    return state->exit_code;
}
#endif

