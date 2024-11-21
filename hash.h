/*------------------------------\
|This file is what defines the  |
|hash used to create dictionary |
{entries from the name of words.|
\------------------------------*/

#ifndef HASH_H
#define HASH_H

#include "stdint.h"
typedef uint32_t hash_t;
#define SEF_HASH_MASK     ((1 << (SEF_HASH_SIZE_BITS)) - 1)
#define SEF_RAW_HASH_MASK (SEF_HASH_MASK >> 1)
#define SEF_HASH_PRINT PRIx32
#define SEF_IDLE_CURRENT_WORD (hash_t) (~0 & SEF_HASH_MASK)


#include "SEForth.h"

static_assert((sizeof(hash_t) * 8) >= SEF_HASH_SIZE_BITS, "hash_t should be able to contain enough bytes.");

hash_t sef_hash(const char* data);

#endif

