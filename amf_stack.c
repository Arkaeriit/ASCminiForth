#include "amf_stack.h"
#include <stdint.h>

amf_stack_t* amf_stack_init(ssize_t size) {
    amf_stack_t* ret = malloc(sizeof(amf_stack_t));
    ret->stack = malloc(sizeof(amf_int_t) * size);
    ret->stack_pointer = 0;
#ifdef AMF_STACK_BOUND_CHECKS
    ret->size = size;
#endif
    return ret;
}

void amf_stack_free(amf_stack_t* stack) {
    free(stack->stack);
    free(stack);
}

amf_int_t amf_stack_peek(amf_stack_t* stack, int depth) {
    amf_int_t ret = stack->stack[stack->stack_pointer - 1 - (depth)];
    return ret;
}

amf_int_t amf_stack_pop(amf_stack_t* stack) {
    amf_int_t ret = stack->stack[stack->stack_pointer - 1];
    stack->stack_pointer--;
    return ret;
}

void amf_stack_push(amf_stack_t* stack, amf_int_t w) {
    stack->stack[stack->stack_pointer] = w;
    stack->stack_pointer++;
}

bool amf_state_state_valid(amf_stack_t* stack) {
#if AMF_STACK_BOUND_CHECKS
    return (stack->stack_pointer >= 0) && (stack->stack_pointer < stack->size);
#else
    return true;
#endif
}

