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
 * @(#)uuid.c: universally unique identifier (UUID) implementation
 * $Id: uuid.c,v 1.1 2019-10-23 10:10:00 zhenglv Exp $
 */

#include <target/uuid.h>
#include <stdio.h>

const char *uuid_export(uuid_t u)
{
	static char s[UUID_LEN_STR+1];

	if (snprintf(s, UUID_LEN_STR+1,
		     "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		     (unsigned long)u.time_low,
		     (unsigned int)u.time_mid,
		     (unsigned int)u.time_hi_and_version,
		     (unsigned int)u.clock_seq_hi_and_reserved,
		     (unsigned int)u.clock_seq_low,
		     (unsigned int)u.node[0],
		     (unsigned int)u.node[1],
		     (unsigned int)u.node[2],
		     (unsigned int)u.node[3],
		     (unsigned int)u.node[4],
		     (unsigned int)u.node[5]) != UUID_LEN_STR) {
		return 0;
	}
	return s;
}

#if 0
bool guid_equal(const guid_t *a, const guid_t *b)
{
	int i;

	for (i = 0; i < GUID_SIZE; i++) {
		if (a->u.bytes[i] != b->u.bytes[i]) {
			return false;
		}
	}
	return true;
}
#endif
