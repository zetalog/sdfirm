#ifndef __PCD_PN53X_H_INCLUDE_
#define __PCD_PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/pcd.h>

#ifndef ARCH_HAVE_PCD
#define ARCH_HAVE_PCD			1
#else
#error "Multiple proximity coupling devices defined"
#endif

#define PN53X_BUF_SIZE			264

/* API used for PN53x USB devices */
scs_err_t pn53x_activate(void);
void pn53x_deactivate(void);
scs_err_t pn53x_xchg_block(scs_size_t nc, scs_size_t ne);
scs_size_t pn53x_xchg_avail(void);
scs_err_t pn53x_xchg_write(scs_off_t index, uint8_t byte);
uint8_t pn53x_xchg_read(scs_off_t index);

#endif /* __PCD_PN53X_H_INCLUDE_ */
