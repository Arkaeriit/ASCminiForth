#include "ASCminiForth.h"
#ifndef AMF_STACK_H
#define AMF_STACK_H

#include <stdbool.h>

typedef struct {
    amf_int_t* stack;
    ssize_t stack_pointer;
#if AMF_STACK_BOUND_CHECKS
    ssize_t size;
#endif
} amf_stack_t;

amf_stack_t* amf_stack_init(ssize_t size);
void amf_stack_free(amf_stack_t* stack);
amf_int_t amf_stack_peek(amf_stack_t* stack, int depth);
amf_int_t amf_stack_pop(amf_stack_t* stack);
void amf_stack_push(amf_stack_t* stack, amf_int_t w);
bool amf_state_state_valid(amf_stack_t* stack);

#endif

