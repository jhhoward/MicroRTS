#ifndef PGM_SPACE
#define PGM_SPACE

#include <string.h>
#include <stdint.h>
#define PROGMEM

#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))

#define pgm_read_ptr(x) (*((uintptr_t*)x))

#define PSTR(x) (x)

#define strlen_P(x) strlen(x)

#endif
