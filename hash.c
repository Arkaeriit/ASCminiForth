/* Implementation of CRC32 found on https://rosettacode.org/wiki/CRC-32#C
 */

#include "hash.h"
#include "string.h"

static uint32_t rc_crc32(uint32_t crc, const char *buf, size_t len)
{
    static uint32_t table[256];
    static int have_table = 0;
    const char *p, *q;
 
    /* This check is not thread safe; there is no mutex. */
    if (have_table == 0) {
        /* Calculate CRC table. */
        for (int i = 0; i < 256; i++) {
            uint32_t rem = i;  /* remainder from polynomial division */
            for (int j = 0; j < 8; j++) {
                if (rem & 1) {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                } else
                    rem >>= 1;
            }
            table[i] = rem;
        }
        have_table = 1;
    }
 
    crc = ~crc;
    q = buf + len;
    for (p = buf; p < q; p++) {
        uint8_t octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}
 

//Returns the CRC32 of a C-string
#include "inttypes.h"
hash_t amf_hash(const char* data){
#if AMF_CASE_INSENSITIVE
    char cpy[strlen(data) + 1];
    for(size_t i=0; i<strlen(data); i++){
        if('a' <= data[i] && data[i] <= 'z'){
            cpy[i] = data[i] - ('a' - 'A');
        }else{
            cpy[i] = data[i];
        }
    }
    uint32_t ret = rc_crc32(0, cpy, strlen(data));
#else
    uint32_t ret = rc_crc32(0, data, strlen(data));
#endif
    /*debug_msg("Hash of %s (size=%zi) is %" PRIu32 ".\n",data, strlen(data), ret);*/
    return ret;
}

