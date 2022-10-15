/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)cci550.c: ARM CoreLink CCI-550 coherent interconnect implementation
 * $Id: cci550.c,v 1.1 2023-02-21 18:21:00 zhenglv Exp $
 */

#include <target/noc.h>
#include <target/barrier.h>

void cci_enable_snoop_dvm_reqs(unsigned int master_id)
{
	int slave_if_id = cci_hw_slave_map[master_id];

	/*
	 * Enable Snoops and DVM messages, no need for Read/Modify/Write
	 * as rest of bits are write ignore
	 */
	cci_enable_snoops(slave_if_id);
	cci_enable_dvms(slave_if_id);

	/*
	 * Wait for the completion of the write to the Snoop Control
	 * Register before testing the change_pending bit
	 */
	mb();

	/* Wait for the dust to settle down */
	while (cci_change_pending());
}

void cci_disable_snoop_dvm_reqs(unsigned int master_id)
{
	int slave_if_id = cci_hw_slave_map[master_id];

	/*
	 * Disable Snoops and DVM messages, no need for Read/Modify/Write
	 * as rest of bits are write ignore.
	 */
	cci_disable_snoops(slave_if_id);
	cci_disable_dvms(slave_if_id);

	/*
	 * Wait for the completion of the write to the Snoop Control
	 * Register before testing the change_pending bit
	 */
	mb();

	/* Wait for the dust to settle down */
	while (cci_change_pending());
}
