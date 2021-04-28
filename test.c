#include "ASCminiForth.h"

const char* test_def[] = {"1", "1", "1", "+", "+", "."};

int main(void){
	forth_state_t* fs = amf_init_state();
	amf_call_name(fs, "1");
	amf_call_name(fs, "1");
	amf_call_name(fs, "+");
	amf_call_name(fs, ".");
	printf("\n");
	amf_compile_user_word(fs->dic, "test", 6, test_def);
	amf_call_name(fs, "test");
	for(int i=0; i<10; i++){
		amf_run_step(fs);
	}
	amf_clean_state(fs);
	return 0;
}
