#ifndef __KBD_DRIVER_H_INCLUDE__
#define __KBD_DRIVER_H_INCLUDE__

#ifdef CONFIG_KBD_AT8XC5122
#include <asm/mach-at8xc5122/kbd.h>
#endif
#ifdef CONFIG_KBD_AT90SCR100
#include <asm/mach-at90scr100/kbd.h>
#endif
#ifdef CONFIG_KBD_LM3S9B92
#include <asm/mach-lm3s9b92/kbd.h>
#endif
#ifdef CONFIG_KBD_LM3S3826
#include <asm/mach-lm3s3826/kbd.h>
#endif

#ifndef ARCH_HAVE_KBD
#define KBD_HW_COL_KEYS			2
#define KBD_HW_ROW_KEYS			1
#define kbd_hw_ctrl_init()
#define kbd_hw_translate_key(scancode)	0
#define kbd_hw_scan_sync(sec)
#endif

#endif /* __KBD_DRIVER_H_INCLUDE__ */
