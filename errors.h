#ifndef ERROR_H
#define ERROR_H

#include "assert.h"

//This type defines the various errors we might encounter
typedef enum {
    OK = 0,
    no_memory = 1,
    not_found = 2,
    invalid_file = 3,
    impossible_error = 4,
    segfault = 5,
    amf_config_error = 6,
} error;

// Basic checks on the current configuration

#if AMF_CLI_ARGS
#if !AMF_USE_SOURCE_FILE
#error CLI arguments are only used when interpreting a file. It makes no sense to enable CLI arguments while not enabling source files.
#endif
#endif

static_assert(sizeof(amf_int_t) == sizeof(amf_unsigned_t), "Unsigned and signed values should have the same size.");
static_assert((sizeof(amf_int_t) * 8) >= (AMF_HASH_SIZE_BITS + AMF_WORD_CONTENT_SIZE_BITS), "You should be able to store word position and hash in a amf_int_t.");

#endif

