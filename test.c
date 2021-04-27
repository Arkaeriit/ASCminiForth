#include "ASCminiForth.h"

int main(void){
	forth_state_t* fs = amf_init_state();
	amf_call_name(fs, "1");
	amf_call_name(fs, "1");
	amf_call_name(fs, "+");
	amf_call_name(fs, ".");
	printf("\n");
	amf_clean_state(fs);
	return 0;
}

