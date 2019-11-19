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
 * @(#)clk.c: clock tree framework implementation
 * $Id: clk.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/cmdline.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

struct clk_driver *clk_drivers[MAX_CLK_DRIVERS];

uint32_t clk_get_frequency(clk_t clk)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);

	if (cat >= MAX_CLK_DRIVERS)
		return -EINVAL;
	clkd = clk_drivers[cat];
	if (!clkd)
		return -EINVAL;
	BUG_ON(!clkd->get_freq);
	return clkd->get_freq(clk_clk(clk));
}

int clk_enable(clk_t clk)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);
	int ret = 0;

	if (cat >= MAX_CLK_DRIVERS)
		return -ENODEV;
	clkd = clk_drivers[cat];
	if (!clkd)
		return -ENODEV;
	if (clkd->enable)
		ret = clkd->enable(clk_clk(clk));
	return ret;
}

int clk_set_frequency(clk_t clk, uint32_t freq)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);

	if (cat >= MAX_CLK_DRIVERS)
		return -EINVAL;
	clkd = clk_drivers[cat];
	if (!clkd || !clkd->set_freq)
		return -EINVAL;
	return clkd->set_freq(clk_clk(clk), freq);
}

void clk_disable(clk_t clk)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);

	if (cat >= MAX_CLK_DRIVERS)
		return;
	clkd = clk_drivers[cat];
	if (!clkd || !clkd->disable)
		return;
	clkd->disable(clk_clk(clk));
}

void clk_select_source(clk_t clk, clk_t src)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);

	if (cat >= MAX_CLK_DRIVERS)
		return;
	clkd = clk_drivers[cat];
	if (!clkd)
		return;
	if (clkd->select)
		clkd->select(clk_clk(clk), src);
}

const char *clk_get_mnemonic(clk_t clk)
{
	struct clk_driver *clkd;
	clk_cat_t cat = clk_cat(clk);

	if (cat >= MAX_CLK_DRIVERS)
		return NULL;
	clkd = clk_drivers[cat];
	if (!clkd || !clkd->get_name)
		return NULL;
	return clkd->get_name(clk_clk(clk));
}

int clk_register_driver(clk_cat_t category, struct clk_driver *clkd)
{
	if (category >= MAX_CLK_DRIVERS)
		return -EINVAL;
	if (clk_drivers[category])
		return -EBUSY;
	clk_drivers[category] = clkd;
	return 0;
}

void clk_init(void)
{
	clk_hw_ctrl_init();
}

clk_t clk_search_mnemonic(const char *mnem)
{
	clk_cat_t cat;
	clk_clk_t clk;
	struct clk_driver *clkd;
	clk_t clkid;
	const char *name;

	for (cat = 0; cat < MAX_CLK_DRIVERS; cat++) {
		if (clk_drivers[cat]) {
			clkd = clk_drivers[cat];
			for (clk = 0; clk < clkd->max_clocks; clk++) {
				clkid = clkid(cat, clk);
				name = clk_get_mnemonic(clkid);
				if (name && strcmp(name, mnem) == 0)
					return clkid;
			}
		}
	}
	return invalid_clk;
}

static int do_clk_dump(int argc, char *argv[])
{
	clk_cat_t cat;
	clk_clk_t clk;
	struct clk_driver *clkd;
	clk_t clkid;
	const char *name;

	printf("%20s: %-10s\n", "clock", "frequency");
	for (cat = 0; cat < MAX_CLK_DRIVERS; cat++) {
		if (clk_drivers[cat]) {
			clkd = clk_drivers[cat];
			for (clk = 0; clk < clkd->max_clocks; clk++) {
				clkid = clkid(cat, clk);
				name = clk_get_mnemonic(clkid);
				if (name)
					printf("%20s: %-10d\n", name,
					       clk_get_frequency(clkid));
			}
		}
	}
	return 0;
}

static int do_clk_select(int argc, char *argv[])
{
	clk_t clkid;
	clk_t srcid;

	if (argc < 4)
		return -EINVAL;
	clkid = clk_search_mnemonic(argv[2]);
	if (clkid == invalid_clk) {
		printf("invalid clock: %s\n.", argv[2]);
		return -EINVAL;
	}
	srcid = clk_search_mnemonic(argv[3]);
	if (srcid == invalid_clk) {
		printf("invalid clock: %s\n.", argv[3]);
		return -EINVAL;
	}
	clk_select_source(clkid, srcid);
	printf("done.\n");
	return 0;
}

static int do_clk_enable(int argc, char *argv[])
{
	clk_t clkid;
	int ret;

	if (argc < 3)
		return -EINVAL;
	clkid = clk_search_mnemonic(argv[2]);
	if (clkid == invalid_clk) {
		printf("invalid clock: %s.\n", argv[2]);
		return -EINVAL;
	}
	ret = clk_enable(clkid);
	if (ret) {
		printf("failure.\n");
		return -EINVAL;
	}
	printf("success.\n");
	return 0;
}

static int do_clk_disable(int argc, char *argv[])
{
	clk_t clkid;

	if (argc < 3)
		return -EINVAL;
	clkid = clk_search_mnemonic(argv[2]);
	if (clkid == invalid_clk) {
		printf("invalid clock: %s.\n", argv[2]);
		return -EINVAL;
	}
	clk_disable(clkid);
	printf("done.\n");
	return 0;
}

static int do_clk_freq(int argc, char *argv[])
{
	clk_t clkid;
	uint32_t freq;
	int ret;

	if (argc < 3)
		return -EINVAL;
	clkid = clk_search_mnemonic(argv[2]);
	if (clkid == invalid_clk) {
		printf("invalid clock: %s.\n", argv[2]);
		return -EINVAL;
	}
	if (argc > 3) {
		freq = strtoul(argv[3], NULL, 0);
		ret = clk_set_frequency(clkid, freq);
		if (ret) {
			printf("faiure: %s %10d.\n", argv[2], freq);
			return ret;
		}
		printf("success.\n");
	} else {
		freq = clk_get_frequency(clkid);
		if (freq == INVALID_FREQ) {
			printf("faiure: %s.\n", argv[2]);
			return -EINVAL;
		}
		printf("%20s: %-10d\n", argv[2], freq);
	}
	return 0;
}

static int do_clk(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_clk_dump(argc, argv);
	if (strcmp(argv[1], "select") == 0)
		return do_clk_select(argc, argv);
	if (strcmp(argv[1], "enable") == 0)
		return do_clk_enable(argc, argv);
	if (strcmp(argv[1], "disable") == 0)
		return do_clk_disable(argc, argv);
	if (strcmp(argv[1], "freq") == 0)
		return do_clk_freq(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(clk, do_clk, "Control clock tree",
	"clk dump\n"
	"    -display clock settings\n"
	"clk select clk src\n"
	"    -select clock source multiplexing\n"
	"clk enable clk\n"
	"    -enable clock\n"
	"clk disable clk\n"
	"    -disable clock\n"
	"clk freq clk [freq]\n"
	"    -set/get clock frequency\n"
	"\n"
);
