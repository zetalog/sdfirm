#include "acpi_int.h"

struct acpi_namespace_node *acpi_gbl_root_node = NULL;
acpi_mutex_t acpi_gbl_space_mutex;

void acpi_space_lock(void)
{
	//(void)acpi_os_acquire_mutex(acpi_gbl_space_mutex, ACPI_WAIT_FOREVER);
}

void acpi_space_unlock(void)
{
	//acpi_os_release_mutex(acpi_gbl_space_mutex);
}

struct acpi_namespace_node *acpi_space_lookup_node(const char *name,
						   uint32_t length)
{
	struct acpi_namespace_node *node = NULL;

	acpi_space_lock();
	if (!name) {
		if (!acpi_gbl_root_node) {
			node = acpi_os_allocate_zeroed(sizeof (struct acpi_namespace_node));
			if (!node)
				goto err_lock;

			node->common.descriptor_type = ACPI_DESC_TYPE_NAMED;
			acpi_reference_set(&node->common.reference_count, 1);
			node->object_type = ACPI_TYPE_DEVICE;
			ACPI_NAMECPY(ACPI_ROOT_TAG, node->name);
			node->parent = NULL;
			node->child = NULL;
			node->peer = NULL;
			acpi_gbl_root_node = node;
		} else {
			node = acpi_gbl_root_node;
		}
	} else {
	}
	(void)acpi_space_get_node(node);

err_lock:
	acpi_space_unlock();
	return node;
}

struct acpi_namespace_node *acpi_space_get_node(struct acpi_namespace_node *node)
{
	if (node)
		acpi_reference_inc(&node->common.reference_count);
	return node;
}

void acpi_space_put_node(struct acpi_namespace_node *node)
{
	if (!node)
		return;
	if (acpi_reference_dec_and_test(&node->common.reference_count))
		acpi_os_free(node);
}

acpi_status_t acpi_initialize_namespace(void)
{
	acpi_status_t status;

	status = acpi_os_create_mutex(&acpi_gbl_space_mutex);
	if (ACPI_FAILURE(status))
		return status;

	return AE_OK;
}
