// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com
#include "iommu.h"
#include "tables_api.h"

uint64_t add_dev_context(device_context_t *DC, uint32_t device_id)
{
    uint64_t a;
    uint8_t i, LEVELS, DC_SIZE;
    ddte_t ddte;
    ddtp_t ddtp;
    uint16_t DDI[3];
    capabilities_t capabilities;
    // The DDT used to locate the DC may be configured to be a 1, 2, or 3 level 
    // radix-table depending on the maximum width of the device_id supported. 
    // The partitioning of the device_id to obtain the device directory indexes
    // (DDI) to traverse the DDT radix-tree table are as follows:
    capabilities.raw = read_register(CAPABILITIES_OFFSET, 8);
    if ( capabilities.msi_flat == 0 ) {
        DDI[0] = get_bits(6,   0, device_id);
        DDI[1] = get_bits(15,  7, device_id);
        DDI[2] = get_bits(23, 16, device_id);
        DC_SIZE = BASE_FORMAT_DC_SIZE;
    } else {
        DDI[0] = get_bits(5,   0, device_id);
        DDI[1] = get_bits(14,  6, device_id);
        DDI[2] = get_bits(23, 15, device_id);
        DC_SIZE = EXT_FORMAT_DC_SIZE;
    }

    ddtp.raw = read_register(DDTP_OFFSET, 8);
    a = ddtp.ppn * PAGESIZE;
    if ( ddtp.iommu_mode == DDT_3LVL ) LEVELS = 3;
    if ( ddtp.iommu_mode == DDT_2LVL ) LEVELS = 2;
    if ( ddtp.iommu_mode == DDT_1LVL ) LEVELS = 1;
    i = LEVELS - 1;
    while ( i > 0 ) {
        ddte.raw = 0;
        if ( read_memory((a + (DDI[i] * 8)), 8, (char *)&ddte.raw) ) return -1;
        if ( ddte.V == 0 ) {
            ddte.V = 1;
            ddte.PPN = get_free_ppn(1);
            if ( write_memory((char *)&ddte.raw, (a + (DDI[i] * 8)), 8) ) return -1;
        }
        i = i - 1;
        a = ddte.PPN * PAGESIZE;
    }
    if ( write_memory((char *)DC, (a + (DDI[0] * DC_SIZE)), DC_SIZE) ) return -1;
    return (a + (DDI[0] * DC_SIZE));
}

uint64_t get_dc(uint32_t device_id)
{
    uint64_t a;
    uint64_t dc_addr;
    uint8_t i, LEVELS, DC_SIZE;
    ddte_t ddte;
    ddtp_t ddtp;
    uint16_t DDI[3];
    capabilities_t capabilities;
    device_context_t DC;

    // The DDT used to locate the DC may be configured to be a 1, 2, or 3 level 
    // radix-table depending on the maximum width of the device_id supported. 
    // The partitioning of the device_id to obtain the device directory indexes
    // (DDI) to traverse the DDT radix-tree table are as follows:
    capabilities.raw = read_register(CAPABILITIES_OFFSET, 8);
    if ( capabilities.msi_flat == 0 ) {
        DDI[0] = get_bits(6,   0, device_id);
        DDI[1] = get_bits(15,  7, device_id);
        DDI[2] = get_bits(23, 16, device_id);
        DC_SIZE = BASE_FORMAT_DC_SIZE;
    } else {
        DDI[0] = get_bits(5,   0, device_id);
        DDI[1] = get_bits(14,  6, device_id);
        DDI[2] = get_bits(23, 15, device_id);
        DC_SIZE = EXT_FORMAT_DC_SIZE;
    }
    ddtp.raw = read_register(DDTP_OFFSET, 8);
    a = ddtp.ppn * PAGESIZE;
    if ( ddtp.iommu_mode == DDT_3LVL ) LEVELS = 3;
    if ( ddtp.iommu_mode == DDT_2LVL ) LEVELS = 2;
    if ( ddtp.iommu_mode == DDT_1LVL ) LEVELS = 1;
    i = LEVELS - 1;
    while ( i > 0 ) {
        ddte.raw = 0;
        if ( read_memory((a + (DDI[i] * 8)), 8, (char *)&ddte.raw) ) return -1;
        if ( ddte.V == 0 ) {
            return -1;
        }
        i = i - 1;
        a = ddte.PPN * PAGESIZE;
    }
    if ( read_memory((a + (DDI[0] * DC_SIZE)), DC_SIZE, (char *)&DC) ) return -1;
    if (DC.tc.V == 0) {
        return -1;
    }
    dc_addr = a + (DDI[0] * DC_SIZE);
    return dc_addr;
}
