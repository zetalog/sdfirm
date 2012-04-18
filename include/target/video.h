#ifndef __VIDEO_H_INCLUDE__
#define __VIDEO_H_INCLUDE__

#include <target/config.h>

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

#endif /* __VIDEO_H_INCLUDE__ */
