#include <target/acpi.h>
#include <target/spinlock.h>
#include <target/semaphore.h>

acpi_status_t acpi_os_create_lock(acpi_spinlock_t *phandle)
{
	spinlock_t *lock;

	lock = acpi_os_allocate(sizeof(*lock));
	if (lock) {
		spin_lock_init(lock);
		*phandle = lock;
	}
	return lock ? AE_OK : AE_NO_MEMORY;
}

void acpi_os_delete_lock(acpi_spinlock_t handle)
{
	if (handle)
		acpi_os_free(handle);
}

acpi_cpuflags_t acpi_os_acquire_lock(acpi_spinlock_t handle)
{
	spin_lock(handle);
	return 0;
}

void acpi_os_release_lock(acpi_spinlock_t handle, acpi_cpuflags_t flags)
{
	spin_unlock(handle);
}

acpi_status_t acpi_os_create_semaphore(uint32_t max_units,
				       uint32_t initial_units,
				       acpi_handle_t *out_handle)
{
	semaphore_t *lock;

	lock = acpi_os_allocate(sizeof(*lock));
	if (lock) {
		sem_init(lock, max_units, initial_units);
		*out_handle = lock;
	}
	return lock ? AE_OK : AE_NO_MEMORY;
}

acpi_status_t acpi_os_delete_semaphore(acpi_handle_t handle)
{
	if (handle)
		acpi_os_free(handle);
	return AE_OK;
}

acpi_status_t acpi_os_wait_semaphore(acpi_handle_t handle,
				     uint32_t units,
				     uint16_t timeout)
{
	int ret;

	ret = sem_wait(handle, units, timeout);
	return ret ? AE_TIME : AE_OK;
}

acpi_status_t acpi_os_signal_semaphore(acpi_handle_t handle,
				       uint32_t units)
{
	int ret;

	ret = sem_signal(handle, units);
	return ret ? AE_LIMIT : AE_OK;
}
