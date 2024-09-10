// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com

typedef struct {
    uint64_t iova;
    uint64_t iopa;
    uint32_t ddi;
    uint32_t pdi;
} iommu_data_t;

typedef struct {
    iommu_data_t *data;
    uint32_t ddi_max;
    uint32_t pdi_max;
    ddtp_t ddtp;
    int inited;
} iommu_case_t;

// Global functions
extern int config_ddtp(ddtp_t ddtp);
extern int8_t enable_cq(uint32_t nppn);
extern int8_t enable_fq(uint32_t nppn);
extern int8_t enable_disable_pq(uint32_t nppn, uint8_t enable_disable);
extern int8_t enable_iommu(uint8_t iommu_mode);
extern void iodir(uint8_t f3, uint8_t DV, uint32_t DID, uint32_t PID);
extern void iotinval( uint8_t f3, uint8_t GV, uint8_t AV, uint8_t PSCV, uint32_t GSCID, uint32_t PSCID, uint64_t address);
extern void iofence(uint8_t f3, uint8_t PR, uint8_t PW, uint8_t AV, uint8_t WSI_bit, uint64_t addr, uint32_t data);
extern void ats_command( uint8_t f3, uint8_t DSV, uint8_t PV, uint32_t PID, uint8_t DSEG, uint16_t RID, uint64_t payload);
extern void generic_any(command_t cmd);
extern void send_translation_request(uint32_t did, uint8_t pid_valid, uint32_t pid, uint8_t no_write,
             uint8_t exec_req, uint8_t priv_req, uint8_t is_cxl_dev, addr_type_t at, uint64_t iova,
             uint32_t length, uint8_t read_writeAMO,
             hb_to_iommu_req_t *req, iommu_to_hb_rsp_t *rsp);
extern int8_t check_rsp_and_faults(hb_to_iommu_req_t *req, iommu_to_hb_rsp_t *rsp, status_t status,
             uint16_t cause, uint64_t exp_iotval2);
extern int8_t check_msg_faults(uint16_t cause, uint8_t  exp_PV, uint32_t exp_PID, 
             uint8_t  exp_PRIV, uint32_t exp_DID, uint64_t exp_iotval);
extern int8_t check_exp_pq_rec(uint32_t DID, uint32_t PID, uint8_t PV, uint8_t PRIV, uint8_t EXEC,
             uint16_t reserved0, uint8_t reserved1, uint64_t PAYLOAD);
extern uint64_t get_free_gppn(uint64_t num_gppn, iohgatp_t iohgatp);
extern uint64_t add_device(uint32_t device_id, uint32_t gscid, uint8_t en_ats, uint8_t en_pri, uint8_t t2gpa, 
           uint8_t dtf, uint8_t prpr, 
           uint8_t gade, uint8_t sade, uint8_t dpe, uint8_t sbe, uint8_t sxl,
           uint8_t iohgatp_mode, uint8_t iosatp_mode, uint8_t pdt_mode,
           uint8_t msiptp_mode, uint8_t msiptp_pages, uint64_t msi_addr_mask, 
           uint64_t msi_addr_pattern);

extern int trl_s1_s2_case(void);
extern int trl_s1_case(void);
extern int dma_msg_send(void *src, void *dst, size_t size);
extern int iommu_get_fq(void);
extern int add_dc_pc_s1_s2(int ddi, int pdi, uint64_t *iova, uint64_t *iopa);
extern int add_dc_pc_s1(int ddi, int pdi, uint64_t *iova, uint64_t *spa);


#define START_TEST(__STR__)\
    printf("%-40s : ", __STR__);
#define fail_if(__COND__) if (__COND__) {printf("\n\x1B[31mFAIL. FILE: %s:%d\x1B[0m\n", __FILE__, __LINE__); return -1;}
#define END_TEST() {printf("\x1B[32mPASS\x1B[0m\n");}
