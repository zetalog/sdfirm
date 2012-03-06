#include <target/generic.h>

int text_strlen(const text_char_t *str)
{
	int len = 0;
	while (*str++)
		len++;
	return len;
}

void memory_set(caddr_t s, uint8_t c, size_t count)
{
	uint8_t *xs = (uint8_t *)s;
	while (count--)
		*xs++ = c;
}

void memory_copy(caddr_t dest, const caddr_t src,
		 size_t count)
{
	uint8_t *tmp = (uint8_t *)dest, *s = (uint8_t *)src;
	if (dest == src) return;
	while (count--)
		*tmp++ = *s++;
}
