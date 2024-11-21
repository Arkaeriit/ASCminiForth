#include "private_api.h"
#ifndef C_FUNC_H
#define C_FUNC_H

typedef void (*C_callback_t)(forth_state_t*);

void sef_register_cfunc(forth_state_t* fs, const char* name, C_callback_t func);
void sef_register_default_C_func(forth_state_t* fs);

#endif

