#include "sef_io.h"

// Implementation specifics functions
// Might need to be changed deppendin on where
// SEForth is ment to be embedded

// sef_input: this function returns a char entered by the input. The char should be echoed
// sef_output: print a char to the user

#include "stdio.h"
char __attribute__((weak)) sef_input(void) {
    int ret = getchar();
    if (ret != -1) {    // Check for unexpected char that could do bad things to  the rest of the parser
        return ret;
    } else {
        return 4; // Return end of transmission in ASCII
    }
}

void __attribute__((weak)) sef_output(char ch) {
    putchar(ch);
}

// Function that should not change depending on the implementation
#include "string.h"

// analogous to puts
void sef_print_string(const char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        sef_output(str[i]);
    }
}

