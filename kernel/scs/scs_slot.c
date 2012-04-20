#include <target/scs_slot.h>
#include <target/bh.h>

scs_cmpl_cb scs_slot_completion = NULL;

#if NR_SCS_SLOTS > 1
scs_slot_driver_t *scs_slot_drivers[NR_SCS_SLOTS];
scs_sid_t scs_nr_slots = 0;
struct scs_slot scs_slots[NR_SCS_SLOTS];

#define scs_slot_driver			scs_slot_drivers[scs_sid]
#define scs_slot_attrib			scs_slots[scs_sid]

scs_sid_t scs_sid = INVALID_SCS_SLOT;

scs_sid_t scs_register_slot(scs_slot_driver_t *drv)
{
	scs_sid_t sid;

	BUG_ON(scs_nr_slots >= INVALID_SCS_SLOT);
	sid = scs_nr_slots;
	scs_slot_drivers[sid] = drv;
	scs_nr_slots++;
	return sid;
}

void scs_slot_restore(scs_sid_t sid)
{
	scs_sid = sid;
	scs_slot_driver->select();
}

scs_sid_t scs_slot_save(scs_sid_t sid)
{
	scs_sid_t ssid = scs_sid;
	scs_slot_restore(sid);
	return ssid;
}
#else
struct scs_slot scs_slot_attrib;
scs_slot_driver_t *scs_slot_driver = NULL;

scs_sid_t scs_register_slot(scs_slot_driver_t *drv)
{
	BUG_ON(scs_slot_driver);
	scs_slot_driver = drv;
	return 0;
}
#endif

static void scs_slot_seq_onoff(boolean on)
{
	if (on)
		scs_slot_attrib.cmpl = scs_slot_completion;
	else
		scs_slot_attrib.cmpl = NULL;
}

void scs_complete_slot(void)
{
	if (scs_slot_attrib.cmpl)
		scs_slot_attrib.cmpl();
	scs_slot_seq_onoff(false);
}

uint8_t scs_get_slot_status(void)
{
	BUG_ON(!scs_slot_driver || !scs_slot_driver->status);
	return scs_slot_driver->status();
}

scs_err_t scs_get_slot_error(void)
{
	BUG_ON(!scs_slot_driver || !scs_slot_driver->get_error);
	return scs_slot_driver->get_error();
}

scs_err_t scs_slot_power_on(void)
{
	scs_err_t err;
	BUG_ON(!scs_slot_driver || !scs_slot_driver->activate);
	err = scs_slot_driver->activate();
	if (err == SCS_ERR_PROGRESS)
		scs_slot_seq_onoff(true);
	return err;
}

scs_err_t scs_slot_power_off(void)
{
	scs_err_t err;
	BUG_ON(!scs_slot_driver || !scs_slot_driver->deactivate);
	err = scs_slot_driver->deactivate();
	if (err == SCS_ERR_PROGRESS)
		scs_slot_seq_onoff(true);
	return err;
}

scs_err_t scs_slot_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	BUG_ON(!scs_slot_driver || !scs_slot_driver->xchg_block);
	err = scs_slot_driver->xchg_block(nc, ne);
	if (err == SCS_ERR_PROGRESS)
		scs_slot_seq_onoff(true);
	return err;
}

/* TODO: Transfer Available Bytes
 * Allow scs_slot_xchg_end/scs_slot_xchg_stop to be called by card device
 * driver instead of calling driver xchg_avail here?
 */
scs_size_t scs_slot_xchg_avail(void)
{
	BUG_ON(!scs_slot_driver || !scs_slot_driver->xchg_avail);
	return scs_slot_driver->xchg_avail();
}

scs_err_t scs_slot_xchg_write(scs_off_t index, uint8_t byte)
{
	scs_err_t err;
	BUG_ON(!scs_slot_driver || !scs_slot_driver->xchg_write);
	err = scs_slot_driver->xchg_write(index, byte);
	BUG_ON(err == SCS_ERR_PROGRESS);
	return err;
}

uint8_t scs_slot_xchg_read(scs_off_t index)
{
	BUG_ON(!scs_slot_driver || !scs_slot_driver->xchg_read);
	return scs_slot_driver->xchg_read(index);
}

void scs_slot_register_completion(scs_cmpl_cb completion)
{
	scs_slot_completion = completion;
}

#ifdef CONFIG_IFD_SLOT
void ifd_slot_init(void);
#else
#define ifd_slot_init()
#endif

#ifdef CONFIG_COS_SLOT
void cos_slot_init(void);
#else
#define cos_slot_init()
#endif

#ifdef CONFIG_ACR122_SLOT
void acr122_slot_init(void);
#else
#define acr122_slot_init()
#endif

void scs_slot_init(void)
{
	ifd_slot_init();
	cos_slot_init();
	acr122_slot_init();
}
