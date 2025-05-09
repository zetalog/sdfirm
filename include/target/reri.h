#ifndef __TARGET_RERI_H_INCLUDE__
#define __TARGET_RERI_H_INCLUDE__

#include <target/generic.h>
#include <asm/mach/reri.h>

void reri_drv_init(void);
int reri_drv_sync_hart_errs(u32 hart_id, u32 *pending_vectors);
#endif
