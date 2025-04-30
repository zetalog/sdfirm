#ifndef __I3C_H_INCLUDE__
#define __I3C_H_INCLUDE__

#include <target/generic.h>
#include <target/clk.h>

typedef uint8_t i3c_t;

#define I3C_ADDR_LEN		1

#define I3C_ADDR_BROADCAST	0x7E
#define I3C_ADDR_HOTJOIN	0x02

#define I3C_BCR			0x00
#define I3C_DCR			0x01
#define I3C_LVR			0x02

#define I3C_BCR_MaxDataSpeedLimitation	_BV(0)
#define I3C_BCR_IBIRequestCapability	_BV(1)
#define I3C_BCR_IBIPayload		_BV(2)
#define I3C_BCR_OfflineCapable		_BV(3)
#define I3C_BCR_BridgeIdentifier	_BV(4)
#define I3C_BCR_HDRCapable		_BV(5)
#define I3C_BCR_DeviceRole_OFFSET	6
#define I3C_BCR_DeviceRole_MASK		REG_2BIT_MASK
#define I3C_BCR_DeviceRole(value)	_GET_FV(I3C_BCR_DeviceRole, value)

#define I3C_LVR_I2CModeIdentifier	_BV(4)
#define I3C_LVR_LegacyI2COnly_OFFSET	5
#define I3C_LVR_LegacyI2COnly_MASK	REG_3BIT_MASK
#define I3C_LVR_LegacyI2COnly(value)	_GET_FV(I3C_LVR_LegacyI2COnly, value)

#define I3C_RNW				_BV(0)
#define I3C_ADDR(addr, rnw)		((rnw) ? I3C_RNW : 0 | ((addr) << 1))

/* 5.1.2.2 I3C Address Header:
 * Using the I3C Arbitrable Address Header, I3C Slaves may transmit any of
 * three requests to the I3C Master:
 * 1. An In-Band Interrupt
 * 2. A Secondary Master request
 * 3. A Hot-Join request
 */
#define I3C_SLV_IPI(addr)		I3C_ADDR(addr, 1)
#define I3C_SLV_SM(addr)		I3C_ADDR(addr, 0)
#define I3C_SLV_HJ(rnw)			I3C_ADDR(I3C_ADDR_HOTJOIN, rnw)

/* Common Command Codes (CCC) */
#define I3C_CCC_BROADCAST	0
#define I3C_CCC_DIRECT		_BV(7)
#define I3C_CCC_ENEC(d)		(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | 0x00)
#define I3C_CCC_DISEC(d)	(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | 0x01)
#define I3C_CCC_ENTAS(d, n)	(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | (0x02 + (n)))
#define I3C_CCC_RSTDAA(d)	(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | 0x06)
#define I3C_CCC_SETMWL(d)	(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | 0x09)
#define I3C_CCC_SETMRL(d)	(((d) ? I3C_CCC_DIRECT : I3C_CCC_BROADCAST) | 0x0A)
#define I3C_CCC_SETXTIME(d)	(((d) ? (I3C_CCC_DIRECT | 0x18) : (I3C_CCC_BROADCAST | 0x28))
/* Broadcast only */
#define I3C_CCC_ENTDAA		(I3C_CCC_BROADCAST | 0x07)
#define I3C_CCC_DEFSLVS		(I3C_CCC_BROADCAST | 0x08)
#define I3C_CCC_ENTTM		(I3C_CCC_BROADCAST | 0x0B)
#define I3C_CCC_SETBUSCON	(I3C_CCC_BROADCAST | 0x0C)
#define I3C_CCC_ENTHDR(n)	(I3C_CCC_BROADCAST | (0x20 + (n)))
#define I3C_CCC_SETAASA		(I3C_CCC_BROADCAST | 0x29)
#define I3C_CCC_SETHID		(I3C_CCC_BROADCAST | 0x61)
#define I3C_CCC_DEVCTRL		(I3C_CCC_BROADCAST | 0x62)
/* Direct only */
#define I3C_CCC_SETDASA		(I3C_CCC_DIRECT | 0x07)
#define I3C_CCC_SETNEWDA	(I3C_CCC_DIRECT | 0x08)
#define I3C_CCC_GETMWL		(I3C_CCC_DIRECT | 0x0B)
#define I3C_CCC_GETMRL		(I3C_CCC_DIRECT | 0x0C)
#define I3C_CCC_GETPID		(I3C_CCC_DIRECT | 0x0D)
#define I3C_CCC_GETBCR		(I3C_CCC_DIRECT | 0x0E)
#define I3C_CCC_GETDCR		(I3C_CCC_DIRECT | 0x0F)
#define I3C_CCC_GETSTATUS	(I3C_CCC_DIRECT | 0x10)
#define I3C_CCC_GETACCMST	(I3C_CCC_DIRECT | 0x11)
#define I3C_CCC_SETBRGTGT	(I3C_CCC_DIRECT | 0x13)
#define I3C_CCC_GETMXDS		(I3C_CCC_DIRECT | 0x14)
#define I3C_CCC_GETHDRCAP	(I3C_CCC_DIRECT | 0x15)
#define I3C_CCC_GETXTIME	(I3C_CCC_DIRECT | 0x19)
#define I3C_CCC_DEVCAPS		(I3C_CCC_DIRECT | 0x60)

/* I3C_CCC_ENEC/I3C_CCC_DISEC: 8-bits */
#define I3C_CCC_EC_INT		_BV(0)
#define I3C_CCC_EC_MR		_BV(1)
#define I3C_CCC_EC_HJ		_BV(3)
#define I3C_CCC_EC_ALL		(I3C_CCC_EC_INT | I3C_CCC_EC_MR | I3C_CCC_EC_HJ)

/* ENEC/DISEC */
struct i3c_ccc_ec {
	uint8_t event;
} __packed;

/* MWL */
struct i3c_ccc_mwl {
	uint16_t len;
} __packed;

/* MRL */
struct i3c_ccc_mrl {
	uint16_t read_len;
	uint8_t ibi_len;
} __packed;

/* Dynamic address: DA */
struct i3c_ccc_da {
	uint8_t addr;
} __packed;

struct i3c_ccc_pid {
	uint8_t pid[6];
} __packed;

/* BCR */
struct i3c_ccc_bcr {
	uint8_t bcr;
} __packed;

/* DCR */
struct i3c_ccc_dcr {
	uint8_t dcr;
} __packed;

struct i3c_ccc_status {
	uint16_t status;
} __packed;

struct i3c_ccc_dev {
	uint8_t da;
	union {
		uint8_t dcr;
		uint8_t lvr;
	};
	uint8_t bcr;
	uint8_t sa;
} __packed;

struct i3c_ccc_defslvs {
	uint8_t count;
	struct i3c_ccc_dev master;
	struct i3c_ccc_dev slaves[];
} __packed;

struct i3c_ccc_dest {
	uint8_t addr;
	uint16_t len;
	void *data;
};

struct i3c_ccc {
	uint8_t rnw;
	uint8_t id;
	uint8_t ndests;
	struct i3c_ccc_dest *dests;
	uint8_t err;
#define I3C_ERROR_UNKNOWN		0
#define I3C_ERROR_M0			1
#define I3C_ERROR_M1			2
#define I3C_ERROR_M2			3
};

typedef uint8_t i3c_bus_t;

#define I3C_SCL_RATE_I3C_MAX		12900000
#define I3C_SCL_RATE_I3C_TYP		12500000
#define I3C_SCL_RATE_I2C_FAST_PLUS	1000000
#define I3C_SCL_RATE_I2C_FAST		400000
#define I3C_I3C_TLOW_OD_MIN_NS		200

#define I3C_BUS_PURE		0x00
#define I3C_BUS_MIXED_FAST	0x04
#define I3C_BUS_MIXED_LIMITED	0x08
#define I3C_BUS_MIXED_SLOW	0x0C
#define I3C_MODE_MASK		0x0C
#define I3C_MODE_SLAVE		0x00
#define I3C_MODE_MASTER		0x02
#define I3C_BUS_MASK		0x02
#define I3C_MODE_TX		0x00
#define I3C_MODE_RX		0x01
#define I3C_DIR_MASK		0x01

#define I3C_MODE_SLAVE_TX	(I3C_MODE_SLAVE | I3C_MODE_TX)
#define I3C_MODE_MASTER_TX	(I3C_MODE_MASTER | I3C_MODE_TX)
#define I3C_MODE_SLAVE_RX	(I3C_MODE_SLAVE | I3C_MODE_RX)
#define I3C_MODE_MASTER_RX	(I3C_MODE_MASTER | I3C_MODE_RX)

#define I3C_MODE(x)		(x & I3C_MODE_MASK)
#define I3C_BUS(x)		(x & I3C_BUS_MASK)
#define I3C_DIR(x)		(x & I3C_DIR_MASK)
#define I3C_BUS_DIR(x)		(x & (I3C_BUS_MASK | I3C_DIR_MASK))
#define I3C_SET_BUS(x, b)	(x &= ~I3C_BUS_MASK, x |= b)
#define I3C_SET_DIR(x, d)	(x &= ~I3C_DIR_MASK, x |= d)

#define I3C_STATUS_IDLE		0x00
#define I3C_STATUS_START	0x01
#define I3C_STATUS_ACK		0x02
#define I3C_STATUS_NACK		0x03
#define I3C_STATUS_STOP		0x04

#define I3C_STATE_IDLE			0x00
#define I3C_STATE_START			0x01
#define I3C_STATE_STOP			0x02
#define I3C_STATE_WAIT			0x03
#define I3C_STATE_WRITE			0x04
#define I3C_STATE_READ			0x05

/* Dynamic Address Assignment (DAA) FSM */
#define I3C_STATE_RSTDAA		0x06
#define I3C_STATE_DISEC			0x07
/* SET_STATUS */
#define I3C_STATE_SETDASA		0x08
#define I3C_STATE_GETPID		0x08
#define I3C_STATE_BCAST_ENTDAA		0x09
#define I3C_STATE_READ_PID		0x0A
#define I3C_STATE_READ_BCR		0x0B
#define I3C_STATE_READ_DCR		0x0C

#define I3C_EVENT_IDLE		_BV(I3C_STATUS_IDLE)
#define I3C_EVENT_START		_BV(I3C_STATUS_START)
#define I3C_EVENT_PAUSE		_BV(I3C_STATUS_ACK)
#define I3C_EVENT_ABORT		_BV(I3C_STATUS_NACK)
#define I3C_EVENT_STOP		_BV(I3C_STATUS_STOP)

#include <driver/i3c.h>

#define i3c_addr_mode(addr, mode)	(((addr) << 1) | (mode))
#define i3c_addr(addr_mode)		((addr_mode) >> 1)
#define i3c_mode(addr_mode)		((addr_mode) & 0x01)

#if CONFIG_I3C_MAX_MASTERS > I3C_HW_MAX_MASTERS
#define NR_I3C_MASTERS		I3C_HW_MAX_MASTERS
#else
#define NR_I3C_MASTERS		CONFIG_I3C_MAX_MASTERS
#endif
#define INVALID_I3C		NR_I3C_MASTERS

#define I3C_MAX_ADDR		0x7F
#define I3C_NR_ADDRS		(I3C_MAX_ADDR + 1)
#define INVALID_I3C_ADDR	I3C_NR_ADDRS

typedef uint16_t i3c_event_t;

struct i3c_device {
	i3c_io_cb iocb;
};
typedef struct i3c_device i3c_device_t;

struct i3c_master {
	uint16_t freq;
	uint8_t bus;

	i3c_addr_t dev_addr;

	i3c_len_t rxsubmit;
	i3c_len_t txsubmit;
	i3c_len_t limit;
	i3c_len_t current;
	i3c_len_t commit;

	clk_freq_t i3c_rate;
	clk_freq_t i2c_rate;

	/* Current communication device */
	i3c_device_t *device;

	/* State machine variables */
	uint8_t state;
	i3c_event_t event;
	uint8_t status;

	/* Address slots */
	DECLARE_BITMAP(addr_slot, I3C_NR_ADDRS);
	DECLARE_BITMAP(addr_i2c, I3C_NR_ADDRS);

	/* I3C registers */
	uint8_t i3c_data[2];
	uint8_t i3c_len;
};

#if NR_I3C_MASTERS > 1
extern struct i3c_master i3c_masters[NR_I3C_MASTERS];
extern i3c_t i3c_mid;

#define i3c_freq	i3c_masters[i3c_mid].freq
#define i3c_bus		i3c_masters[i3c_mid].bus
#define i3c_dev_addr	i3c_masters[i3c_mid].dev_addr
#define i3c_txsubmit	i3c_masters[i3c_mid].txsubmit
#define i3c_rxsubmit	i3c_masters[i3c_mid].rxsubmit
#define i3c_limit	i3c_masters[i3c_mid].limit
#define i3c_current	i3c_masters[i3c_mid].current
#define i3c_commit	i3c_masters[i3c_mid].commit
#define i3c_state	i3c_masters[i3c_mid].state
#define i3c_event	i3c_masters[i3c_mid].event
#define i3c_status	i3c_masters[i3c_mid].status
#define i3c_device	i3c_masters[i3c_mid].device
#define i3c_addr_slot	i3c_masters[i3c_mid].addr_slot
#define i3c_addr_i2c	i3c_masters[i3c_mid].addr_i2c
#define i3c_scl_rate	i3c_masters[i3c_mid].i3c_rate
#define i2c_scl_rate	i3c_masters[i3c_mid].i2c_rate
#define i2c_payld_data	i3c_masters[i3c_mid].i2c_data
#define i2c_payld_len	i3c_masters[i3c_mid].i2c_len

void i3c_master_select(i3c_t i3c);
i3c_t i3c_master_save(i3c_t i3c);
#define i3c_master_restore(i3c)		i3c_master_select(i3c)
#else
extern uint16_t i3c_freq;
extern uint8_t i3c_mode;
extern i3c_addr_t i3c_dev_addr;
extern i3c_len_t i3c_txsubmit;
extern i3c_len_t i3c_rxsubmit;
extern i3c_len_t i3c_limit;
extern i3c_len_t i3c_current;
extern i3c_len_t i3c_commit;
extern uint8_t i3c_state;
extern i3c_event_t i3c_event;
extern uint8_t i3c_status;
extern i3c_device_t *i3c_device;
extern DECLARE_BITMAP(i3c_addr_slot, I3C_NR_ADDRS);
extern DECLARE_BITMAP(i3c_addr_i2c, I3C_NR_ADDRS);
extern clk_freq_t i3c_scl_rate;
extern clk_freq_t i2c_scl_rate;
extern uint8_t i3c_payld_data[2];
extern uint8_t i3c_payld_len;

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

uint8_t i3c_i3c_mode(void);
uint8_t i3c_bus_mode(void);
uint8_t i3c_dir_mode(void);
uint8_t i3c_bus_dir_mode(void);

void i3c_master_submit_async(i3c_addr_t slave, i3c_len_t txlen, i3c_len_t rxlen);
void i3c_master_commit(i3c_len_t len);
void i3c_master_abort(i3c_addr_t slave);
void i3c_master_start(void);
void i3c_master_stop(void);
void i3c_config_mode(uint8_t mode, bool freq);
void i3c_sync_status(void);
void i3c_bus_set_status(uint8_t status);

i3c_addr_t i3c_get_free_addr(i3c_addr_t start_addr);

#endif /* __I3C_H_INCLUDE__ */
