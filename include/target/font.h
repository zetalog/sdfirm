#ifndef __FONT_H_INCLUDE__
#define __FONT_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

struct font_desc {
    uint8_t width;
    uint8_t height;
    text_byte_t *data;
};
__TEXT_TYPE__(const struct font_desc, font_desc_t);

#endif /* __FONT_H_INCLUDE__ */
