#ifndef __STRING_H_INCLUDE__
#define __STRING_H_INCLUDE__

#include <target/types.h>

int text_strlen(const text_char_t *str);
void memory_set(caddr_t s, uint8_t c, size_t count);
void memory_copy(caddr_t dest, const caddr_t src, size_t count);

#endif	/* __STRING_H_INCLUDE__ */
