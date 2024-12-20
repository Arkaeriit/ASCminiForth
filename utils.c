#include "private_api.h"
#include <stdbool.h>

// Format a number in any base. Assumes that the size given for the string is
// large enough. The string that should be used is made with the memory from
// `out`.
// TODO: replace it with the use of >number
char* sef_base_format(sef_int_t i, char* out, int base) {
    bool negative = i < 0;
    i = (negative ? -i : i);
    char* p = out + SEF_MAX_NUMBER_DIGIT - 1;;
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
            c = mod + 'A' - 10;
        }
        *p = c;
        p--;
    } while (i != 0);
    if (negative) {
        *p = '-';
        p--;
    }
    return p + 1;
}

