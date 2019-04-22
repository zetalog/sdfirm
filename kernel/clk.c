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

#include <errno.h>
#include <target/clk.h>

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

int clk_register_driver(clk_cat_t category, struct clk_driver *clkd)
{
	if (clk_drivers[category])
		return -EBUSY;
	clk_drivers[category] = clkd;
	return 0;
}

int clk_init(void)
{
	return clk_hw_ctrl_init();
}
