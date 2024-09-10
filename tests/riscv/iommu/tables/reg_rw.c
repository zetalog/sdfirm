// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com
#include <target/iommu.h>
#include <target/io.h>
#include "iommu.h"

#define RV_IOMMU_BASE(n)	ULL(0x04780000000)

uint64_t read_register(uint16_t offset, uint8_t num_bytes)
{
    return ( num_bytes == 4 ) ? __raw_readl((caddr_t)RV_IOMMU_BASE(0) + offset) :
                                __raw_readq((caddr_t)RV_IOMMU_BASE(0) + offset);
}
void write_register(uint16_t offset, uint8_t num_bytes, uint64_t data)
{
    return ( num_bytes == 4 ) ? __raw_writel(data, (caddr_t)RV_IOMMU_BASE(0) + offset) :
                                __raw_writeq(data, (caddr_t)RV_IOMMU_BASE(0) + offset);
}
