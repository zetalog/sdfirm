#ifndef __VIDEO_HX8347GT_H_INCLUDE__
#define __VIDEO_HX8347GT_H_INCLUDE__

#include <target/gpio.h>

#ifndef ARCH_HAVE_VIDEO
#define ARCH_HAVE_VIDEO			1
#else
#error "Multiple video controller defined"
#endif

#define NR_HW_VIDEO_VPIXELS	240
#define NR_HW_VIDEO_HPIXELS	320

void video_hw_ctrl_init(void);
void video_hw_draw_pixel(video_pt_t x, video_pt_t y, video_rgb_t rgb);

#endif
