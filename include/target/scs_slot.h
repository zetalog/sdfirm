#ifndef __SCS_SLOT_H_INCLUDE__
#define __SCS_SLOT_H_INCLUDE__

#include <target/scs.h>

/* slot identifier */
typedef uint8_t scs_sid_t;

#ifdef CONFIG_SCS_MAX_SLOTS
#define NR_SCS_SLOTS		CONFIG_SCS_MAX_SLOTS
#else
#define NR_SCS_SLOTS		1
#endif
#define INVALID_SCS_SLOT	NR_SCS_SLOTS

/* Smart card standard slot devices includes
 * 1. ISO7816-4 card with contacts COS (native ICC)
 * 2. ISO7816-3 interface device (IFD)
 * 3. ISO14443-4 contactless interface device (PCD)
 */
struct scs_slot_driver {
	uint8_t (*get_error)(void);
	uint8_t (*status)(void);
	void (*select)(void);
	scs_err_t (*activate)(void);
	scs_err_t (*deactivate)(void);
	scs_err_t (*xchg_block)(scs_size_t nc, scs_size_t ne);
	scs_size_t (*xchg_avail)(void);
	scs_err_t (*xchg_write)(scs_off_t index, uint8_t byte);
	uint8_t (*xchg_read)(scs_off_t index);
};
__TEXT_TYPE__(struct scs_slot_driver, scs_slot_driver_t);

struct scs_slot {
	scs_cmpl_cb cmpl;
};

#define SCS_SLOT_STATUS_ACTIVE		0x00
#define SCS_SLOT_STATUS_INACTIVE	0x01
#define SCS_SLOT_STATUS_NOTPRESENT	0x02

#if NR_SCS_SLOTS > 1
scs_sid_t scs_slot_save(scs_sid_t sid);
void scs_slot_restore(scs_sid_t sid);
extern scs_sid_t scs_sid;
extern scs_sid_t scs_nr_slots;
#else
#define scs_slot_save(sid)		0
#define scs_slot_restore(sid)
#define scs_sid				0
#define scs_nr_slots			1
#endif
#define scs_slot_select(id)		scs_slot_restore(id)

/* Called by SCS slot implementations */
scs_sid_t scs_register_slot(scs_slot_driver_t *drv);
void scs_complete_slot(void);

/* Called by SCS slot users */
void scs_slot_register_completion(scs_cmpl_cb completion);

scs_err_t scs_slot_power_on(void);
scs_err_t scs_slot_power_off(void);

scs_err_t scs_slot_xchg_block(scs_size_t nc, scs_size_t ne);
scs_size_t scs_slot_xchg_avail(void);
scs_err_t scs_slot_xchg_write(scs_off_t index, uint8_t byte);
uint8_t scs_slot_xchg_read(scs_off_t index);

/* misc */
scs_err_t scs_get_slot_error(void);
uint8_t scs_get_slot_status(void);

#endif /* __SCS_SLOT_H_INCLUDE__ */
