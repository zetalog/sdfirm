#ifndef __IFD_SERIAL_H_INCLUDE__
#define __IFD_SERIAL_H_INCLUDE__

#ifndef ARCH_HAVE_IFD
#define ARCH_HAVE_IFD			1
#else
#error "Multiple interface devices defined"
#endif

#ifdef CONFIG_ARCH_HAS_IFDS
#include <asm/mach/ifds.h>
#endif

#ifndef ARCH_HAVE_IFDS
#define ifds_hw_ctrl_init()
#define ifds_hw_ctrl_start()
#define ifds_hw_ctrl_stop()
#define ifds_hw_read_empty()			true
#define ifds_hw_parity_error()			false
#define ifds_hw_read_byte()			0
#define ifds_hw_write_byte(byte)
#define ifds_hw_config_baudrate(br)
#define ifds_hw_write_pin(port, pin, val)
#define ifds_hw_read_pin(port, pin)
#endif

#ifndef IFD_HW_FREQ_MAX
#define IFD_HW_FREQ_MAX			4000
#endif
#ifndef IFD_HW_DATA_MAX
#define IFD_HW_DATA_MAX			115200
#endif

void ifd_hw_deactivate(void);
boolean ifd_hw_activate(void);
void ifd_hw_reset(void);
void ifd_hw_warm_reset(void);

#ifdef CONFIG_IFD_CLOCK_CONTROL
void ifd_hw_suspend_clock(uint8_t level);
void ifd_hw_resume_clock(void);
#else
#define ifd_hw_suspend_clock(level)
#define ifd_hw_resume_clock()
#endif

void ifd_hw_config_cls(uint8_t cls);
void ifd_hw_config_etu(uint16_t F, uint8_t D);
#define ifd_hw_config_conv(conv)
#define ifd_hw_config_crep(on)
void ifd_hw_delay_gt(uint32_t gt, boolean blk);
void ifd_hw_delay_wt(uint32_t wt, boolean blk);

void ifd_hw_start_wtc(void);
void ifd_hw_stop_wtc(void);

uint8_t ifd_hw_read_byte(void);
void ifd_hw_write_byte(uint8_t byte);

void ifd_hw_slot_reset(void);
#define ifd_hw_slot_select(sid)

boolean ifd_hw_get_contact(uint8_t pin);
boolean ifd_hw_icc_present(void);

#define ifd_hw_cf_nr_freq()		1
#define ifd_hw_cf_get_freq(index)	IFD_HW_FREQ_DEF
#define ifd_hw_set_freq(freq)

void ifd_hw_ctrl_init(void);

#endif /* __IFD_TDA8034HN_H_INCLUDE__ */
