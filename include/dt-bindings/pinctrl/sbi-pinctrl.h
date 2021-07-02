/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smarco.cn>
 */

#ifndef __DT_BINDINGS_PINCTRL_SBI_H
#define __DT_BINDINGS_PINCTRL_SBI_H

/*=========================================================================
 * GPIO special drive
 *=======================================================================*/
#define GPIO_DRIVE_IN		0

/*=========================================================================
 * GPIO mux configuration
 *=======================================================================*/
#define GPIO_MUX_NONE		0

/*=========================================================================
 * GPIO pad configuration
 *=======================================================================*/
#define GPIO_PAD_PUSH_PULL	0x00
#define GPIO_PAD_OPEN_DRAIN	0x01
#define GPIO_PAD_PULL_UP	0x02
#define GPIO_PAD_PULL_DOWN	0x04
#define GPIO_PAD_ANALOG_IO	0x00
#define GPIO_PAD_DIGITAL_IO	0x08
#define GPIO_PAD_WEAK_PULL	0x00
#define GPIO_PAD_MEDIUM_PULL	0x10
#define GPIO_PAD_STRONG_PULL	0x20
#define GPIO_PAD_SLEW_RATE	0x40
#define GPIO_PAD_SCHMITT_TRIG	0x80
/* Re-use schmitt trigger unless being complained */
#define GPIO_PAD_KEEPER		0x80
#define GPIO_PAD_PULL_MASK	\
	(GPIO_PAD_PULL_UP | GPIO_PAD_PULL_DOWN)

#define GPIO_PAD_PP		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_PUSH_PULL)
#define GPIO_PAD_OD		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_OPEN_DRAIN)
#define GPIO_PAD_KB		(GPIO_PAD_PP | GPIO_PAD_SLEW_RATE)
#define GPIO_PAD_WU		(GPIO_PAD_PULL_UP | GPIO_PAD_WEAK_PULL)
#define GPIO_PAD_WD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_WEAK_PULL)
#define GPIO_PAD_MU		(GPIO_PAD_PULL_UP | GPIO_PAD_MEDIUM_PULL)
#define GPIO_PAD_MD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_MEDIUM_PULL)
#define GPIO_PAD_SU		(GPIO_PAD_PULL_UP | GPIO_PAD_STRONG_PULL)
#define GPIO_PAD_SD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_STRONG_PULL)

/* push pull with weak pull up */
#define GPIO_PAD_PP_WU		(GPIO_PAD_PP | GPIO_PAD_WU)
/* push pull with weak pull down */
#define GPIO_PAD_PP_WD		(GPIO_PAD_PP | GPIO_PAD_WD)
/* push pull with medium pull up */
#define GPIO_PAD_PP_MU		(GPIO_PAD_PP | GPIO_PAD_MU)
/* push pull with medium pull down */
#define GPIO_PAD_PP_MD		(GPIO_PAD_PP | GPIO_PAD_MD)
/* push pull with strong pull up */
#define GPIO_PAD_PP_SU		(GPIO_PAD_PP | GPIO_PAD_SU)
/* push pull with medium pull down */
#define GPIO_PAD_PP_SD		(GPIO_PAD_PP | GPIO_PAD_SD)
/* open drain with weak pull up */
#define GPIO_PAD_OD_WU		(GPIO_PAD_OD | GPIO_PAD_WU)
/* open drain with weak pull down */
#define GPIO_PAD_OD_WD		(GPIO_PAD_OD | GPIO_PAD_WD)
/* open drain with medium pull up */
#define GPIO_PAD_OD_MU		(GPIO_PAD_OD | GPIO_PAD_MU)
/* open drain with medium pull down */
#define GPIO_PAD_OD_MD		(GPIO_PAD_OD | GPIO_PAD_MD)
/* open drain with strong pull up */
#define GPIO_PAD_OD_SU		(GPIO_PAD_OD | GPIO_PAD_SU)
/* open drain with strong pull down */
#define GPIO_PAD_OD_SD		(GPIO_PAD_OD | GPIO_PAD_SD)

#define SBI_PIN(port, pin)			((port) << 16 | (pin))
#define SBI_PAD(pad, drive)			((pad) << 4 | (drive))
#define SBI_PINMUX(port, pin, func)		SBI_PIN(port, pin), func
#define SBI_PINPAD(port, pin, pad, drive)	\
	SBI_PIN(port, pin), SBI_PAD(pad, drive)

#define sbi_pin_port(pinid)		((unsigned short)((pinid) >> 16))
#define sbi_pin_pin(pinid)		((unsigned short)(pinid))
#define sbi_pad_pad(pad)		((pad) >> 4)
#define sbi_pad_drive(pad)		((unsigned char)((pad) & 0x0f))

#endif /* __DT_BINDINGS_PINCTRL_SBI_H */
