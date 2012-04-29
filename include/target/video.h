#ifndef __VIDEO_H_INCLUDE__
#define __VIDEO_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

typedef uint16_t video_pt_t;

typedef uint32_t video_rgb_t;
#define RGB(r,g,b)	((video_rgb_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint16_t)(uint8_t)(b))<<16)))
#define RGB_R(rgb)	((uint8_t)(rgb))
#define RGB_G(rgb)	((uint8_t)(((uint16_t)(rgb)) >> 8))
#define RGB_B(rgb)	((uint8_t)((rgb)>>16))

struct video_rbg_triple {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};

struct video_font {
    uint8_t width;
    uint8_t height;
    text_byte_t *data;
};
__TEXT_TYPE__(const struct video_font, video_font_t);

video_font_t *font;

#include <driver/video.h>

#define video_draw_pixel(x, y, rgb)	video_hw_draw_pixel(x, y, rgb)

#endif /* __VIDEO_H_INCLUDE__ */
