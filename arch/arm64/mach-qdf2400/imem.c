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
 * @(#)imem.c: internal memory (imem) ECC RAS implementation
 * $Id: imem.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <stdio.h>
#include <asm/mach/imem.h>

QDF2400_IMEM(sys_imem, SYS_IMEM);
QDF2400_IMEM(msg_imem, MSG_IMEM);
QDF2400_IMEM(ram0_imem, RAM_0_IMEM);
QDF2400_IMEM(ram1_imem, RAM_1_IMEM);
QDF2400_IMEM(ram2_imem, RAM_2_IMEM);
QDF2400_IMEM(ram3_imem, RAM_3_IMEM);
QDF2400_IMEM(ram4_imem, RAM_4_IMEM);
QDF2400_IMEM(ram5_imem, RAM_0_IMEM);

char *imem_type_strings[IMEM_TYPE_NUM] = {
	"RAM_0_IMEM",
	"RAM_1_IMEM",
	"RAM_2_IMEM",
	"RAM_3_IMEM",
	"RAM_4_IMEM",
	"RAM_5_IMEM",
	"SYS_IMEM",
	"MSG_IMEM",
};

char *imem_syndrome_strings[IMEM_SYNDROME_NUM] = {
	"NO_ERR",
	"ECC_CE_READ",
	"ECC_UE_READ",
	"ECC_POISON_READ",
	"ECC_CE_WRITE",
	"ECC_UE_WRITE",
	"ECC_POISON_WRITE",
	"ECC_CE_SCRUB",
	"ECC_UE_SCRUB",
	"ECC_POISION_SCRUB",
};

void imem_print_err(struct imem_err *err)
{
	printf("%s %016llx %s\n",
	       err->imem_type < IMEM_TYPE_NUM ?
	       imem_type_strings[err->imem_type] : "UNKNOWN",
	       err->address,
	       err->syndrome < IMEM_SYNDROME_NUM ?
	       imem_syndrome_strings[err->syndrome] : "UNKNOWN");
}

int imem_init(void)
{
	ram0_imem_init();
	ram1_imem_init();
	ram2_imem_init();
	ram3_imem_init();
	ram4_imem_init();
	ram5_imem_init();
	sys_imem_init();
	msg_imem_init();
	return 0;
}
