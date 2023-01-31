#ifndef AMF_CONFIG_H
#define AMF_CONFIG_H

#include "inttypes.h"
typedef intptr_t amf_int_t;
#define AMF_INT_PRINT PRIdPTR

// Optional features and words. Set to 1 to enable and to 0 to disable
// Enable reading source files
#define AMF_USE_SOURCE_FILE 1

// Depth of the three stacks
#define CODE_STACK_SIZE 100
#define DATA_STACK_SIZE 100
#define LOOP_STACK_SIZE 100

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
// 0 = no logs; 1 = erros; 2 = errors and warnings; 3 = errors, warning, and debug
#define AMF_LOG 2

// Use words defined in Forth
#define AMF_REGISTER_FORTH_FUNC 1

// Store name alongside word
#define AMF_STORE_NAME 1

// Max number of digits in a number
#define AMF_MAX_NUMBER_DIGIT 64

#endif

