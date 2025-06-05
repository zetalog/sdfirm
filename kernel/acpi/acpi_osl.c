#include <target/acpi.h>


acpi_status_t acpi_os_create_lock(acpi_spinlock_t *phandle)
{
}

void acpi_os_delete_lock(acpi_spinlock_t handle)
{
}

acpi_cpuflags_t acpi_os_acquire_lock(acpi_spinlock_t handle)
{
}

void acpi_os_release_lock(acpi_spinlock_t handle, acpi_cpuflags_t flags)
{
}

acpi_status_t acpi_os_create_semaphore(uint32_t max_units,
				       uint32_t initial_units,
				       acpi_handle_t *out_handle)
{
}

acpi_status_t acpi_os_delete_semaphore(acpi_handle_t handle)
{
}

acpi_status_t acpi_os_wait_semaphore(acpi_handle_t handle,
				     uint32_t units,
				     uint16_t timeout)
{
}

acpi_status_t acpi_os_signal_semaphore(acpi_handle_t handle,
				       uint32_t units)
{
}
