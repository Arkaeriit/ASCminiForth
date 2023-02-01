#ifndef ERROR_H
#define ERROR_H

//This type defines the various errors we might encounter
typedef enum {
    OK = 0,
    no_memory = 1,
    not_found = 2,
    invalid_file = 3,
    impossible_error = 4,
    segfault = 5,
} error;

#endif

