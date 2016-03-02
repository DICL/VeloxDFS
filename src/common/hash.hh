#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

#define HASHLENGTH 64

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

extern "C" uint32_t h (const char *, size_t);
extern "C" uint32_t hash_ruby (const char * data);
uint32_t h (std::string);
