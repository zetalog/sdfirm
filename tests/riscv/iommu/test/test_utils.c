// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com

#include <target/generic.h>
#include <target/page.h>
#include "iommu.h"
#include "tables_api.h"
#include "test_app.h"

uint64_t
get_free_gppn(uint64_t num_gppn, iohgatp_t iohgatp) {
    static uint64_t free_gppn = 0x20000;

    free_gppn += (0x1000*num_gppn);
    return free_gppn; 
}

static int wait_ddtp_idle(void)
{
    ddtp_t ddtp;

    do {
        ddtp.raw = read_register(DDTP_OFFSET, 8);
    } while (ddtp.busy == 1);

    return 0;
}

int config_ddtp(ddtp_t ddtp)
{
    wait_ddtp_idle();
    write_register(DDTP_OFFSET, 8, 0);
    wait_ddtp_idle();
    write_register(DDTP_OFFSET, 8, ddtp.raw);
    wait_ddtp_idle();

    return 0;
}

int8_t enable_cq(uint32_t nppn)
{
    cqb_t cqb;
    cqcsr_t cqcsr;

    cqb.raw = 0;
    cqb.ppn = get_free_ppn(nppn);
    cqb.log2szm1 = 5;
    write_register(CQB_OFFSET, 8, cqb.raw);
    do {
        cqcsr.raw = read_register(CQCSR_OFFSET, 4);
    } while ( cqcsr.busy == 1 );
    cqcsr.raw = 0;
    cqcsr.cie = 1;
    cqcsr.cqen = 1;
    cqcsr.cqmf = 1;
    cqcsr.cmd_to = 1;
    cqcsr.cmd_ill = 1;
    cqcsr.fence_w_ip = 1;
    write_register(CQCSR_OFFSET, 4, cqcsr.raw);
    do {
        cqcsr.raw = read_register(CQCSR_OFFSET, 4);
    } while ( cqcsr.busy == 1 );
    if ( cqcsr.cqon != 1 ) {
        printf("CQ enable failed\n");
        return -1;
    }

    return 0;
}

int8_t enable_fq(uint32_t nppn)
{
    fqb_t fqb;
    fqcsr_t fqcsr;

    fqb.raw = 0;
    fqb.ppn = get_free_ppn(nppn);
    fqb.log2szm1 = 5;
    write_register(FQB_OFFSET, 8, fqb.raw);
    do {
        fqcsr.raw = read_register(FQCSR_OFFSET, 4);
    } while ( fqcsr.busy == 1 );
    fqcsr.raw = 0;
    fqcsr.fie = 1;
    fqcsr.fqen = 1;
    fqcsr.fqmf = 1;
    fqcsr.fqof = 1;
    write_register(FQCSR_OFFSET, 4, fqcsr.raw);
    do {
        fqcsr.raw = read_register(FQCSR_OFFSET, 4);
    } while ( fqcsr.busy == 1 );
    if ( fqcsr.fqon != 1 ) {
        printf("FQ enable failed\n");
        return -1;
    }

    return 0;
}

int8_t enable_disable_pq(uint32_t nppn, uint8_t enable_disable)
{
    pqb_t pqb;
    pqcsr_t pqcsr;

    if ( enable_disable == 1 ) {
        pqb.raw = 0;
        pqb.ppn = get_free_ppn(4);
        pqb.log2szm1 = 9;
        write_register(PQB_OFFSET, 8, pqb.raw);
    }
    do {
        pqcsr.raw = read_register(PQCSR_OFFSET, 4);
    } while ( pqcsr.busy == 1 );
    pqcsr.raw = 0;
    pqcsr.pie = 1;
    pqcsr.pqen = enable_disable;
    pqcsr.pqmf = 1;
    pqcsr.pqof = 1;
    write_register(PQCSR_OFFSET, 4, pqcsr.raw);
    do {
        pqcsr.raw = read_register(PQCSR_OFFSET, 4);
    } while ( pqcsr.busy == 1 );
    if ( pqcsr.pqon != 1 && enable_disable == 1) {
        printf("PQ enable failed\n");
        return -1;
    }
    if ( pqcsr.pqon == 1 && enable_disable == 0 ) {
        printf("PQ disable failed\n");
        return -1;
    }

    return 0;
}
#if 0
typedef struct {
    uint8_t msiptp_mode;
    uint8_t msiptp_pages;
    uint64_t msi_addr_mask;
    uint64_t msi_addr_pattern;    
} dc_msi_t;

static inline int tc_init(tc_t *tc)
{
    capabilities_t cap;

    cap.raw = read_register(CAPABILITIES_OFFSET, 8);

    tc->V = 1;
    tc->EN_ATS = 1;
    tc->EN_PRI = 1;
    tc->DPE = 1;
    tc->PDTV = 1; // enable pc

    if (cap.amo_hwad) {
        tc->GADE = 1;
        tc->SADE = 1;
    }

    return 0;
}

static inline int gpte_init(gpte_t *gpte)
{
    capabilities_t cap;

    cap.raw = read_register(CAPABILITIES_OFFSET, 8);

    gpte->raw = 0;
    gpte->V = 1;
    gpte->R = 1;
    gpte->W = 1;
    gpte->X = 0;
    gpte->U = 1;
    gpte->G = 0;
    gpte->A = 1;
    gpte->D = 0;
    gpte->PBMT = PMA;
    
    return 0;
}

// typedef struct {
//     uint8_t s1_enabled;
//     uint8_t s2_enabled;
//     uint32_t ddi;
//     uint32_t pdi;
//     uint32_t gscid;
//     uint32_t pscid;
//     device_context_t dc;
//     uint64_t dc_addr;
//     process_context_t pc;
//     uint64_t pc_addr;
// } iommu_case_t;

static int pc_with_gppn(device_context_t *dc, uint32_t pdi, uint32_t pscid)
{
    uint64_t pc_addr;
    process_context_t pc;
    pc_addr = get_pc(dc, pdi);
    if (pc_addr == (uint64_t)-1) {
        gpte_t gpte;
        memset(&PC, 0, 16);
        pc.fsc.iosatp.MODE = IOSATP_Sv39;
        pc.fsc.iosatp.PPN = get_free_gppn(1); // s1 page base
        pc.ta.V = 1;
        pc.ta.PSCID = pscid;
        pc.ta.ENS = 0;
        pc.ta.SUM = 0;
    
        gpte_init(&gpte);
        gpte.PPN = get_free_ppn(1);
        add_g_stage_pte(dc->iohgatp, (PAGESIZE * pc->fsc.iosatp.PPN), gpte, 0); //add pc base(gva) to s2 page
        pc_addr = add_process_context(dc, &pc, pdi);
printf("-------------ppn: %llx\n, pc addr: %llx PC.fsc.iosatp.MODE: %d\n", \
         pc.fsc.iosatp.PPN, pc_addr, pc.fsc.iosatp.MODE);
        fail_if( (read_memory(pc_addr, 16, (char *)&PC) != 0) );
printf("-------PC.fsc.iosatp.MODE: %d (addr: %llx)\n", pc.fsc.iosatp.MODE, pc_addr);
    }

    return pc_addr;
}

static int pte_init(pte_t *pte)
{
    pte->raw = 0;
    pte->V = 1;
    pte->R = 1;
    pte->W = 1;
    pte->X = 0;
    pte->U = 1;
    pte->G = 0;
    pte->A = 1;
    pte->D = 1;
    pte->PBMT = PMA;
    return 0;
}
uint64_t dc_with_pc_s1_s2(uint32_t device_id, uint32_t gscid, uint8_t iohgatp_mode, uint8_t iosatp_mode);
int dc_pc_s1_s2(void)
{
#if 0
    iommu_case_t i_case;
    pte_t pte;

    i_case.s1_enabled = i_case.s2_enabled = 1;
    i_case.ddi = 0x80000;
    i_case.pdi = 0;

    i_case.gscid = 0x1234;
    i_case.pscid = 0xa5;

    i_case.dc_addr = dc_with_pc_s1_s2(i_case.ddi, i_case.gscid, IOHGATP_Sv39x4, IOSATP_Sv39);
    read_memory(i_case.dc_addr, sizeof(i_case.dc), &i_case.dc);

    i_case.pc_addr = pc_with_gppn(&i_case.dc, i_case.pdi, i_case.pscid);

    iodir(INVAL_PDT, 1, i_case.ddi, i_case.pdi);


    pte_init(&pte);
    pte.PPN = get_free_ppn(1);

    // gva = 0x10000 + (page_offset ++) * PAGESIZE;
    // pte_addr = add_s_stage_pte(PC.fsc.iosatp, gva, pte, 0);
#endif
}

uint64_t dc_with_pc_s1_s2(uint32_t device_id, uint32_t gscid, uint8_t iohgatp_mode, uint8_t iosatp_mode)
{
    device_context_t dc;
    capabilities_t cap;
    gpte_t gpte;

    memset(&dc, 0, sizeof(dc));

    tc_init(&dc.tc);

    dc.iohgatp.PPN = get_free_ppn(4); //s2 page base
    dc.iohgatp.GSCID = gscid;
    dc.iohgatp.MODE = iohgatp_mode;

    dc.ta.raw = 0;

    dc.fsc.pdtp.MODE = iosatp_mode;
    dc.fsc.pdtp.PPN = get_free_gppn(1, dc.iohgatp);

    // dc.msiptp = dc.msi_addr_mask = dc.msi_addr_pattern = 0;
    // dc.reserved = 0;

    // gpte_init(&gpte);
    // gpte.PPN = get_free_ppn(1);
    // add_g_stage_pte(dc.iohgatp, (PAGESIZE * dc.fsc.pdtp.PPN), gpte, 0); //add pc base(gva) to s2 page

    // return add_dev_context(&dc, device_id);
}

uint64_t ddt_new(uint32_t device_id, uint32_t gscid, uint32_t pdtv, uint8_t iohgatp_mode, uint8_t iosatp_mode, dc_msi_t *msi)
{
    return 0;
}
#endif
uint64_t
add_device(uint32_t device_id, uint32_t gscid, uint8_t en_ats, uint8_t en_pri, uint8_t t2gpa, 
           uint8_t dtf, uint8_t prpr, 
           uint8_t gade, uint8_t sade, uint8_t dpe, uint8_t sbe, uint8_t sxl,
           uint8_t iohgatp_mode, uint8_t iosatp_mode, uint8_t pdt_mode,
           uint8_t msiptp_mode, uint8_t msiptp_pages, uint64_t msi_addr_mask, 
           uint64_t msi_addr_pattern) {
    device_context_t DC;
    memset(&DC, 0, sizeof(DC));

    DC.tc.V      = 1;
    DC.tc.EN_ATS = en_ats;
    DC.tc.EN_PRI = en_pri;
    DC.tc.T2GPA  = t2gpa;
    DC.tc.DTF    = dtf;
    DC.tc.PRPR   = prpr;
    DC.tc.GADE   = gade;
    DC.tc.SADE   = sade;
    DC.tc.DPE    = dpe;
    DC.tc.SBE    = sbe;
    DC.tc.SXL    = sxl;
    if ( iohgatp_mode != IOHGATP_Bare ) {
        DC.iohgatp.GSCID = gscid;
        DC.iohgatp.PPN = get_free_ppn(4);
        DC.iohgatp.PPN = ALIGN(DC.iohgatp.PPN*PAGESIZE, 4*PAGESIZE) / PAGESIZE;
    }
    DC.iohgatp.MODE = iohgatp_mode;
    if ( iosatp_mode != IOSATP_Bare ) {
        DC.tc.PDTV = 0;
        DC.fsc.iosatp.MODE = iosatp_mode;
        if ( DC.iohgatp.MODE != IOHGATP_Bare ) {
            gpte_t gpte;
            DC.fsc.iosatp.PPN = get_free_gppn(1, DC.iohgatp);
            gpte.raw = 0;
            gpte.V = 1;
            gpte.R = 1;
            gpte.W = 1;
            gpte.X = 0;
            gpte.U = 1;
            gpte.G = 0;
            gpte.A = 1;
            gpte.D = 0;
            gpte.PBMT = PMA;
            gpte.PPN = get_free_ppn(1);
            add_g_stage_pte(DC.iohgatp, (PAGESIZE * DC.fsc.iosatp.PPN), gpte, 0);
        } else {
            DC.fsc.iosatp.PPN = get_free_ppn(1);
        }
    }
    if ( pdt_mode != PDTP_Bare ) {
        DC.tc.PDTV = 1;
        DC.fsc.pdtp.MODE = pdt_mode;
        if ( DC.iohgatp.MODE != IOHGATP_Bare ) {
            gpte_t gpte;
            DC.fsc.pdtp.PPN = get_free_gppn(1, DC.iohgatp);
            gpte.raw = 0;
            gpte.V = 1;
            gpte.R = 1;
            gpte.W = 1;
            gpte.X = 0;
            gpte.U = 1;
            gpte.G = 0;
            gpte.A = 1;
            gpte.D = 0;
            gpte.PBMT = PMA;
            gpte.PPN = get_free_ppn(1);
            add_g_stage_pte(DC.iohgatp, (PAGESIZE * DC.fsc.pdtp.PPN), gpte, 0);
        } else {
            DC.fsc.pdtp.PPN = get_free_ppn(1);
        }
    }
    DC.msiptp.MODE = msiptp_mode;
    if ( msiptp_mode != MSIPTP_Off ) {
       DC.msiptp.PPN = get_free_ppn(msiptp_pages);
       DC.msi_addr_mask.mask = msi_addr_mask;
       DC.msi_addr_pattern.pattern = msi_addr_pattern;
    }
    return add_dev_context(&DC, device_id);
}

void iotinval(uint8_t f3, uint8_t GV, uint8_t AV, uint8_t PSCV, uint32_t GSCID, uint32_t PSCID, uint64_t address)
{
    command_t cmd;
    cqb_t cqb;
    cqt_t cqt;
    cmd.low = cmd.high = 0;
    cmd.iotinval.opcode = IOTINVAL;
    cmd.iotinval.func3 = f3;
    cmd.iotinval.gv = GV;
    cmd.iotinval.av = AV;
    cmd.iotinval.pscv = PSCV;
    cmd.iotinval.gscid = GSCID;
    cmd.iotinval.pscid = PSCID;
    cmd.iotinval.addr_63_12 = address / PAGESIZE;
    cqb.raw = read_register(CQB_OFFSET, 8);
    cqt.raw = read_register(CQT_OFFSET, 4);
    write_memory((char *)&cmd, ((cqb.ppn * PAGESIZE) | (cqt.index * 16)), 16);
    cqt.index++;
    write_register(CQT_OFFSET, 4, cqt.raw);
    return;
}

void ats_command(uint8_t f3, uint8_t DSV, uint8_t PV, uint32_t PID, uint8_t DSEG, uint16_t RID, uint64_t payload)
{
    command_t cmd;
    cqb_t cqb;
    cqt_t cqt;

    cmd.low = cmd.high = 0;
    cmd.ats.opcode = ATS;
    cmd.ats.func3 = f3;
    cmd.ats.rid = RID;
    cmd.ats.pv = PV;
    cmd.ats.pid = PID;
    cmd.ats.dsv = DSV;
    cmd.ats.dseg = DSEG;
    cmd.ats.payload = payload;

    cqb.raw = read_register(CQB_OFFSET, 8);
    cqt.raw = read_register(CQT_OFFSET, 4);
    write_memory((char *)&cmd, ((cqb.ppn * PAGESIZE) | (cqt.index * 16)), 16);

    cqt.index++;
    write_register(CQT_OFFSET, 4, cqt.raw);
    return;
}

void generic_any(command_t cmd)
{
    cqb_t cqb;
    cqt_t cqt;

    cqb.raw = read_register(CQB_OFFSET, 8);
    cqt.raw = read_register(CQT_OFFSET, 4);
    write_memory((char *)&cmd, ((cqb.ppn * PAGESIZE) | (cqt.index * 16)), 16);

    cqt.index++;
    write_register(CQT_OFFSET, 4, cqt.raw);
    return;
}
    
void iodir(uint8_t f3, uint8_t DV, uint32_t DID, uint32_t PID)
{
    command_t cmd;
    cqb_t cqb;
    cqt_t cqt;

    cmd.low = cmd.high = 0;
    cmd.iodir.opcode = IODIR;
    cmd.iodir.func3 = f3;
    cmd.iodir.dv = DV;
    cmd.iodir.did = DID;
    cmd.iodir.pid = PID;
    cqb.raw = read_register(CQB_OFFSET, 8);
    cqt.raw = read_register(CQT_OFFSET, 4);
    write_memory((char *)&cmd, ((cqb.ppn * PAGESIZE) | (cqt.index * 16)), 16);

    cqt.index++;
    write_register(CQT_OFFSET, 4, cqt.raw);
    return;
}

void iofence(uint8_t f3, uint8_t PR, uint8_t PW, uint8_t AV, uint8_t WSI_bit, uint64_t addr, uint32_t data)
{
    command_t cmd;
    cqb_t cqb;
    cqt_t cqt;

    cmd.low = cmd.high = 0;
    cmd.iofence.opcode = IOFENCE;
    cmd.iofence.func3 = f3;
    cmd.iofence.pr = PR;
    cmd.iofence.pw = PW;
    cmd.iofence.av = AV;
    cmd.iofence.wsi = WSI_bit;
    cmd.iofence.addr_63_2 = addr >> 2;
    cmd.iofence.data = data;
    cqb.raw = read_register(CQB_OFFSET, 8);
    cqt.raw = read_register(CQT_OFFSET, 4);
    write_memory((char *)&cmd, ((cqb.ppn * PAGESIZE) | (cqt.index * 16)), 16);

    cqt.index++;
    write_register(CQT_OFFSET, 4, cqt.raw);
    return;
}

uint64_t get_free_ppn(uint64_t num_ppn)
{
    uint64_t free_ppn;

    free_ppn = (uint64_t)page_alloc_pages(num_ppn);
    if (free_ppn)
        memset((uint8_t *)free_ppn, 0, num_ppn * PAGESIZE);
    return free_ppn / PAGESIZE; 
}
