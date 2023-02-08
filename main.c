#include "ASCminiForth.h"
#include "stdio.h"

int main(int argc, char** argv) {
#if AMF_USE_SOURCE_FILE
    if (argc > 1) {
        parser_state_t* parse = amf_init_parser();
#if AMF_CLI_ARGS
        parse->fs->argc = argc - 2;
        parse->fs->argv = argv + 2;
#endif
        if (amf_register_file(parse, argv[1])) {
            fprintf(stderr, "Error, unable to read file %s.\n", argv[1]);
            return 1;
        }
        amf_run(parse->fs);
        amf_clean_parser(parse);
        return 0;
    }
#endif
    amf_shell();
    return 0;
}

