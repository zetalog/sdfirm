// Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
// Author: ved@rivosinc.com

#include "iommu.h"
#include "tables_api.h"
#include "test_app.h"

uint8_t read_memory(uint64_t addr, uint8_t size, char *data)
{
    memcpy(data, (void*)addr, size);
    return 0;
}

uint8_t read_memory_for_AMO(uint64_t addr, uint8_t size, char *data)
{
    return read_memory(addr, size, data);
}

uint8_t write_memory(char *data, uint64_t addr, uint32_t size)
{
    memcpy((void *)addr, data, size);
    return 0;
}    

void iommu_to_hb_do_global_observability_sync(uint8_t PR, uint8_t PW)
{
    // pr_go_requested = PR;
    // pw_go_requested = PW;
    return;
}

void send_msg_iommu_to_hb(ats_msg_t *msg)
{
    // if ( exp_msg.MSGCODE != msg->MSGCODE ||
    //      exp_msg.TAG != msg->TAG ||
    //      exp_msg.RID != msg->RID ||
    //      exp_msg.PV  != msg->PV ||
    //      exp_msg.PID != msg->PID ||
    //      exp_msg.PRIV != msg->PRIV ||
    //      exp_msg.EXEC_REQ != msg->EXEC_REQ ||
    //      exp_msg.DSV != msg->DSV ||
    //      exp_msg.DSEG != msg->DSEG ||
    //      exp_msg.PAYLOAD != msg->PAYLOAD )
    //     exp_msg_received = 0;
    // else
    //     exp_msg_received = 1;
    // message_received = 1;
    // memcpy(&rcvd_msg, msg, sizeof(ats_msg_t));
    return;
}
