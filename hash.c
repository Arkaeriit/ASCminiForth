/* Simple public domain implementation of the standard CRC32 checksum.
 * Found on Björn Samuelsson's blog, http://home.thep.lu.se/~bjorn/crc/.
 */

#include "hash.h"
#include "string.h"

static uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

static void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}

//Returns the CRC32 of a C-string
hash_t forth_hash(const char* data){
    uint32_t ret;
    crc32(data, strlen(data), &ret);
    return ret;
}

