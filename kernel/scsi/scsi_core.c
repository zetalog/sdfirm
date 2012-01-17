#include <target/scsi.h>

void scsi_init(void)
{
	scsi_target_init();
	scsi_sbc_init();
}
