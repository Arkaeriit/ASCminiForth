#include "private_api.h"
#ifndef SEF_STACK_H
#define SEF_STACK_H

#include <stdbool.h>

typedef struct {
    sef_int_t* stack;
    size_t stack_pointer;
#if SEF_STACK_BOUND_CHECKS
    size_t size;
    bool overflowed;
#endif
} sef_stack_t;

sef_stack_t* sef_stack_init(size_t size);
void sef_stack_reset(sef_stack_t* stack);
void sef_stack_free(sef_stack_t* stack);
sef_int_t sef_stack_peek(sef_stack_t* stack, int depth);
sef_int_t sef_stack_pop(sef_stack_t* stack);
void sef_stack_push(sef_stack_t* stack, sef_int_t w);
bool sef_state_state_valid(sef_stack_t* stack);

#endif

