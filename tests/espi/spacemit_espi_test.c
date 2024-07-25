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
 * @(#)spacemit_espi_test.c: SpacemiT eSPI test implementation
 * $Id: spacemit_espi_test.c,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */
#include <target/console.h>
#include <target/barrier.h>
#include <target/irq.h>
#include <target/stream.h>
#include <target/cmdline.h>
#include <target/bh.h>

#include <driver/espi.h>
#include <driver/spacemit_espi.h>

static int do_espi_read(int argc, char *argv[])
{
	caddr_t addr;

	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "fw") == 0) {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)(uint32_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1"))
			return __raw_readb(addr);
		else if (strcmp(argv[3], "2"))
			return __raw_readw(addr);
		else if (strcmp(argv[3], "4"))
			return __raw_readl(addr);
		return -EINVAL;
	} else {
		addr = (caddr_t)(uint32_t)strtoull(argv[3], 0, 0);
		if (strcmp(argv[2], "io") == 0)
			return __raw_readb(addr);
		else if (strcmp(argv[2], "mem") == 0)
			return __raw_readb(addr);
	}
	return -EINVAL;
}

static int do_espi_write(int argc, char *argv[])
{
	caddr_t addr;

	if (argc < 5)
		return -EINVAL;
	if (strcmp(argv[2], "fw") == 0) {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			__raw_writeb(v, addr);
		else if (size == 2)
			__raw_writew(v, addr);
		else if (size == 4)
			__raw_writel(v, addr);
		else
			return -EINVAL;
		return 0;
	} else {
		uint8_t v;

		v = (uint32_t)strtoull(argv[3], 0, 0);
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[2], "io") == 0)
			__raw_writeb(v, addr);
		else if (strcmp(argv[2], "mem") == 0)
			__raw_writeb(v, addr);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

static int do_espi_send(int argc, char *argv[])
{
	if (argc < 5)
		return -EINVAL;
	if (strcmp(argv[2], "vw") == 0) {
		int idx = (uint32_t)strtoull(argv[3], 0, 0);
		int val = (uint32_t)strtoull(argv[4], 0, 0);
		spacemit_espi_tx_vw((uint8_t *)&idx, (uint8_t *)&val, 1);
	} else if (strcmp(argv[2], "oob") == 0) {
		long val = (uint32_t)strtoull(argv[3], 0, 0);
		int len = (uint32_t)strtoull(argv[4], 0, 0);
		if (len > 8)
			return -EINVAL;
		spacemit_espi_tx_oob((uint8_t *)&val, len);
	}

	return 0;
}

static void rxvw_cb(int group, uint8_t rxvw_data)
{
	printf("group = %d, rxvw_data=0x%02x\n", group, rxvw_data);
}

static void rxoob_cb(void *buffer, int len)
{
	uint8_t *buf = (uint8_t *)buffer;

	printf("buffer:\n");
	for (int i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
	}
	printf("\n");
}

static int do_espi_recv(int argc, char *argv[])
{
	if (strcmp(argv[2], "vw") == 0) {
		espi_register_rxvw_callback(rxvw_cb);
	} else if (strcmp(argv[2], "oob") == 0) {
		espi_register_rxvw_callback(rxoob_cb);
	}

	return 0;
}

static int do_espi(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_espi_read(argc, argv);
	else if (strcmp(argv[1], "write") == 0)
		return do_espi_write(argc, argv);
	else if (strcmp(argv[1], "send") == 0)
		return do_espi_send(argc, argv);
	else if (strcmp(argv[1], "recv") == 0)
		return do_espi_recv(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(espi, do_espi, "SpacemiT eSPI commands",
	/*"espi read io\n"
	"espi read mem\n"
	"espi read fw [1|2|4]\n"
	"    -eSPI read sequence\n"
	"espi write io value\n"
	"espi write mem value\n"
	"espi write fw value [1|2|4]\n"
	"    -eSPI write sequence\n"*/
	"espi send vw <idx> <val>\n"
	"espi send oob <val> <len>\n"
	"espi recv vw\n"
	"espi recv oob\n"
);
