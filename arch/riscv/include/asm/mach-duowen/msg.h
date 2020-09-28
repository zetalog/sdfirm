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
 * @(#)msg.h: DUOWEN shared message interface
 * $Id: msg.h,v 1.1 2020-08-06 14:43:00 zhenglv Exp $
 */

#ifndef __MSG_DUOWEN_H_INCLUDE__
#define __MSG_DUOWEN_H_INCLUDE__

#include <target/paging.h>

#define MSG_REG(offset)		(MSG_BASE + (offset))

#define MSG_TEST_ITEM		MSG_REG(0x00)
#define MSG_TEST_SUBITEM_LO	MSG_REG(0x04)
#define MSG_TEST_SUBITEM_HI	MSG_REG(0x08)
#define MSG_TEST_DONE		MSG_REG(0x0C)
#define MSG_TEST_STATUS_APC(n)	MSG_REG(0x10 + ((n) << 2))
#define MSG_TEST_STATUS_IMC	MSG_REG(0x50)
#define MSG_MSG			MSG_REG(0x60)

/* hmp cpu ID of IMC */
#define MSG_IMC			16

/* TEST_STATUS */
#define MSG_IDLE		_BV(0)
#define MSG_BUSY		_BV(1)
#define MSG_OK			_BV(2)
#define MSG_NG			_BV(3)

#ifdef CONFIG_DUOWEN_MSG
#define msg_imc_status(status)			\
	__raw_writel(status, MSG_TEST_STATUS_IMC)
#define msg_apc_status(apc, status)		\
	__raw_writel(status, MSG_TEST_STATUS_APC(apc))

#define msg_imc_test_start()					\
	do {							\
		for (int apc = 0; apc < 16; apc++) 		\
			msg_apc_status(apc, MSG_IDLE);		\
		msg_imc_status(MSG_BUSY);			\
	} while (0)
#define msg_apc_test_start(apc)		msg_apc_status(apc, MSG_BUSY)
#define msg_imc_finish()		__raw_setl(_BV(MSG_IMC), MSG_TEST_DONE)
#define msg_apc_finish(apc)		__raw_setl(_BV(apc), MSG_TEST_DONE)
#define msg_imc_test_stop(ok)					\
	do {							\
		msg_imc_status(ok ? MSG_OK : MSG_NG);		\
		msg_imc_finish();				\
	} while (0)
#define msg_apc_test_stop(apc, ok)				\
	do {							\
		msg_apc_status(apc, ok ? MSG_OK : MSG_NG);	\
		msg_apc_finish(apc);				\
	} while (0)
#define msg_imc_success()					\
	do {							\
		msg_imc_test_start();				\
		msg_imc_test_stop(true);			\
	} while (0)
#define msg_imc_failure()					\
	do {							\
		msg_imc_test_start();				\
		msg_imc_test_stop(false);			\
	} while (0)
#else
#define msg_imc_success()		do { } while (0)
#define msg_imc_falure()		do { } while (0)
#endif
#define msg_imc_shutdown()		msg_imc_success()

#endif /* __MSG_DUOWEN_H_INCLUDE__ */
