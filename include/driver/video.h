#ifndef __VIDEO_DRIVER_H_INCLUDE__
#define __VIDEO_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_VIDEO_HX8347G
#include <driver/video_hx8347g.h>
#endif

#ifndef ARCH_HAVE_VIDEO
#define NR_VIDEO_HW_VPIXELS	1
#define NR_VIDEO_HW_HPIXELS	1
#define video_hw_drwa_pxiel(x, y, rgb)
#define video_hw_ctrl_init()
#endif

#endif /* __VIDEO_DRIVER_H_INCLUDE__ */
