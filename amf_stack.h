#include "ASCminiForth.h"
#ifndef AMF_STACK_H
#define AMF_STACK_H

#include <stdbool.h>

typedef struct {
    amf_int_t* stack;
    size_t stack_pointer;
#if AMF_STACK_BOUND_CHECKS
    size_t size;
    bool overflowed;
#endif
} amf_stack_t;

amf_stack_t* amf_stack_init(size_t size);
void amf_stack_free(amf_stack_t* stack);
amf_int_t amf_stack_peek(amf_stack_t* stack, int depth);
amf_int_t amf_stack_pop(amf_stack_t* stack);
void amf_stack_push(amf_stack_t* stack, amf_int_t w);
bool amf_state_state_valid(amf_stack_t* stack);

#endif

