#include <target/generic.h>
#include <target/console.h>
#include <stdio.h>
#include "tables_api.h"
#include "test_app.h"

#define CASE_NAME "S1+S2:"

static iommu_case_t case_s1_s2_data;
static int tlb_inv_all(void)
{
    uint32_t ddi, pdi;
    for(pdi = 0; pdi < case_s1_s2_data.pdi_max; pdi++) {
        for(ddi = 0; ddi < case_s1_s2_data.ddi_max; ddi++) {
            iotinval(VMA, 1, 0, 1, 0, 0xa5,0);
            // iotinval(VMA, 1, 0, 1, 0, ddi_pdi_2_scid(ddi, pdi),0);
            iodir(INVAL_DDT, 1, ddi, 0);
            iodir(INVAL_PDT, 1, ddi, pdi);
        }
    }
    return 0;
}

static int s1_s2_case_init(void)
{
    uint32_t ddi, pdi;
    if (case_s1_s2_data.inited) {
        config_ddtp(case_s1_s2_data.ddtp);
        tlb_inv_all();
        return 0;
    }
    // ppn_addr_set_tag("s1+s2 case ");
    // case_s1_s2_data.ddtp.ppn = get_free_ppn(1, PPN_TYPE_DDT, "ddtp.ppn") ;
    case_s1_s2_data.ddtp.ppn = get_free_ppn(1) ;
    if(!case_s1_s2_data.ddtp.ppn){
        con_err(CASE_NAME"ppn alloc failed");
        return -1;
    }
    case_s1_s2_data.ddtp.iommu_mode = DDT_1LVL;
    config_ddtp(case_s1_s2_data.ddtp);
    
    case_s1_s2_data.ddi_max = 1;
    case_s1_s2_data.pdi_max = 1;
    // case_s1_s2_data.data = (iommu_data_t*)(get_free_ppn(1, PPN_TYPE_OTHER, "iommu data")*PAGESIZE);
    case_s1_s2_data.data = (iommu_data_t*)(get_free_ppn(1)*PAGESIZE);
    if (!case_s1_s2_data.ddtp.ppn) {
        return -1;
    }

    for(pdi = 0; pdi < case_s1_s2_data.pdi_max; pdi ++) {
        for(ddi = 0; ddi < case_s1_s2_data.ddi_max; ddi ++) {
            int offset = pdi * case_s1_s2_data.ddi_max + ddi;
            // add_dc_pc_s1(ddi, pdi, &case_s1_s2_data.data[offset].iova, &case_s1_s2_data.data[offset].iopa);
            case_s1_s2_data.data[offset].ddi = 0xc0000;
            case_s1_s2_data.data[offset].pdi = 0;
            add_dc_pc_s1_s2(0xc0000, 0, &case_s1_s2_data.data[offset].iova,&case_s1_s2_data.data[offset].iopa);
        }
    }

    case_s1_s2_data.inited = 1;

    return 0;
}

static int tbu_access_init(void)
{
    START_TEST(CASE_NAME"tbu access case");
    tlb_inv_all();

    uint64_t iova = case_s1_s2_data.data[0].iova;
    uint64_t iopa = case_s1_s2_data.data[0].iopa;

    memset((uint8_t*)iopa, 0xa5, 4);
    memset((uint8_t*)(iopa+16), 0, 4);
    dma_msg_send((uint8_t *)iova, (uint8_t *)(iova + 16), 4);
    if (memcmp((uint8_t*)iopa, (uint8_t*)(iopa+16), 4) != 0) {
        iommu_get_fq();
        fail_if((1));
        return -1;
    }
    END_TEST();

    return 0;
}

int trl_s1_s2_case(void)
{
    fail_if(s1_s2_case_init() < 0);
    
    tbu_access_init();

    return 0;
}