#include "ASCminiForth.h"
#include "stdio.h"

int main(int argc, char** argv){
#if AMF_USE_SOURCE_FILE
    if(argc > 1){
        parser_state_t* parse = amf_init_parser();
        for(int i=1; i<argc; i++){
            if(amf_register_file(parse, argv[i])){
                fprintf(stderr, "Error, unable to read file %s.\n",argv[i]);
                return 1;
            }    
        }
        amf_run(parse->fs);
        amf_clean_parser(parse);
        return 0;
    }
#endif
    amf_shell();
    return 0;
}

