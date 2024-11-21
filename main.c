#include "private_api.h"
#include "stdio.h"

int main(int argc, char** argv) {
    int rc = 0;
#if SEF_USE_SOURCE_FILE
    if (argc > 1) {
        parser_state_t* parse = sef_init_parser();
#if SEF_CLI_ARGS
        parse->fs->argc = argc - 1;
        parse->fs->argv = argv + 1;
#endif
        if (sef_register_file(parse, argv[1])) {
            fprintf(stderr, "Error, unable to read file %s.\n", argv[1]);
            return 1;
        }
#if SEF_PROGRAMMING_TOOLS
        rc = parse->fs->exit_code;
#endif
        sef_clean_parser(parse);
        return rc;
    }
#endif
    rc = sef_shell();
    return rc;
}

