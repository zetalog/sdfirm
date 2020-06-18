/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)tbox.h: Vaisra trickbox and tube interface
 * $Id: tbox.h,v 1.1 2020-06-18 09:38:00 zhenglv Exp $
 */

#ifndef __VAISRA_TBOX_H_INCLUDE__
#define __VAISRA_TBOX_H_INCLUDE__

#ifndef TBOX_BASE
#define TBOX_BASE		ULL(0xFFFFFFF000)
#endif

#define TBOX_REG(offset)	(TBOX_BASE + (offset))

#define TBOX_TUBE		TBOX_REG(0)
#define TBOX

#define TBOX_CHAR_LF		0x0A
#define TBOX_CHAR_CR		0x0D
#define TBOX_CHAR_EOT		0x04 /* ends the test */
#define TBOX_CHAR_ERR		0x08
#define TBOX_CHAR_NUL		0x00

#define TBOX_CHAR_EOT_CPU(cpu)	((((cpu) + 1) << 4) + TBOX_CHAR_EOT)
#define TBOX_CHAR_ERR_CPU(cpu)	((((cpu) + 1) << 4) + TBOX_CHAR_ERR)

#ifndef __ASSEMBLY__
void tbox_finish(void);
void tbox_error(void);
#endif

#endif /* __VAISRA_TBOX_H_INCLUDE__ */
