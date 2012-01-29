#include <target/usb_scd.h>

__near__ struct scd_cmd scd_cmds[NR_SCD_QUEUES];
__near__ struct scd_resp scd_resps[NR_SCD_QUEUES];
/* scd_qid_t scd_qid = INVALID_SCD_QUEUE; */

/*=========================================================================
 * bulk-out data
 *=======================================================================*/
void scd_CmdHeader_out(void)
{
	USBD_OUTB(scd_cmds[scd_qid].bMessageType);
	USBD_OUTL(scd_cmds[scd_qid].dwLength);
	USBD_OUTB(scd_cmds[scd_qid].bSlot);
	USBD_OUTB(scd_cmds[scd_qid].bSeq);
	USBD_OUTB(scd_cmds[scd_qid].abRFU[0]);
	USBD_OUTB(scd_cmds[scd_qid].abRFU[1]);
	USBD_OUTB(scd_cmds[scd_qid].abRFU[2]);
}

void __scd_CmdSuccess_out(void)
{
	scd_resps[scd_qid].bStatus = scd_slot_status();
	scd_resps[scd_qid].bError = 0;
}

void __scd_CmdFailure_out(uint8_t error, uint8_t status)
{
	scd_resps[scd_qid].bStatus = SCD_CMD_STATUS_FAIL |
				     status;
	scd_resps[scd_qid].bError = error;
	scd_resps[scd_qid].abRFU3 = 0;
	scd_resps[scd_qid].dwLength = 0;
}

void scd_SlotStatus_out(void)
{
	__scd_CmdSuccess_out();
	scd_resps[scd_qid].abRFU3 = 0;
	scd_resps[scd_qid].dwLength = 0;
}

void scd_SlotNotExist_cmp(void)
{
	__scd_CmdFailure_out(5, SCD_SLOT_STATUS_NOTPRESENT);
	scd_CmdResponse_cmp();
}

void scd_CmdOffset_cmp(uint8_t offset)
{
	scd_CmdFailure_out(offset);
	scd_CmdResponse_cmp();
}

void scd_SlotStatus_cmp(void)
{
	scd_SlotStatus_out();
	scd_CmdResponse_cmp();
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
void scd_RespHeader_in(scs_size_t length)
{
	USBD_INB(scd_resp_message());
	USBD_INL(length);
	USBD_INB(scd_cmds[scd_qid].bSlot);
	USBD_INB(scd_cmds[scd_qid].bSeq);
	USBD_INB(scd_resps[scd_qid].bStatus);
	USBD_INB(scd_resps[scd_qid].bError);
	USBD_INB(scd_resps[scd_qid].abRFU3);
}
