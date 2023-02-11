#ifndef AMF_CONFIG_H
#define AMF_CONFIG_H

#include "inttypes.h"
typedef intptr_t amf_int_t;
#define AMF_INT_PRINT PRIdPTR

// Optional features and words. Set to 1 to enable and to 0 to disable

// Enable reading source files
#define AMF_USE_SOURCE_FILE 1

// Enable the words argc, argv, (arg), and arg to use when giving command
// line arguments to the interpreter
#define AMF_CLI_ARGS 1

// Enable words related to file manipulation
#define AMF_FILE 1

// Enable words related to string manipulation
#define AMF_STRING 1

// Enable programing tools words
#define AMF_PROGRAMMING_TOOLS 1

// Depth of the memory blocks
#define CODE_STACK_SIZE 100
#define DATA_STACK_SIZE 100
#define LOOP_STACK_SIZE 100
#define FORTH_MEMORY_SIZE 100

// Dummy values to put in the state's code position to indicate that
// the interpreter is idling and not inside a function
#define IDLE_CURRENT_WORD (hash_t) ~0
#define IDLE_POS_IN_WORD  (size_t) ~0

// Parser configuration
#define PARSER_BUFFER_SIZE 700
#define PARSER_CUSTOM_NAME_SIZE 25

// Case-sensitivity
#define AMF_CASE_INSENSITIVE 1

// Log messages
// 0 = no logs; 1 = errors; 2 = errors and warnings; 3 = errors, warning, and debug
#define AMF_LOG 2

// Writing logs to stderr instead of the usual canals
#define AMF_LOG_OVER_STDERR 0

// Use words defined in Forth
#define AMF_REGISTER_FORTH_FUNC 1

// Store name alongside word
#define AMF_STORE_NAME 1

// Max number of digits in a number
#define AMF_MAX_NUMBER_DIGIT 64

// Performs bound checks on stack to prevent overflows or underflows
#define AMF_STACK_BOUND_CHECKS 1

// Handling segfaults when running Forth code
// With this option set to 1, segfaults caused by Forth code will be caught and
// the interpreter will be put back into an idle state if encountered. This
// relies on static variable and thus, this prevent the interpreter to be used
// on multiple threads.
#define AMF_CATCH_SEGFAULTS 0

#endif

