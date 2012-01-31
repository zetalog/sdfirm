#ifndef __PN53X_H_INCLUDE_
#define __PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

#define PN53X_BUF_SIZE			264
#define PN53X_HEAD_SIZE			6
#define PN53X_TAIL_SIZE			2
#define PN53X_DATA_SIZE			(PN53X_BUF_SIZE-PN53X_HEAD_SIZE-PN53X_TAIL_SIZE)

/* API used for PN53x USB devices */
boolean pn53x_poll_ready(void);
uint8_t pn53x_read_byte(void);
void pn53x_write_byte(uint8_t val);
void pn53x_ctrl_init(void);

#endif /* __PN53X_H_INCLUDE_ */
