// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com
#include "iommu.h"
ddt_cache_t ddt_cache[DDT_CACHE_SIZE];
pdt_cache_t pdt_cache[2];
tlb_t       tlb[2];
uint32_t    dc_lru_time = 0;
uint32_t    pc_lru_time = 0;
uint32_t    tlb_lru_time = 0;

// Cache a device context
void
cache_ioatc_dc(
    uint32_t device_id, device_context_t *DC) {
    uint8_t i, replace;
    uint32_t lru = 0xFFFFFFFF;
    
    for ( i = 0; i < DDT_CACHE_SIZE; i++ ) {
        if ( ddt_cache[i].valid == 0 ) {
            replace = i; 
            break;
        }
    }
    if ( i == DDT_CACHE_SIZE ) {
        for ( i = 0; i < DDT_CACHE_SIZE; i++ ) {
            if ( ddt_cache[i].lru < lru ) {
                replace = i;
            }
        }
    }
    ddt_cache[replace].DC = *DC;
    ddt_cache[replace].DID = device_id;
    ddt_cache[replace].valid = 1;
    return;
}

// Lookup IOATC for a device context
uint8_t
lookup_ioatc_dc(
    uint32_t device_id, device_context_t *DC) {
    uint8_t i;
    for ( i = 0; i < DDT_CACHE_SIZE; i++ ) {
        if ( ddt_cache[i].valid == 1 && ddt_cache[i].DID == device_id ) {
            *DC = ddt_cache[i].DC;
            ddt_cache[i].lru = dc_lru_time++;
            return IOATC_HIT;
        }
    }
    return IOATC_MISS;
}
// Cache a process context
void
cache_ioatc_pc(
    uint32_t device_id, uint32_t process_id, process_context_t *PC) {
    uint8_t i, replace = 0;
    uint32_t lru = 0xFFFFFFFF;
    
    for ( i = 0; i < PDT_CACHE_SIZE; i++ ) {
        if ( pdt_cache[i].valid == 0 ) {
            replace = i; 
            break;
        }
    }
    if ( i == PDT_CACHE_SIZE ) {
        for ( i = 0; i < PDT_CACHE_SIZE; i++ ) {
            if ( pdt_cache[i].lru < lru ) {
                replace = i;
            }
        }
    }
    pdt_cache[replace].PC = *PC;
    pdt_cache[replace].DID = device_id;
    pdt_cache[replace].PID = process_id;
    pdt_cache[replace].valid = 1;
    return;
}
// Lookup IOATC for a process context
uint8_t
lookup_ioatc_pc(
    uint32_t device_id, uint32_t process_id, process_context_t *PC) {
    uint8_t i;
    for ( i = 0; i < PDT_CACHE_SIZE; i++ ) {
        if ( pdt_cache[i].valid == 1 && 
             pdt_cache[i].DID == device_id &&
             pdt_cache[i].PID == process_id ) {
            *PC = pdt_cache[i].PC;
            pdt_cache[i].lru = pc_lru_time++;
            return 1;
        }
    }
    return 0;
}
// Cache a translation in the IOATC
void
cache_ioatc_iotlb(
    uint64_t vpn, uint8_t  GV, uint8_t  PSCV, uint32_t GSCID, uint32_t PSCID,
    rvos_pte_t *vs_pte, gpte_t *g_pte, uint64_t PPN, uint8_t S) {

    uint8_t i, replace = 0;
    uint32_t lru = 0xFFFFFFFF;

    for ( i = 0; i < TLB_SIZE; i++ ) {
        if ( tlb[i].valid == 0 ) {
            replace = i; 
            break;
        }
    }
    if ( i == TLB_SIZE ) {
        for ( i = 0; i < TLB_SIZE; i++ ) {
            if ( tlb[i].lru < lru ) {
                replace = i;
            }
        }
    }

    // Fill the tags
    tlb[replace].vpn   = vpn;
    tlb[replace].GV    = GV;
    tlb[replace].PSCV  = PSCV;
    tlb[replace].GSCID = GSCID;
    tlb[replace].PSCID = PSCID;
    // Fill VS stage attributes
    tlb[replace].VS_R  = vs_pte->R;
    tlb[replace].VS_W  = vs_pte->W;
    tlb[replace].VS_X  = vs_pte->X;
    tlb[replace].VS_D  = vs_pte->D;
    tlb[replace].U     = vs_pte->U;
    tlb[replace].G     = vs_pte->G;
    tlb[replace].PBMT  = vs_pte->PBMT;
    // Fill G stage attributes
    tlb[replace].G_R   = g_pte->R;
    tlb[replace].G_W   = g_pte->W;
    tlb[replace].G_X   = g_pte->X;
    tlb[replace].G_D   = g_pte->D;
    // PPN and size
    tlb[replace].PPN   = PPN;
    tlb[replace].S     = S;
    tlb[replace].valid = 1;
    return;
}

// Lookup a translation in the IOATC
uint8_t
lookup_ioatc_iotlb(
    uint64_t iova, uint8_t check_access_perms,
    uint8_t priv, uint8_t is_read, uint8_t is_write, uint8_t is_exec,
    uint8_t SUM, uint8_t PSCV, uint32_t PSCID, uint8_t GV, uint16_t GSCID, 
    uint32_t *cause, uint64_t *resp_pa, uint64_t *page_sz,
    rvos_pte_t *vs_pte, gpte_t *g_pte) {

    uint8_t i, hit;
    uint64_t vpn = iova / PAGESIZE;

    hit = 0xFF;
    for ( i = 0; i < TLB_SIZE; i++ ) {
        if ( tlb[i].valid == 1 && 
             tlb[i].GV == GV && tlb[i].GSCID == GSCID && 
             tlb[i].PSCV == PSCV && tlb[i].PSCID == PSCID &&
             match_address_range(vpn, tlb[i].vpn, tlb[i].S) ) {
            hit = i;
            break;
        }
    }
    if ( hit == 0xFF ) return IOATC_MISS;

    // Age the entries
    tlb[i].lru = tlb_lru_time++;

    // Check S/VS stage permissions
    if ( check_access_perms == 1 ) {
        if ( is_exec  && (tlb[hit].VS_X == 0) ) goto page_fault;
        if ( is_read  && (tlb[hit].VS_R == 0) ) goto page_fault;
        if ( is_write && (tlb[hit].VS_W == 0) ) goto page_fault;
    }
    if ( (priv == U_MODE) && (tlb[hit].U == 0) ) goto page_fault;
    if ( is_exec && (priv == S_MODE) && (tlb[hit].U == 1) ) goto page_fault;
    if ( (priv == S_MODE) && !is_exec && SUM == 0 && tlb[hit].U == 1 ) goto page_fault;

    // Check G stage permissions
    if ( (is_exec  && (tlb[hit].G_X == 0)) ||
         (is_read  && (tlb[hit].G_R == 0)) ||
         (is_write && (tlb[hit].G_W == 0)) ) {
        // More commonly, implementations contain address-translation caches that 
        // map guest virtual addresses directly to supervisor physical addresses, 
        // removing a level of indirection. 
        // If a G-stage permission fault is detected then such caches may not have
        // GPA to report in the iotval2. A common technique is to treat it as a 
        // TLB miss and trigger a page walk such that the GPA can be reported if 
        // the fault is actually detected again by the G-stage page tables
        tlb[hit].valid = 0;
        return IOATC_MISS;
    }
    // If memory access is a store and VS/S or G stage D bit is 0 then mark
    // TLB entry as invalid so it returns a miss to trigger a page walk
    if ( (tlb[hit].VS_D == 0 || tlb[hit].G_D == 0) && is_write == 1 ) {
        tlb[hit].valid = 0;
        return IOATC_MISS;
    }
    *page_sz = ((tlb[hit].S == 0) ? 1 : ((tlb[hit].PPN ^ (tlb[hit].PPN + 1)) + 1));
    *page_sz = *page_sz * PAGESIZE;
    *resp_pa = ((tlb[hit].PPN * PAGESIZE) & ~(*page_sz - 1)) | (iova & (*page_sz - 1));
    g_pte->R = tlb[hit].G_R;
    g_pte->W = tlb[hit].G_W;
    g_pte->X = tlb[hit].G_X;

    vs_pte->R = tlb[hit].VS_R;
    vs_pte->W = tlb[hit].VS_W;
    vs_pte->X = tlb[hit].VS_X;
    vs_pte->G = tlb[hit].G;
    vs_pte->U = tlb[hit].U;
    vs_pte->PBMT = tlb[hit].PBMT;
    return IOATC_HIT;

page_fault:
    if ( is_exec ) *cause = 12;      // Instruction page fault
    else if ( is_read ) *cause = 13; // Read page fault
    else *cause = 15;                // Write/AMO page fault
    return IOATC_FAULT;
}
