#include <target/video.h>

#ifdef CONFIG_FONT_8x8
extern video_font_t font_vga_8x8;
video_font_t *font = &font_vga_8x8;
#endif
#ifdef CONFIG_FONT_8x16
extern video_font_t font_vga_8x16;
video_font_t *font = &font_vga_8x16;
#endif
#ifdef CONFIG_FONT_6x11
extern video_font_t font_vga_6x11;
video_font_t *font = &font_vga_6x11;
#endif
#ifdef CONFIG_FONT_7x14
extern video_font_t font_7x14;
video_font_t *font = &font_7x14;
#endif
#ifdef CONFIG_FONT_SUN8x16
extern video_font_t font_sun_8x16;
video_font_t *font = &font_sun_8x16;
#endif
#ifdef CONFIG_FONT_SUN12x22
extern video_font_t font_sun_12x22;
video_font_t *font = &font_sun_12x22;
#endif
#ifdef CONFIG_FONT_10x18
extern video_font_t font_10x18;
video_font_t *font = &font_10x18;
#endif
#ifdef CONFIG_FONT_ACORN_8x8
extern video_font_t font_acorn_8x8;
video_font_t *font = &font_acorn_8x8;
#endif
#ifdef CONFIG_FONT_PEARL_8x8
extern video_font_t font_pearl_8x8;
video_font_t *font = &font_pearl_8x8;
#endif
#ifdef CONFIG_FONT_MINI_4x6
extern video_font_t font_mini_4x6;
video_font_t *font = &font_mini_4x6;
#endif
