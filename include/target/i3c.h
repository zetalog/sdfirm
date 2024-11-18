#ifndef __I3C_H_INCLUDE__
#define __I3C_H_INCLUDE__

#include <target/generic.h>

typedef uint8_t i3c_t;

#include <driver/i3c.h>

#if CONFIG_I3C_MAX_MASTERS > I3C_HW_MAX_MASTERS
#define NR_I3C_MASTERS		I3C_HW_MAX_MASTERS
#else
#define NR_I3C_MASTERS		CONFIG_I3C_MAX_MASTERS
#endif
#define INVALID_I3C		NR_I3C_MASTERS

struct i3c_device {
	i3c_io_cb iocb;
};
typedef struct i3c_device i3c_device_t;

struct i3c_master {
	uint16_t freq;
	uint8_t mode;

	uint8_t state;
	i3c_device_t *device;
};

#if NR_I3C_MASTERS > 1
extern struct i3c_master i3c_masters[NR_I3C_MASTERS];
extern i3c_t i3c_mid;

#define i3c_freq	i3c_masters[i3c_mid].freq
#define i3c_mode	i3c_masters[i3c_mid].mode
#define i3c_device	i3c_masters[i3c_mid].device
#define i3c_state	i3c_masters[i3c_mid].state

void i3c_master_select(i3c_t i3c);
i3c_t i3c_master_save(i3c_t i3c);
#define i3c_master_restore(i3c)		i3c_master_select(i3c)
#else
extern uint16_t i3c_freq;
extern uint8_t i3c_mode;
extern uint8_t i3c_state;
extern i3c_device_t *i3c_device;

#define i3c_mid				0
#define i3c_master_save(i3c)		0
#define i3c_master_restore(i3c)		do { } while (0)
#endif

#ifdef I3C_HW_FREQ
#define I3C_FREQ		I3C_HW_FREQ
#else
#define I3C_FREQ		300
#endif

/* Target address and mode suffix */
#define i3c_addr_mode(addr, mode)	(((addr) << 1) | (mode))
#define i3c_addr(addr_mode)		((addr_mode) >> 1)
#define i3c_mode(addr_mode)		((addr_mode) & 0x01)

#ifdef CONFIG_I3C_MASTER
void i3c_apply_frequency(void);
uint8_t i3c_master_write(i3c_addr_t slave, i3c_len_t txlen);
uint8_t i3c_master_read(i3c_addr_t slave, i3c_len_t rxlen);
void i3c_master_submit(i3c_addr_t slave,
		       i3c_len_t txlimit, i3c_len_t rxlimit);
void i3c_master_release(void);
void i3c_master_init(void);
#define i3c_set_frequency(khz) i3c_hw_set_frequency(khz)
#if CONFIG_I3C_MAX_MASTERS > 1
void i3c_master_select(i3c_t i3c);
extern i3c_t i3c_mid;
#else
#define i3c_master_select(i3c)			do { } while (0)
#define i3c_mid					0
#endif
#else
#define i3c_apply_frequency()
#define i3c_master_write(slave, txlen)		I3C_STATUS_ARBI
#define i3c_master_read(slave, rxlen)		I3C_STATUS_ARBI
#define i3c_set_frequency(khz)			do { } while (0)
#define i3c_master_release()			do { } while (0)
#define i3c_master_select(i3c)			do { } while (0)
#endif

void i3c_raise_event(uint8_t event);
void i3c_enter_state(uint8_t state);

/* called by device driver */
void i3c_register_device(i3c_device_t *dev);
uint8_t i3c_read_byte(void);
void i3c_write_byte(uint8_t byte);

void i3c_master_submit_async(i3c_addr_t slave, i3c_len_t txlen, i3c_len_t rxlen);
void i3c_master_commit(i3c_len_t len);
void i3c_master_abort(i3c_addr_t slave);
void i3c_master_start(void);
void i3c_master_stop(void);
void i3c_config_mode(uint8_t mode, bool freq);
void i3c_sync_status(void);

#endif /* __I3C_H_INCLUDE__ */
