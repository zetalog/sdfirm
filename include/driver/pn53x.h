#ifndef __PN53X_H_INCLUDE_
#define __PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

#define PN53X_BUF_SIZE			264
#define PN53X_HEAD_SIZE			6
#define PN53X_TAIL_SIZE			2
#define PN53X_DATA_SIZE			(PN53X_BUF_SIZE-PN53X_HEAD_SIZE-PN53X_TAIL_SIZE)

#define PN53X_ACK			0x00FF
#define PN53X_NAK			0xFF00
#define pn53x_type(buf)			MAKEWORD((buf)[4], (buf)[3])

/* API used for PN53x USB devices */
boolean pn53x_poll_ready(void);
void pn53x_write_cmpl(scs_size_t nc);
void pn53x_read_cmpl(scs_size_t ne);
uint8_t pn53x_xchg_read(scs_off_t index);
void pn53x_xchg_write(scs_off_t index, uint8_t val);
void pn53x_ctrl_init(void);

#endif /* __PN53X_H_INCLUDE_ */
