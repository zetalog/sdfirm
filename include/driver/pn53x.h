#ifndef __PN53X_H_INCLUDE_
#define __PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

#ifdef CONFIG_PN53X_DEBUG
#define pn53x_debug(tag, val)		dbg_print((tag), (val))
#define PN53X_DUMP_USBD			true
#else
#define pn53x_debug(tag, val)
#define PN53X_DUMP_USBD			false
#endif

#define PN53X_BUF_SIZE			264
#define PN53X_HEAD_SIZE			6
#define PN53X_TAIL_SIZE			2
#define PN53X_DATA_SIZE			(PN53X_BUF_SIZE-PN53X_HEAD_SIZE-PN53X_TAIL_SIZE)

#define PN53X_ACK			0x00FF
#define PN53X_NAK			0xFF00
#define pn53x_type(buf)			MAKEWORD((buf)[4], (buf)[3])

boolean pn53x_hw_poll_ready(void);
void pn53x_hw_write_cmpl(scs_size_t nc);
void pn53x_hw_read_cmpl(scs_size_t ne);
uint8_t pn53x_hw_xchg_read(scs_off_t index);
void pn53x_hw_xchg_write(scs_off_t index, uint8_t val);
void pn53x_hw_ctrl_init(void);

/* API used for PN53x USB devices */
#define pn53x_poll_ready()			pn53x_hw_poll_ready()
#define pn53x_write_cmpl(nc)			pn53x_hw_write_cmpl(nc)
#define pn53x_read_cmpl(ne)			pn53x_hw_read_cmpl(ne)
#define pn53x_xchg_read(index)			pn53x_hw_xchg_read(index)
#define pn53x_xchg_write(index, val)		pn53x_hw_xchg_write(index, val)
#define pn53x_ctrl_init()			pn53x_hw_ctrl_init()

#endif /* __PN53X_H_INCLUDE_ */
