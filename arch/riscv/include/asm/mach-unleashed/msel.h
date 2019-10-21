/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
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
 * @(#)msel.h: FU540 (unleashed) mode select defintions
 * $Id: msel.h,v 1.1 2019-10-21 17:30:00 zhenglv Exp $
 */

#ifndef __MSEL_UNLEASHED_H_INCLUDE__
#define __MSEL_UNLEASHED_H_INCLUDE__

/* Modes 0-4 are handled in the MSEL Gate ROM         0123 */
#define MSEL_LOOP				0  /* 0000 ---- */
#define MSEL_SPI0_FLASH_XIP			1  /* 0001 _--- */
#define MSEL_SPI1_FLASH_XIP			2  /* 0010 -_-- */
#define MSEL_CHIPLINK_TL_UH_XIP			3  /* 0011 __-- */
#define MSEL_CHIPLINK_TL_C_XIP			4  /* 0100 --_- */
#define MSEL_ZSBL_SPI0_MMAP			5  /* 0101 _-_- */
#define MSEL_FSBL_SPI0_MMAP			5  /* 0101 _-_- */
#define MSEL_ZSBL_SPI0_MMAP_QUAD		6  /* 0110 -__- */
#define MSEL_FSBL_SPI0_MMAP_QUAD		6  /* 0110 -__- */
#define MSEL_ZSBL_SPI1_MMAP_QUAD		7  /* 0111 ___- */
#define MSEL_FSBL_SPI1_MMAP_QUAD		7  /* 0111 ___- */
#define MSEL_ZSBL_SPI1_SDCARD			8  /* 1000 ---_ */
#define MSEL_FSBL_SPI1_SDCARD			8  /* 1000 ---_ */
#define MSEL_ZSBL_SPI2_FLASH			9  /* 1001 _--_ */
#define MSEL_FSBL_SPI2_FLASH			9  /* 1001 _--_ */
#define MSEL_ZSBL_SPI0_MMAP_QUAD		10 /* 1010 -_-_ */
#define MSEL_FSBL_SPI1_SDCARD			10 /* 1010 -_-_ */
#define MSEL_ZSBL_SPI2_SDCARD			11 /* 1011 __-_ */
#define MSEL_FSBL_SPI2_SDCARD			11 /* 1011 __-_ */
#define MSEL_ZSBL_SPI1_FLASH			12 /* 1100 --__ */
#define MSEL_FSBL_SPI2_SDCARD			12 /* 1100 --__ */
#define MSEL_ZSBL_SPI1_MMAP_QUAD		13 /* 1101 -_-- */
#define MSEL_FSBL_SPI2_SDCARD			13 /* 1101 -_-- */
#define MSEL_ZSBL_SPI0_FLASH			14 /* 1110 -___ */
#define MSEL_FSBL_SPI2_SDCARD			14 /* 1110 -___ */
#define MSEL_ZSBL_SPI0_MMAP_QUAD		15 /* 1111 ____ */
#define MSEL_FSBL_SPI2_SDCARD			15 /* 1111 ____ */

#endif /* __MSEL_UNLEASHED_H_INCLUDE__ */
