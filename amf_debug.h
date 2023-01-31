#ifndef AMF_DEBUG_H
#define AMF_DEBUG_H

#include "amf_config.h"

#include "stdio.h"
#include "stdarg.h"
static void __attribute__((unused)) amf_log_error_msg(int ANSI_color, const char* tag, const char* msg, ...) {
    fprintf(stderr, "\033[%im%s ", ANSI_color, tag);
    va_list arg;
    va_start(arg, msg);
    vfprintf(stderr, msg, arg);
    va_end(arg);
    fprintf(stderr, "\033[0m");
}

#if AMF_LOG > 0
#define error_msg(msg, ...) amf_log_error_msg(31, "[ERROR]", msg, ##__VA_ARGS__);
#else
#define error_msg(msg, ...)
#endif

#if AMF_LOG > 1
#define warn_msg(msg, ...) amf_log_error_msg(33, "[WARNING]", msg, ##__VA_ARGS__);
#else
#define warn_msg(msg, ...)
#endif

#if AMF_LOG > 2
#define debug_msg(msg, ...) amf_log_error_msg(36, "[DEBUG]", msg, ##__VA_ARGS__);
#else
#define debug_msg(msg, ...)
#endif

#endif

