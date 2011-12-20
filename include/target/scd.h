#ifndef __SCD_H_INCLUDE__
#define __SCD_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

typedef uint8_t scd_t;
typedef uint16_t scd_size_t;
typedef uint16_t scd_off_t;
typedef uint8_t scd_err_t;

typedef void (*scd_intr_cb)(void);
typedef void (*scd_cmpl_cb)(void);

#ifdef CONFIG_SCD_MAX_DEVICES
#define NR_SCD_DEVICES		CONFIG_SCD_MAX_DEVICES
#else
#define NR_SCD_DEVICES		1
#endif
#define INVALID_SCD_UNIT	NR_SCD_DEVICES

/* Smart card devices includes
 * 1. ISO7816-3 interface device (IFD)
 * 2. ISO7816-4 card with contacts COS (ICC)
 * 3. ISO14443-4 contactless interface device (MIFARE)
 */

struct scd_driver {
	void (*select)(void);
	scd_err_t (*activate)(void);
	scd_err_t (*deactivate)(void);
	scd_err_t (*xchg_block)(scd_size_t nc, scd_size_t ne);
	scd_size_t (*xchg_avail)(void);
	scd_err_t (*xchg_write)(scd_off_t index, uint8_t byte);
	uint8_t (*xchg_read)(scd_off_t index);
};
__TEXT_TYPE__(struct scd_driver, scd_driver_t);

struct scd_device {
	uint8_t error;
	uint8_t state;
	scd_cmpl_cb cmpl;
};

#define SCD_ERR_PROGRESS		0x12
#define SCD_ERR_OVERRUN			0x0E
#define SCD_ERR_TIMEOUT			0x03
#define SCD_ERR_NOTPRESENT		0x02
#define SCD_ERR_HW_ERROR		0x01
#define SCD_ERR_SUCCESS			0x00

#define SCD_DEV_STATE_NOTPRESENT	0x00
#define SCD_DEV_STATE_PRESENT		0x01
#define SCD_DEV_STATE_ACTIVE		0x02
#define SCD_DEV_STATE_HWERROR		0x03

#if NR_SCD_DEVICES > 1
void scd_dev_restore(scd_t id);
scd_t scd_dev_save(scd_t id);
extern scd_t scd_id;
extern scd_t scd_nr_devs;
#else
#define scd_dev_save(id)		0
#define scd_dev_restore(id)
#define scd_id				0
#define scd_nr_devs			1
#endif
#define scd_dev_select(id)		scd_dev_restore(id)

/* used by SCD server */
scd_t scd_register_device(scd_driver_t *drv);
void scd_seq_complete(scd_err_t err);

/* used by SCD client */
void scd_register_handlers(scd_intr_cb notifier, scd_cmpl_cb completion);

scd_err_t scd_power_on(void);
scd_err_t scd_power_off(void);

scd_err_t scd_xchg_block(scd_size_t nc, scd_size_t ne);
scd_size_t scd_xchg_avail(void);
scd_err_t scd_xchg_write(scd_off_t index, uint8_t byte);
uint8_t scd_xchg_read(scd_off_t index);

/* misc */
void scd_set_error(scd_err_t errno);
scd_err_t scd_get_error(void);
uint8_t scd_dev_get_state(void);
void scd_dev_set_state(uint8_t status);

#endif
