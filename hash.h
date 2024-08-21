/*------------------------------\
|This file is what defines the  |
|hash used to create dictionary |
{entries from the name of words.|
\------------------------------*/

#ifndef HASH_H
#define HASH_H

#include "stdint.h"
typedef uint32_t hash_t;
#define AMF_HASH_MASK     ((1 << (AMF_HASH_SIZE_BITS)) - 1)
#define AMF_RAW_HASH_MASK (AMF_HASH_MASK >> 1)
#define IDLE_CURRENT_WORD (hash_t) (~0 & AMF_HASH_MASK)


#include "ASCminiForth.h"

static_assert((sizeof(hash_t) * 8) >= AMF_HASH_SIZE_BITS, "hash_t should be able to contain enough bytes.");

hash_t amf_hash(const char* data);

#endif

