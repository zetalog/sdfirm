/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2012
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
 * @(#)pcd.c: ISO/IEC 14443-4 proximity coupling device functions
 * $Id: pcd.c,v 1.1 2011-11-14 09:00:01 zhenglv Exp $
 */
#include "pcd_priv.h"
#include <target/state.h>
#include <target/delay.h>
#include <target/timer.h>

#define PCD_CRC_A 1
#define PCD_CRC_B 2

uint16_t pcd_crc16_update(uint8_t ch, uint16_t *pcrc)
{
	ch = (ch^(uint8_t)((*pcrc) & 0x00FF));
	ch = (ch^(ch<<4));
	*pcrc = (*pcrc >> 8)^
		((uint16_t)ch << 8)^
		((uint16_t)ch << 3)^
		((uint16_t)ch >> 4);
	return (*pcrc);
}

void pcd_crc16_compute(int type, char *buf, size_t len,
			uint8_t *xmit_1st, uint8_t *xmit_2nd)
{
	uint8_t c;
	uint16_t crc;
	switch (type) {
	case PCD_CRC_A:
		crc = 0x6363; /* ITU-V.41 */
		break;
	case PCD_CRC_B:
		crc = 0xFFFF; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
		break;
	default:
		return;
	}
	do {
		c = *buf++;
		pcd_crc16_update(c, &crc);
	} while (--len);
	if (type == PCD_CRC_B)
		crc = ~crc; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
	*xmit_1st = (uint8_t)(crc & 0xFF);
	*xmit_2nd = (uint8_t)((crc >> 8) & 0xFF);
}

#ifdef CONFIG_PCD_TEST
void pcd_crc16_test(void)
{
	int i;
	uint8_t buf_a[10] = {0x12, 0x34};
	uint8_t buf_b[10] = {0x0A, 0x12, 0x34, 0x56};
	uint8_t first, second;

	printf("CRC-16 reference results ISO/IEC 14443-3\n");
	printf("Crc-16 G(x) = x^16 + x^12 + x^5 + 1\n\n");

	printf("CRC_A of [ ");
	for (i = 0; i < 2; i++)
		printf("%02X ", buf_a[i]);
	pcd_crc16_compute(PCD_CRC_A, buf_a, 2, &first, &second);
	printf("] Transmitted: %02X then %02X.\n", first, second);

	printf("CRC_B of [ ");
	for (i = 0; i < 4; i++)
		printf("%02X ", buf_b[i]);
	pcd_crc16_compute(PCD_CRC_B, buf_b, 4, &first, &second);
	printf("] Transmitted: %02X then %02X.\n", first, second);
}
#endif
