/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009 - 2014
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
 * @(#)acpi_int.h: ACPI internal interfaces
 * $Id: acpi_int.h,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#ifndef __ACPI_INT_H_INCLUDE__
#define __ACPI_INT_H_INCLUDE__

#include <target/acpi.h>

/*=========================================================================
 * Table internals
 *=======================================================================*/
/* exported for internal table installation */
acpi_status_t acpi_table_install(acpi_addr_t address, acpi_tag_t signature,
				 acpi_table_flags_t flags,
				 boolean override, boolean versioning,
				 acpi_ddb_t *ddb_handle);
/* exported for interfacing with event callbacks */
void acpi_table_notify_existing(void);
/* exported for internal table initialization */
acpi_status_t acpi_rsdp_parse(acpi_addr_t rsdp_address);
acpi_status_t acpi_xsdt_parse(acpi_addr_t xsdt_address, uint32_t table_entry_size);
acpi_status_t acpi_xsdt_verify(acpi_addr_t xsdt_address);
void acpi_fadt_parse(struct acpi_table_header *table);

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2);

#endif /* __ACPI_INT_H_INCLUDE__ */
