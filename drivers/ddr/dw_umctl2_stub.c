#include <target/ddr.h>
#include "dwc_ddr5_sw_cmd.h"
#include "dwc_ddr5_ucode.h"
#include "dwc_ddr54_ctrl.h"

#ifndef CONFIG_ARCH_HAS_DW_UMCTL2
ddr_ctrl_regs_t g_ctrl_regs;
SubsysHdlr_t hdlr;
#endif
