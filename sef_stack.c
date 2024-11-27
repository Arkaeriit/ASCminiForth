#include "sef_stack.h"
#include <stdint.h>

sef_stack_t* sef_stack_init(size_t size) {
    sef_stack_t* ret = malloc(sizeof(sef_stack_t));
    ret->stack = malloc(sizeof(sef_int_t) * size);
#if SEF_STACK_BOUND_CHECKS
    ret->size = size;
#endif
    sef_stack_reset(ret);
    return ret;
}

void sef_stack_reset(sef_stack_t* stack) {
    stack->stack_pointer = 0;
#if SEF_STACK_BOUND_CHECKS
    stack->overflowed = false;
#endif
}

void sef_stack_free(sef_stack_t* stack) {
    free(stack->stack);
    free(stack);
}

sef_int_t sef_stack_peek(sef_stack_t* stack, int depth) {
    sef_int_t ret = stack->stack[stack->stack_pointer - 1 - (depth)];
    return ret;
}

sef_int_t sef_stack_pop(sef_stack_t* stack) {
#if SEF_STACK_BOUND_CHECKS
    if (stack->stack_pointer == 0) {
        stack->overflowed = 1;
        return 0;
    }
#endif
    sef_int_t ret = stack->stack[stack->stack_pointer - 1];
    stack->stack_pointer--;
    return ret;
}

void sef_stack_push(sef_stack_t* stack, sef_int_t w) {
#if SEF_STACK_BOUND_CHECKS
    if (stack->stack_pointer >= (stack->size - 1)) {
        stack->overflowed = 1;
        return;
    }
#endif
    stack->stack[stack->stack_pointer] = w;
    stack->stack_pointer++;
}

bool sef_state_state_valid(sef_stack_t* stack) {
#if SEF_STACK_BOUND_CHECKS
    return !stack->overflowed;
#else
    return true;
#endif
}

