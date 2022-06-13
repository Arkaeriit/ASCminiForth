#include "amf_config.h"

#include <stdio.h>
// Format a number in any base. Assumes that the size given for the string is
// large enough. The string that should be used is made with the memory from
// `out`.
char* amf_base_format(amf_int_t i, char* out, int base) {
	char* p = out + AMF_MAX_NUMBER_DIGIT - 1;;
	*p = 0;
	p--;
	do {
		int mod = i % base;
		i -= mod;
		i /= base;
		char c;
		if (mod <= 9) {
			c = mod + '0';
		} else {
			c = mod + 'A';
		}
		*p = c;
		p--;
	} while(i != 0);
	return p + 1;
}

