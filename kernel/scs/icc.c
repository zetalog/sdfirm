#include <target/icc.h>
#include <target/state.h>

scs_intr_cb scd_notifier = NULL;
scs_cmpl_cb scd_complete = NULL;

#if NR_ICC_CARDS > 1
icc_driver_t *scd_drivers[NR_ICC_CARDS];
scd_t scd_nr_devs = 0;
struct scd_device scd_devices[NR_ICC_CARDS];

#define scd_dev_driver			scd_drivers[scd_id]
#define scd_dev_attrib			scd_devices[scd_id]

scd_t scd_id = INVALID_ICC_CARD;

scd_t scd_register_device(icc_driver_t *drv)
{
	scd_t id;

	BUG_ON(scd_nr_devs >= INVALID_ICC_CARD);
	id = scd_nr_devs;
	scd_drivers[id] = drv;
	scd_nr_devs++;
	return id;
}

void scd_dev_restore(scd_t id)
{
	scd_id = id;
	scd_dev_driver->select();
}

scd_t scd_dev_save(scd_t id)
{
	scd_t o_id = scd_id;
	scd_dev_restore(id);
	return o_id;
}
#else
struct scd_device scd_dev_attrib;
icc_driver_t *scd_dev_driver = NULL;

scd_t scd_register_device(icc_driver_t *drv)
{
	BUG_ON(scd_dev_driver);
	scd_dev_driver = drv;
	return 0;
}
#endif

static void scd_seq_onoff(boolean on)
{
	if (on)
		scd_dev_attrib.cmpl = scd_complete;
	else
		scd_dev_attrib.cmpl = NULL;
}

void scd_seq_complete(scs_err_t err)
{
	scd_set_error(err);
	if (scd_dev_attrib.cmpl)
		scd_dev_attrib.cmpl();
	scd_seq_onoff(false);
}

void scd_dev_set_state(uint8_t state)
{
	scd_dev_attrib.state = state;
	if (scd_notifier) scd_notifier();
}

uint8_t scd_dev_get_state(void)
{
	return scd_dev_attrib.state;
}

void scd_set_error(scs_err_t errno)
{
	scd_dev_attrib.error = errno;
}

scs_err_t scd_get_error(void)
{
	return scd_dev_attrib.error;
}

scs_err_t scd_power_on(void)
{
	scs_err_t err;
	BUG_ON(!scd_dev_driver || !scd_dev_driver->activate);
	err = scd_dev_driver->activate();
	if (err == SCS_ERR_PROGRESS)
		scd_seq_onoff(true);
	return err;
}

scs_err_t scd_power_off(void)
{
	scs_err_t err;
	BUG_ON(!scd_dev_driver || !scd_dev_driver->deactivate);
	err = scd_dev_driver->deactivate();
	if (err == SCS_ERR_PROGRESS)
		scd_seq_onoff(true);
	return err;
}

scs_err_t scd_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	BUG_ON(!scd_dev_driver || !scd_dev_driver->xchg_block);
	err = scd_dev_driver->xchg_block(nc, ne);
	if (err == SCS_ERR_PROGRESS)
		scd_seq_onoff(true);
	return err;
}

/* TODO: Transfer Available Bytes
 * allow scd_xchg_end/scd_xchg_stop to be called by card device driver
 * instead of calling driver xchg_avail here?
 */
scs_size_t scd_xchg_avail(void)
{
	BUG_ON(!scd_dev_driver || !scd_dev_driver->xchg_avail);
	return scd_dev_driver->xchg_avail();
}

scs_err_t scd_xchg_write(scs_off_t index, uint8_t byte)
{
	scs_err_t err;
	BUG_ON(!scd_dev_driver || !scd_dev_driver->xchg_write);
	err = scd_dev_driver->xchg_write(index, byte);
	BUG_ON(err == SCS_ERR_PROGRESS);
	return err;
}

uint8_t scd_xchg_read(scs_off_t index)
{
	BUG_ON(!scd_dev_driver || !scd_dev_driver->xchg_read);
	return scd_dev_driver->xchg_read(index);
}

void scd_register_handlers(scs_intr_cb notifier, scs_cmpl_cb completion)
{
	scd_notifier = notifier;
	scd_complete = completion;
}

void icc_init(void)
{
}
