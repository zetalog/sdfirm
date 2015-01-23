/*
 * ZETALOG's Personal COPYRIGHT v2
 *
 * Copyright (c) 2014
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 2. Permission of reuse of souce code only granted to ZETALOG and the
 *    developer(s) in the companies ZETALOG worked and has redistributed this
 *    software to.  Permission of redistribution of source code only granted
 *    to ZETALOG.
 * 3. Permission of redistribution and/or reuse of binary fully granted
 *    to ZETALOG and the companies ZETALOG worked and has redistributed this
 *    software to.
 * 4. Any modification of this software in the redistributed companies need
 *    not be published to ZETALOG.
 * 5. All source code modifications linked/included this software and modified
 *    by ZETALOG are of ZETALOG's personal COPYRIGHT unless the above COPYRIGHT
 *    is no long disclaimed.
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
 * @(#)acpi_generic.c: ACPI generic hardware implementation
 * $Id: acpi_generic.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "acpi_int.h"

static struct acpi_namespace_node *acpi_gbl_fadt_gpe_device = NULL;

void acpi_generic_lock(void)
{
}

void acpi_generic_unlock(void)
{
}

int acpi_genhw_init(void)
{
#if 0
	uint8_t count0 = 0;
	uint8_t count1 = 0;
	uint16_t gpe_no = 0;
	acpi_status_t status;

	acpi_generic_lock();
	if (acpi_gbl_FADT.gpe0_block_length && acpi_gbl_FADT.xgpe0_block.address) {
		count0 = acpi_gbl_FADT.gpe0_block_length / 2;
		gpe_no = (count0 * ACPI_GPE_REGISTER_WIDTH) - 1;
		status = acpi_ev_create_gpe_block(acpi_gbl_fadt_gpe_device,
						  &acpi_gbl_FADT.xgpe0_block,
						  count0, 0,
						  acpi_gbl_FADT.sci_interrupt,
						  &acpi_gbl_gpe_fadt_blocks[0]);
		if (ACPI_FAILURE(status))
			acpi_err("Could not create GPE Block 0");
	}
	if (acpi_gbl_FADT.gpe1_block_length && acpi_gbl_FADT.xgpe1_block.address) {
		count1 = acpi_gbl_FADT.gpe1_block_length / 2;
		if ((count0) && (gpe_no >= acpi_gbl_FADT.gpe1_base)) {
			acpi_err("GPE0 block (GPE 0 to %u) overlaps the GPE1 block "
				 "(GPE %u to %u) - Ignoring GPE1",
				 gpe_no, acpi_gbl_FADT.gpe1_base,
				 acpi_gbl_FADT.gpe1_base +
				 ((count1 * ACPI_GPE_REGISTER_WIDTH) - 1));
			count1 = 0;
		} else {
			status = acpi_ev_create_gpe_block(acpi_gbl_fadt_gpe_device,
							  &acpi_gbl_FADT.xgpe1_block,
							  count1,
							  acpi_gbl_FADT.gpe1_base,
							  acpi_gbl_FADT.
							  sci_interrupt,
							  &acpi_gbl_gpe_fadt_blocks[1]);
			if (ACPI_FAILURE(status))
				acpi_err("Could not create GPE Block 1");
			gpe_no = acpi_gbl_FADT.gpe1_base + (count1 * ACPI_GPE_REGISTER_WIDTH) - 1;
		}
	}
	if ((count0 + count1) == 0) {
		acpi_info("There are no GPE blocks defined in the FADT\n");
		status = AE_OK;
		goto cleanup;
	}
	if (gpe_no > ACPI_GPE_MAX) {
		acpi_err("Maximum GPE number from FADT is too large: 0x%X", gpe_no);
		status = AE_BAD_VALUE;
		goto cleanup;
	}

cleanup:
	acpi_generic_unlock();
#endif
	return 0;
}
