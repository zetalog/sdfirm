#include <target/acpi.h>

struct acpi_event_table {
	uint8_t flags;
#define ACPI_EVENT_TABLE_GARBAGE	0x01
	uint8_t invokings;
	acpi_event_table_cb handler;
	void *context;
};

acpi_mutex_t acpi_gbl_event_mutex;
static struct acpi_event_table acpi_gbl_event_table = { 0, 0, NULL, NULL };

static void acpi_event_lock(void)
{
	(void)acpi_os_acquire_mutex(&acpi_gbl_event_mutex, ACPI_WAIT_FOREVER);
}

static void acpi_event_unlock(void)
{
	acpi_os_release_mutex(&acpi_gbl_event_mutex);
}

void acpi_event_table_notify(struct acpi_table_desc *table_desc,
			     acpi_ddb_t ddb, uint32_t event)
{
	acpi_event_lock();
	if (!acpi_gbl_event_table.handler ||
	    acpi_gbl_event_table.flags & ACPI_EVENT_TABLE_GARBAGE)
		goto err_lock;
	acpi_gbl_event_table.invokings++;
	acpi_event_unlock();

	/* Invoking callback without any locks held */
	acpi_gbl_event_table.handler(table_desc, ddb, event,
				     acpi_gbl_event_table.context);

	acpi_event_lock();
	acpi_gbl_event_table.invokings--;
err_lock:
	acpi_event_unlock();
}

acpi_status_t acpi_event_register_table_handler(acpi_event_table_cb handler,
						void *context)
{
	acpi_status_t status;

	if (!handler)
		return AE_BAD_PARAMETER;

	acpi_event_lock();

	if (acpi_gbl_event_table.handler) {
		status = AE_ALREADY_EXISTS;
		goto err_lock;
	}
	acpi_gbl_event_table.handler = handler;
	acpi_gbl_event_table.context = context;
	acpi_gbl_event_table.invokings = 0;
	acpi_gbl_event_table.flags = 0;

err_lock:
	acpi_event_unlock();

	return status;
}

void acpi_event_unregister_table_handler(acpi_event_table_cb handler)
{
	if (!handler)
		return;

	acpi_event_lock();

	if (handler != acpi_gbl_event_table.handler ||
	    acpi_gbl_event_table.flags & ACPI_EVENT_TABLE_GARBAGE)
		goto err_lock;
	acpi_gbl_event_table.invokings++;
	acpi_gbl_event_table.flags |= ACPI_EVENT_TABLE_GARBAGE;
	acpi_event_unlock();

	while (acpi_gbl_event_table.invokings != 1)
		acpi_os_sleep(10);

	acpi_event_lock();
	acpi_gbl_event_table.handler = NULL;
	acpi_gbl_event_table.context = NULL;
	acpi_gbl_event_table.invokings--;
	acpi_gbl_event_table.flags = 0;

err_lock:
	acpi_event_unlock();
}
