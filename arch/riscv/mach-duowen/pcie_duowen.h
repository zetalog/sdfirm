#ifdef SDFIRM
#include <driver/pcie_designware.h>
#else
#include "pcie_designware.h"
#endif

#define X16         0
#define X8          1
#define X4_0        2
#define X4_1        3
#define SUBSYS      4

#define APB_PORT_X16                0x4
#define APB_PORT_X8                 0x5
#define APB_PORT_X4_0               0x6
#define APB_PORT_X4_1               0x7
#define APB_PORT_SUBSYS             0x8

#define AXI_DBI_PORT_X16            0x0
#define AXI_DBI_PORT_X8             0x1
#define AXI_DBI_PORT_X4_0           0x2
#define AXI_DBI_PORT_X4_1           0x3

#define RESET_CORE_X16              0x0
#define RESET_CORE_X8               0x4
#define RESET_CORE_X4_0             0x8
#define RESET_CORE_X4_1             0xC
#define RESET_PHY                   0x10
#define SUBSYS_CONTROL              0x14

#define LINK_MODE_4_4_4_4           0x0
#define LINK_MODE_8_4_0_4           0x1
#define LINK_MODE_8_8_0_0           0x2
#define LINK_MODE_16_0_0_0          0x3
#define LINK_MODE_INVALID           0xff

#define LINK_MODE_0                 LINK_MODE_4_4_4_4
#define LINK_MODE_1                 LINK_MODE_8_4_0_4
#define LINK_MODE_2                 LINK_MODE_8_8_0_0
#define LINK_MODE_3                 LINK_MODE_16_0_0_0

#ifndef DEFAULT_LINK_MODE
#define DEFAULT_LINK_MODE           LINK_MODE_16_0_0_0
#endif

#ifdef IPBENCH
#define CFG_APB_SUBSYS              0x0
#define CFG_APB_CORE_X16            0x0
#define CFG_APB_CORE_X8             0x0
#define CFG_APB_CORE_X4_0           0x0
#define CFG_APB_CORE_X4_1           0x0

#define CFG_APB_PHY_0               0x0
#define CFG_APB_PHY_1               0x0
#define CFG_APB_PHY_2               0x0
#define CFG_APB_PHY_3               0x0

#define CFG_AXI_CORE_X16            0x0
#define CFG_AXI_CORE_X8             0x0
#define CFG_AXI_CORE_X4_0           0x0
#define CFG_AXI_CORE_X4_1           0x0
#else
#define CFG_APB_SUBSYS              0xff09000000ULL
#define CFG_APB_CORE_X16            0xff09001000ULL
#define CFG_APB_CORE_X8             0xff09002000ULL
#define CFG_APB_CORE_X4_0           0xff09003000ULL
#define CFG_APB_CORE_X4_1           0xff09004000ULL

#define CFG_APB_PHY_0               0xff09100000ULL
#define CFG_APB_PHY_1               0xff09200000ULL
#define CFG_APB_PHY_2               0xff09300000ULL
#define CFG_APB_PHY_3               0xff09400000ULL

#define CFG_AXI_CORE_X16            0xff09110000ULL
#define CFG_AXI_CORE_X8             0xff09210000ULL
#define CFG_AXI_CORE_X4_0           0xff09310000ULL
#define CFG_AXI_CORE_X4_1           0xff09410000ULL
#endif

#define KB                          (1UL << 10)
#define MB                          (1UL << 20)
#define GB                          (1UL << 30)

#define PCIE_SUBSYS_ADDR_START      0x40000000000ULL
#define PCIE_CORE_X16_ADDR_START    PCIE_SUBSYS_ADDR_START
#define PCIE_CORE_X8_ADDR_START     PCIE_CORE_X16_ADDR_START + 512*GB
#define PCIE_CORE_X4_0_ADDR_START   PCIE_CORE_X8_ADDR_START + 512*GB
#define PCIE_CORE_X4_1_ADDR_START   PCIE_CORE_X4_0_ADDR_START + 512*GB
#define PCIE_SUBSYS_ADDR_END        PCIE_CORE_X4_1_ADDR_START + 512*GB

#define PCIE_CORE_CFG_SIZE      512*KB
#define PCIE_CORE_MEM_SIZE      512*GB
#define PCIE_CORE_X16_CFG0_START     PCIE_CORE_X8_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X16_CFG1_START     PCIE_CORE_X8_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X8_CFG0_START     PCIE_CORE_X4_0_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X8_CFG1_START     PCIE_CORE_X4_0_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_0_CFG0_START     PCIE_CORE_X4_1_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_0_CFG1_START     PCIE_CORE_X4_1_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_1_CFG0_START     PCIE_SUBSYS_ADDR_END - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_1_CFG1_START     PCIE_SUBSYS_ADDR_END - 2*PCIE_CORE_CFG_SIZE


#ifdef IPBENCH
void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);
#endif

void init_duowen_pcie_subsystem(void);


struct duowen_pcie_subsystem
{
    uint64_t cfg_apb[5];
    uint8_t link_mode;

#ifdef IPBENCH
    //uint8_t port_
#endif

    struct dw_pcie *controller;

    //uint8_t (* get_link_mode)(struct duowen_pcie_subsystem *pcie_subsystem);
    //uint32_t (* apb_read)(uint64_t *addr);
    //void (* apb_write)(uint64_t *addr, uint32_t val);
};
