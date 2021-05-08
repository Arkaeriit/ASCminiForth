#include "ASCminiForth.h"
#include "stdio.h"

char* test_def[] = {"1", "1", "1", "+", "+", "."};
char* test2_def[] = {"test", "test", "1", ".", "exit", "test"};

int main(void){
	forth_state_t* fs = amf_init_state();
	
    amf_compile_user_word(fs->dic, "test", 6, test_def);
	amf_compile_user_word(fs->dic, "test2", 6, test2_def);
	amf_call_name(fs, "test");
	for(int i=0; i<10; i++){
		amf_run_step(fs);
	}
	printf("finished calling test\n");
	amf_call_name(fs, "test2");
    amf_run(fs);
	printf("finished calling test2\n");

    amf_compile_string(fs->dic, "test3", "test test 1 1 1 1 + + + .");
    amf_call_name(fs, "test3");
    amf_run(fs);
	printf("finished calling test3\n");

    amf_compile_string(fs->dic, "test4", "1 if 3 . else 11 . then");
    amf_compile_string(fs->dic, "test5", "0 if 6 . else 12 . then");
    amf_call_name(fs, "test4");
    amf_run(fs);
    amf_call_name(fs, "test5");
    amf_run(fs);
	amf_clean_state(fs);

    amf_shell();
	return 0;
}

