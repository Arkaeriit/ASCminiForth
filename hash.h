/*------------------------------\
|This file is what defines the  |
|hash used to create dictionary |
{entries from the name of words.|
\------------------------------*/

#ifndef HASH_H
#define HASH_H

#include "stdint.h"

typedef uint32_t hash_t;

hash_t amf_hash(const char* data);

#endif

