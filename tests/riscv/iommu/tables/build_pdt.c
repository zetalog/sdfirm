// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com
#include "iommu.h"
#include "tables_api.h"

uint64_t
add_process_context(
    device_context_t *DC, process_context_t *PC, uint32_t process_id) {
    uint64_t a;
    uint8_t i, LEVELS;
    pdte_t pdte;
    uint16_t PDI[3];

    PDI[0] = get_bits(7,   0, process_id);
    PDI[1] = get_bits(16,  8, process_id);
    PDI[2] = get_bits(19, 17, process_id);

    if ( DC->fsc.pdtp.MODE == PD20 ) LEVELS = 3;
    if ( DC->fsc.pdtp.MODE == PD17 ) LEVELS = 2;
    if ( DC->fsc.pdtp.MODE == PD8  ) LEVELS = 1;

    a = DC->fsc.pdtp.PPN * PAGESIZE;
    i = LEVELS - 1;
    while ( i > 0 ) {
        if ( translate_gpa(DC->iohgatp, a, &a) == -1 ) {
            printf("file: %s line: %d\n", __FILE__, __LINE__);
            return -1;
        }
        pdte.raw = 0;
        if ( read_memory((a + (PDI[i] * 8)), 8, (char *)&pdte.raw) ) return -1;
        if ( pdte.V == 0 ) {
            pdte.V = 1;
            pdte.reserved0 = pdte.reserved1 = 0;
            if (DC->iohgatp.MODE != IOHGATP_Bare) {
                gpte_t gpte;

                pdte.PPN = get_free_gppn(1, DC->iohgatp);

                gpte.raw = 0;
                gpte.V = 1;
                gpte.R = 1;
                gpte.W = 0;
                gpte.X = 0;
                gpte.U = 1;
                gpte.G = 0;
                gpte.A = 1;
                gpte.D = 0;
                gpte.PBMT = PMA;
                gpte.PPN = get_free_ppn(1);

                if ( add_g_stage_pte(DC->iohgatp, (PAGESIZE * pdte.PPN), gpte, 0) == -1 ) return -1;
            } else {
                pdte.PPN = get_free_ppn(1);
            }
            if ( write_memory((char *)&pdte.raw, (a + (PDI[i] * 8)), 8) ) return -1;
        }
        i = i - 1;
        a = pdte.PPN * PAGESIZE;
    }
    if ( translate_gpa(DC->iohgatp, a, &a) == -1 ) {
        printf("file: %s line: %d\n", __FILE__, __LINE__);
        return -1;
    }
    if ( write_memory((char *)PC, (a + (PDI[0] * 16)), 16) ) return -1;
    return (a + (PDI[0] * 16));
}

uint64_t get_pc(device_context_t *DC, uint32_t process_id)
{
    uint64_t a;
    uint8_t i, LEVELS;
    pdte_t pdte;
    uint16_t PDI[3];
    process_context_t PC;

    PDI[0] = get_bits(7,   0, process_id);
    PDI[1] = get_bits(16,  8, process_id);
    PDI[2] = get_bits(19, 17, process_id);

    if ( DC->fsc.pdtp.MODE == PD20 ) LEVELS = 3;
    if ( DC->fsc.pdtp.MODE == PD17 ) LEVELS = 2;
    if ( DC->fsc.pdtp.MODE == PD8  ) LEVELS = 1;

    a = DC->fsc.pdtp.PPN * PAGESIZE;
    i = LEVELS - 1;
    while ( i > 0 ) {
        if ( translate_gpa(DC->iohgatp, a, &a) == -1 ) return -1;
        pdte.raw = 0;
        if ( read_memory((a + (PDI[i] * 8)), 8, (char *)&pdte.raw) ) return -1;
        if ( pdte.V == 0 ) {
            return -1;
        }
        i = i - 1;
        a = pdte.PPN * PAGESIZE;
    }
    if ( translate_gpa(DC->iohgatp, a, &a) == -1 ) return -1;
    if ( read_memory((a + (PDI[0] * 16)), 16, (char *)&PC) ) return -1;
    if (PC.ta.V == 0) {
        return -1;
    }
    return (a + (PDI[0] * 16));
}
