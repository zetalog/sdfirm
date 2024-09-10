// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com

#include <target/io.h>
#include <target/console.h>
#include "iommu.h"
#include "tables_api.h"
#include "test_app.h"

static void con_cap(capabilities_t *cap, char *help)
{
    con_log("----cap start(%s)-----\n", help ? help : "");
    con_log("raw            = 0x%llx\n", cap->raw);
    con_log("version        = 0x%x\n", cap->version);
    con_log("Sv32           = 0x%x\n", cap->Sv32);
    con_log("Sv39           = 0x%x\n", cap->Sv39);
    con_log("Sv48           = 0x%x\n", cap->Sv48);
    con_log("Sv57           = 0x%x\n", cap->Sv57);
    con_log("rsvd0          = 0x%x\n", cap->rsvd0);
    con_log("Svpbmt         = 0x%x\n", cap->Svpbmt);
    con_log("Sv32x4         = 0x%x\n", cap->Sv32x4);
    con_log("Sv39x4         = 0x%x\n", cap->Sv39x4);
    con_log("Sv48x4         = 0x%x\n", cap->Sv48x4);
    con_log("Sv57x4         = 0x%x\n", cap->Sv57x4);
    con_log("rsvd1          = 0x%x\n", cap->rsvd1);
    con_log("amo_mrif       = 0x%x\n", cap->amo_mrif);
    con_log("msi_flat       = 0x%x\n", cap->msi_flat);
    con_log("msi_mrif       = 0x%x\n", cap->msi_mrif);
    con_log("amo_hwad       = 0x%x\n", cap->amo_hwad);
    con_log("ats            = 0x%x\n", cap->ats);
    con_log("t2gpa          = 0x%x\n", cap->t2gpa);
    con_log("end            = 0x%x\n", cap->end);
    con_log("igs            = 0x%x\n", cap->igs);
    con_log("hpm            = 0x%x\n", cap->hpm);
    con_log("dbg            = 0x%x\n", cap->dbg);
    con_log("pas            = 0x%x\n", cap->pas);
    con_log("pd8            = 0x%x\n", cap->pd8);
    con_log("pd17           = 0x%x\n", cap->pd17);
    con_log("pd20           = 0x%x\n", cap->pd20);
    con_log("rsvd3          = 0x%x\n", cap->rsvd3);
    con_log("custom         = 0x%x\n", cap->custom);
    con_log("-----cap end-----\n");
}

static int g_iommu_init = 0;
static int iommu_init(void)
{
    capabilities_t cap;

    if (g_iommu_init) {
        return 0;
    }

    g_iommu_init = 1;
    fail_if( (enable_cq(4) < 0));
    fail_if( (enable_fq(4) < 0));
    // fail_if( (enable_disable_pq(4, 1) < 0));

    cap.raw = read_register(CAPABILITIES_OFFSET, 8);
    con_cap(&cap, "cap");

    return 0;
}

static uint32_t page_offset = 0;

static uint64_t iova_get(void)
{
    return 0x10000 + (page_offset ++) * PAGESIZE;
}

int add_dc_pc_s1(int ddi, int pdi, uint64_t *iova, uint64_t *spa)
{
    uint64_t DC_addr, pte_addr;
    uint64_t gva, PC_addr;
    device_context_t DC;
    process_context_t PC;
    pte_t pte;

    DC_addr = get_dc(ddi);
    if (DC_addr == (uint64_t)-1) {
        DC_addr = add_device(ddi, 0x1234, 1, 0, 0, 0, 0,
                            0, 0, 1, 0, 0,
                            IOHGATP_Bare, IOSATP_Bare, PD8,
                            MSIPTP_Off, 1, 0xFFFFFFFFFF, 0x1000000000);
        read_memory(DC_addr, 64, (char *)&DC);
        DC.msiptp.MODE = MSIPTP_Off;
        write_memory((char *)&DC, DC_addr, 64);
        iodir(INVAL_DDT, 1, ddi, 0);
    } else {
        read_memory(DC_addr, 64, (char *)&DC);
    }
    con_log("DC addr: 0x%llx\n", DC_addr);

    PC_addr = get_pc(&DC, pdi);
    if (PC_addr == (uint64_t)-1) {
        memset(&PC, 0, 16);
        PC.fsc.iosatp.MODE = IOSATP_Sv39;
        PC.fsc.iosatp.PPN = get_free_ppn(1);
        PC.ta.V = 1;
        PC.ta.PSCID = 0xa5;
        // PC.ta.PSCID = ddi_pdi_2_scid(ddi, pdi);
        PC.ta.ENS = 0;
        PC.ta.SUM = 0;
        PC_addr = add_process_context(&DC, &PC, pdi);
        fail_if( (read_memory(PC_addr, 16, (char *)&PC) != 0) );
        iodir(INVAL_PDT, 1, ddi, pdi);
    } else {
        read_memory(PC_addr, 16, (char *)&PC);
    }
    con_log("PC addr: 0x%llx\n", PC_addr);

    pte.raw = 0;
    pte.V = 1;
    pte.R = 1;
    pte.W = 1;
    pte.X = 0;
    pte.U = 1;
    pte.G = 0;
    pte.A = 1;
    pte.D = 0;
    pte.PBMT = PMA;
    pte.PPN = get_free_ppn(1);

    gva = iova_get();
    pte_addr = add_s_stage_pte(PC.fsc.iosatp, gva, pte, 0);
    fail_if( (pte_addr == -1) );
    iotinval(VMA, 1, 0, 1, 0, PC.ta.PSCID, 0);
    *spa = pte.PPN * PAGESIZE;
    *iova = gva;
    // translate_s1(PC.fsc.iosatp, gva);
    return gva;
}

int add_dc_pc_s1_s2(int ddi, int pdi, uint64_t *iova, uint64_t *iopa)
{
    uint64_t DC_addr, gva, PC_addr, spa, temp, gpa;
    device_context_t DC;
    process_context_t PC;
    pdte_t pdte;
    pte_t pte;
    gpte_t gpte;

    DC_addr = get_dc(ddi);
    if (DC_addr == (uint64_t)-1) {
        DC_addr = add_device(ddi, 0x1234, 1, 0, 0, 0, 0, 
                            0, 0, 1, 0, 0,
                            IOHGATP_Sv39x4, IOSATP_Sv39, PD8,
                            MSIPTP_Off, 1, 0xFFFFFFFFFF, 0x1000000000);
        read_memory(DC_addr, 64, (char *)&DC);
        DC.msiptp.MODE = MSIPTP_Off;
        write_memory((char *)&DC, DC_addr, 64);
        iodir(INVAL_DDT, 1, ddi, 0);
    } else {
        read_memory(DC_addr, 64, (char *)&DC);
    }

    con_log("DC addr: 0x%llx\n", DC_addr);

    // Add process context
    PC_addr = get_pc(&DC, pdi);
    if (PC_addr == (uint64_t)-1) {
        memset(&PC, 0, 16);
        PC.fsc.iosatp.MODE = IOSATP_Sv39;
        PC.fsc.iosatp.PPN = get_free_gppn(1, DC.iohgatp);
        gpte.raw = 0;
        gpte.V = 1;
        gpte.R = 1;
        gpte.W = 1;
        gpte.X = 1;
        gpte.U = 1;
        gpte.G = 0;
        gpte.A = 1;
        gpte.D = 0;
        gpte.PBMT = PMA;
        gpte.PPN = get_free_ppn(1);
        add_g_stage_pte(DC.iohgatp, (PC.fsc.iosatp.PPN * PAGESIZE), gpte, 0);
        PC.ta.V = 1;
        PC.ta.PSCID = 10;
        PC.ta.ENS = 1;
        PC.ta.SUM = 1;
        PC_addr = add_process_context(&DC, &PC, pdi);
        fail_if( (read_memory(PC_addr, 16, (char *)&PC) != 0) );
        iodir(INVAL_PDT, 1, ddi, pdi);
    } else {
        read_memory(PC_addr, 16, (char *)&PC);
    }
    con_log("PC addr: 0x%llx\n", PC_addr);

    // misconfigured NL PTE
    fail_if( (translate_gpa(DC.iohgatp, DC.fsc.pdtp.PPN * PAGESIZE, &temp) == -1) );
    temp = (temp) | (get_bits(19, 17, 0x3) * 8);
    read_memory(temp, 8, (char *)&pdte);
    pdte.reserved0 = 1;
    write_memory((char *)&pdte, temp, 8);
    pdte.reserved0 = 0;
    write_memory((char *)&pdte, temp, 8);

    PC.ta.reserved0 = 0;
    PC.ta.reserved1 = 0;
    write_memory((char *)&PC, PC_addr, 16);

    PC.ta.V = 1;
    write_memory((char *)&PC, PC_addr, 16);

    gpte.V = 1;
    // Two stage translation
    iodir(INVAL_DDT, 1, 0x1, 0);
    pte.raw = 0;
    pte.V = 1;
    pte.R = 1;
    pte.W = 1;
    pte.X = 1;
    pte.U = 1;
    pte.G = 0;
    pte.A = 1;
    pte.D = 0;
    pte.PBMT = PMA;
    pte.PPN = get_free_gppn(1, DC.iohgatp);

    gpte.raw = 0;
    gpte.V = 1;
    gpte.R = 1;
    gpte.W = 1;
    gpte.X = 1;
    gpte.U = 1;
    gpte.G = 0;
    gpte.A = 1;
    gpte.D = 0;
    gpte.PBMT = PMA;
    gpte.PPN = get_free_ppn(1);
    gpa = pte.PPN * PAGESIZE;
    spa = gpte.PPN * PAGESIZE;
    add_g_stage_pte(DC.iohgatp, gpa, gpte, 0);
    gva = iova_get();
    add_vs_stage_pte(PC.fsc.iosatp, gva, pte, 0, DC.iohgatp);

    iotinval(VMA, 1, 0, 1, 0, PC.ta.PSCID, 0);
    // memset(spa, 0xa5, 4);
    *iova = gva;
    *iopa = spa;

    return 0;
}

int dma_msg_send(void *src, void *dst, size_t size)
{
    // TODO 
#define BIT(nr)			(1UL << (nr))
#define DWAXIDMAC_IRQ_DMA_TRF BIT(1)
    uint64_t src_addr = (uint64_t)src;
    uint64_t dst_addr = (uint64_t)dst;
    uint32_t status = __raw_readl(0x424c00000+0x188); //get irq status
    if (status) {
        __raw_writel(status , 0x424c00000+0x198); // clean irq status
    }

    // printf("src: 0x%lx dst: 0x%lx\n", src_addr, dst_addr);
    // __raw_writel(0x01,          0x4780000000+0x10); //iommu bare
    __raw_writel(src_addr,      0x424c00000+0x100);
    __raw_writel(dst_addr,      0x424c00000+0x108);
    __raw_writel(0xcd200,       0x424c00000+0x118);
    __raw_writel(0x3108840,     0x424c00000+0x11c);
    __raw_writel(0x0,           0x424c00000+0x120);
    __raw_writel(0x8818000,     0x424c00000+0x124);
    __raw_writel(0x1,           0x424c00000+0x110);
    __raw_writel(0x3,           0x424c00000+0x010);
    __raw_writel(0x10001,       0x424c00000+0x018);

    while (1) {
        status = __raw_readl(0x424c00000+0x188); //get irq status
        if (status) {
            // printf("get dma irq status: 0x%x\n", status);
            // if(status & DWAXIDMAC_IRQ_DMA_TRF) {
            //     printf("dma xfer complete\n");
            // }
             __raw_writel(status , 0x424c00000+0x198); // clean irq status
             break;
        }
    }
    // printf("get dma irq status: 0x%x\n", status);

    return 0;
}

static int iommu_get_dc(int ddi)
{
    uint64_t DC_addr;

    DC_addr = get_dc(ddi);
    if (DC_addr == (uint64_t)-1) {
        printf("get dc failed\n");
        return -1;
    }
    printf("ddi[0x%x] addr=0x%llx\n", ddi, DC_addr);

    return 0;
}

static int iommu_get_pc(int ddi, int pdi)
{
    device_context_t DC;
    uint64_t DC_addr;
    uint64_t PC_addr;

    DC_addr = get_dc(ddi);
    if (DC_addr == (uint64_t)-1) {
        printf("get dc failed\n");
        return -1;
    }
    read_memory(DC_addr, 64, (char *)&DC);
    // con_dc(&DC);
    PC_addr = get_pc(&DC, pdi);
    // read_memory(PC_addr, sizeof(process_context_t), (char*)&PC);
    printf("ddi[0x%x] addr=0x%llx  pdi[%x] addr=0x%llx\n", \
            ddi, DC_addr, pdi, PC_addr);
    return 0;
}

static int iommu_translate_s1_s2(int ddi, int pdi, uint64_t va)
{
    device_context_t DC;
    process_context_t PC;
    uint64_t DC_addr;
    uint64_t PC_addr;
    uint64_t pa;

    DC_addr = get_dc(ddi);
    if (DC_addr == (uint64_t)-1) {
        printf("get dc failed\n");
        return -1;
    }
    read_memory(DC_addr, 64, (char *)&DC);
    PC_addr = get_pc(&DC, pdi);
    read_memory(PC_addr, 16, (char *)&PC);
    printf("ddi[0x%x] addr=0x%llx  pdi[%x] addr=0x%llx\n", \
            ddi, DC_addr, pdi, PC_addr);
    pa = translate_s1_s2(PC.fsc.iosatp, DC.iohgatp, va);
    printf("iommu_translate_s1_s2 va: %llx pa: %llx\n", va, pa);

    return 0;
}

int iommu_get_fq(void)
{
    fqb_t fqb;
    fqcsr_t fqcsr;
    fqh_t fqh;
    fqt_t fqt;
    fault_rec_t *fault_rec;
    int i;

    fqb.raw = read_register(FQB_OFFSET, 8);
    printf("fqb ppn:0x%llx\n", (uint64_t)fqb.ppn);
    fqcsr.raw = read_register(FQCSR_OFFSET, 4);
    printf("fqcsr.fqmf: %d fqcsr.fqof: %d\n", fqcsr.fqmf, fqcsr.fqof);
    fqh.raw = read_register(FQH_OFFSET, 4);
    fqt.raw = read_register(FQT_OFFSET, 4);
    printf("fqh:%d fqt:%d\n", fqh.raw, fqt.raw);

    fault_rec = (fault_rec_t *)(fqb.ppn * PAGESIZE);
    for (i = fqh.index; i < fqt.index; i++) {
        printf("FAULT:%d CAUSE: %d, PID: %d, DID: 0x%x, iotval: 0x%llx, iotval2 0x%llx\n",
            i - fqh.index, fault_rec[i].CAUSE, fault_rec[i].PID, 
            fault_rec[i].DID, fault_rec[i].iotval, fault_rec[i].iotval2);
    }
    fqh.index = i;
    write_register(FQH_OFFSET, 4, fqh.raw);

    return 0;
}

#include <stdlib.h>
static int do_iommu(int argc, char **argv)
{
    iommu_init();
    if (!strcmp(argv[1], "s1+s2")) {
        trl_s1_s2_case();
    } else if (!strcmp(argv[1], "dc")) {
        // iommu_get_dc(atoi(argv[2]));
        iommu_get_dc(0x80000);
    } else if (!strcmp(argv[1], "pc")) {
        // iommu_get_pc(strtoull(argv[2],0,0), strtoull(argv[3],0,0));
        iommu_get_pc(0x80000, 0);
    } else if (!strcmp(argv[1], "s1")) {
#if 0
        access_s1_case();
        // tlb_inv_all();
        dma_msg_send(src_data, src_data+16, 4);
#else
        trl_s1_case();
#endif    
    } else if (!strcmp(argv[1], "trl")) {
        iommu_translate_s1_s2(0x80000, 0, 0x100000);
    } else if (!strcmp(argv[1], "fq")) {
        iommu_get_fq();
    }
    return 0;
}


DEFINE_COMMAND(iommu, do_iommu, "IOMMU tests",
	"iommu\n"
);

