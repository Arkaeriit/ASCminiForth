#ifndef SEF_CONFIG_H
#define SEF_CONFIG_H

#include "inttypes.h"
#include "assert.h"

// Typedef of the number used everywhere
typedef intptr_t sef_int_t;
typedef uintptr_t sef_unsigned_t;

// Size used to encode the word identifier and the index of each subword in a word
#define SEF_HASH_SIZE_BITS         24
#define SEF_WORD_CONTENT_SIZE_BITS 24

// Optional features and words. Set to 1 to enable and to 0 to disable

// Enable reading source files
#define SEF_USE_SOURCE_FILE 1

// Enable the words argc, argv, (arg), and arg to use when giving command
// line arguments to the interpreter
#define SEF_CLI_ARGS 1

// Enable words related to file manipulation
#define SEF_FILE 1

// Enable words related to string manipulation
#define SEF_STRING 1

// Enable programing tools words
#define SEF_PROGRAMMING_TOOLS 1

// Depth of the memory blocks
#define SEF_CODE_STACK_SIZE 1000
#define SEF_DATA_STACK_SIZE 1000
#define SEF_COMPILATION_STACK_SIZE 100
#define FORTH_MEMORY_SIZE 1000000
#define SEF_PAD_SIZE 100

// Parser configuration
#define SEF_PARSER_BUFFER_SIZE 1000
#define SEF_PARSER_CUSTOM_NAME_SIZE 50

// Case-sensitivity
#define SEF_CASE_INSENSITIVE 1

// Log messages
// 0 = no logs; 1 = errors; 2 = errors and warnings; 3 = errors, warning, and debug
#define SEF_LOG 2

// Writing logs to stderr instead of the usual canals
#define SEF_LOG_OVER_STDERR 0

// Use words defined in Forth
#define SEF_REGISTER_FORTH_FUNC 1

// Store name alongside word
#define SEF_STORE_NAME 1

// Max number of digits in a number
#define SEF_MAX_NUMBER_DIGIT 64

// Performs bound checks on stack to prevent overflows or underflows
#define SEF_STACK_BOUND_CHECKS 1

// Handling segfaults when running Forth code
// With this option set to 1, segfaults caused by Forth code will be caught and
// the interpreter will be put back into an idle state if encountered. This
// relies on static variable and thus, this prevent the interpreter to be used
// on multiple threads.
#define SEF_CATCH_SEGFAULTS 1

// Print a stack strace when an error is encountered during execution.
#define SEF_STACK_TRACE 1

#endif

