#include "amf_shell.h"
#include "string.h"

void amf_shell(forth_state_t* fs){
    printf("Starting ASCminiFORTH shell...\n");
    char buffer[100];
    size_t pnt = 0;
    bool in_word = false;
    bool in_def = false;
    char* def_name = malloc(25);
    bool is_def_first_word = false;
    while(true){
        char ch = getchar();
        if(ch == ':'){
            if(!in_def){
                in_def = true;
                is_def_first_word = true;
            }else{
                //error
            }
        }else if(ch == ';'){
            if(in_def){
                error_msg(buffer);
                in_def = false;
                buffer[pnt] = 0;
                in_word = false;
                pnt = 0;
                amf_compile_string(fs->dic, def_name, buffer);
            }else{
                //error
            }
        }else if(is_delimiter(ch)){
            if(in_word){
                if(is_def_first_word){
                    buffer[pnt] = 0;
                    in_word = false;
                    pnt = 0;
                    is_def_first_word = false;
                    strcpy(def_name, buffer);
                }else if(in_def){
                    buffer[pnt] = ch;
                    pnt++;
                }else{
                    buffer[pnt] = 0;
                    in_word = false;
                    pnt = 0;
                    amf_call_name(fs, buffer);
                    amf_run(fs);
                }
            }else if(in_def && !is_def_first_word){
                buffer[pnt] = ch;
                pnt++;
            }
        }else{
            in_word = true;
            buffer[pnt] = ch;
            pnt++;
        }        
    }
}

