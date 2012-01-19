#ifndef __IFD_DRIVER_H_INCLUDE__
#define __IFD_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_IFD
#include <asm/mach/ifd.h>
#endif

#ifdef CONFIG_IFD_SERIAL
#include <driver/ifd_serial.h>
#endif

#ifndef ARCH_HAVE_IFD
#define IFD_HW_DATA_MAX		0
#define IFD_HW_FREQ_MAX		0

#define ifd_hw_deactivate()
#define ifd_hw_activate()	false
#define ifd_hw_reset()
#define ifd_hw_warm_reset()

/* clock */
#define ifd_hw_suspend_clock(level)
#define ifd_hw_resume_clock()

#define ifd_hw_config_cls(cls)
#define ifd_hw_config_etu(F, D)
#define ifd_hw_config_conv(conv)
#define ifd_hw_delay_gt(gt, blk)
#define ifd_hw_delay_wt(wt, blk)

#define ifd_hw_start_wtc()
#define ifd_hw_stop_wtc()
#define ifd_hw_read_byte()		0
#define ifd_hw_write_byte(b)

#define ifd_hw_slot_reset()
#define ifd_hw_slot_select(sid)

#define ifd_hw_cf_nr_freq()		1
#define ifd_hw_cf_get_freq(index)	IFD_HW_FREQ_DEF
#define ifd_hw_set_freq(freq)
#define ifd_hw_icc_present()		false

#define ifd_hw_config_crep(on)

#define ifd_hw_get_contact(pin)		false

#define ifd_hw_ctrl_init()
#endif

#endif /* __IFD_DRIVER_H_INCLUDE__ */
