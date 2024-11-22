#include "SEForth.h"
#include "stdio.h"

int main(int argc, char** argv) {
    int rc = 0;
#if SEF_USE_SOURCE_FILE
    if (argc > 1) {
        forth_state_t* fs = sef_init();
#if SEF_CLI_ARGS
        sef_feed_arguments(fs, argc - 1, argv + 1);
#endif
        if (sef_parse_file(fs, argv[1])) {
            fprintf(stderr, "Error, unable to read file %s.\n", argv[1]);
            return 1;
        }
#if SEF_PROGRAMMING_TOOLS
        rc = sef_exit_code(fs);
#endif
        sef_free(fs);
        return rc;
    }
#endif
    rc = sef_shell();
    return rc;
}

