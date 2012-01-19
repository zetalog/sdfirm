/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)usb_msd.h: mass storage device interfaces
 * $Id: usb_msd.h,v 1.73 2011-04-20 09:55:42 zhenglv Exp $
 */

#ifndef __USB_MSD_H_INCLUDE__
#define __USB_MSD_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/usb.h>
#include <target/scsi.h>

#ifdef CONFIG_MSD_DEBUG
#define msd_debug(tag, val)	dbg_print((tag), (val))
#define msd_debug_halt(halting)	\
	do {							\
		usb_debug(MSD_DEBUG_HALT, halting);		\
	} while (0)
#define MSD_DUMP_USBD		true
#else
#define msd_debug(tag, val)
#define msd_debug_halt(halting)
#define MSD_DUMP_USBD		false
#endif

/*
 * USB MSD (Mass Storage Device) interface class code
 */
#define USB_INTERFACE_CLASS_MSD		8

/* USB MSD interface subclasses */
#define USB_INTERFACE_SUBCLASS_NONE	0x00
#define USB_INTERFACE_SUBCLASS_RBC	0x01		/* flash drives */
#define USB_INTERFACE_SUBCLASS_MMC5	0x02		/* CD-ROM */
#define USB_INTERFACE_SUBCLASS_UFI	0x04		/* floppy */
#define USB_INTERFACE_SUBCLASS_SCSI	0x06		/* transparent SCSI */
#define USB_INTERFACE_SUBCLASS_LSDFS	0x07		/* LSD FS */
#define USB_INTERFACE_SUBCLASS_IEEE1667	0x08		/* IEEE 1667 */
#define USB_INTERFACE_SUBCLASS_DEVICE	0xFF

/* USB MSD interface protocols */
#define USB_INTERFACE_PROTOCOL_CBI	0x00		/* control/bulk/interrupt */
#define USB_INTERFACE_PROTOCOL_CB	0x01		/* control/bulk w/o interrupt */
#define USB_INTERFACE_PROTOCOL_BBB	0x50		/* bulk only */
#define USB_INTERFACE_PROTOCOL_UAS	0x62		/* USB attached SCSI */
#define USB_INTERFACE_PROTOCOL_DEVICE	0xFF

typedef void (*msd_cmpl_cb)(boolean result);

/* subclass drivers */
#ifdef CONFIG_MSD_SCSI
#include <target/msd_scsi.h>
#endif

/* protocol drivers */
#ifdef CONFIG_MSD_BBB
#include <target/msd_bbb.h>
#endif

#define MSD_ENDP_INTERVAL		10

#ifndef MSD_INTERFACE_SUBCLASS
#error "MSD subclass is not defined"
#endif
#ifndef MSD_INTERFACE_PROTOCOL
#error "MSD protocol is not defined"
#endif

void msd_proto_ctrl(void);
void msd_proto_init(void);
void msd_proto_start(void);
void msd_proto_reset(void);
uint32_t msd_proto_get_expect(void);
uint8_t msd_proto_get_lun(void);
uint8_t msd_proto_get_dir(void);
#define msd_proto_bulk_open	usbd_bulk_open
#define msd_proto_bulk_close	usbd_bulk_close
#define msd_proto_bulk_put	usbd_bulk_put
#define msd_proto_bulk_get	usbd_bulk_get
#define msd_proto_bulk_write	usbd_bulk_in
#define msd_proto_bulk_read	usbd_bulk_out
uint8_t msd_proto_bulk_type(void);
bulk_size_t msd_proto_bulk_size(uint8_t type);
boolean msd_proto_bulk_idle(void);

void msd_class_init(void);
void msd_class_start(void);
void msd_class_reset(void);
void msd_class_io(void);
boolean msd_class_cb_valid(uint8_t *cb, uint8_t len);
boolean msd_class_cb_schedule(msd_cmpl_cb call);
void msd_class_cb_complete(void);
uint8_t msd_class_max_luns(void);
uint32_t msd_class_get_expect(void);
boolean msd_class_get_status(void);

extern uint8_t msd_addr[NR_MSD_ENDPS];

#endif /* __USB_MSD_H_INCLUDE__ */
