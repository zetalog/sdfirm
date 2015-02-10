#include "acpi_int.h"

struct acpi_namespace_node *acpi_gbl_root_node = NULL;
acpi_mutex_t acpi_gbl_space_mutex;

void acpi_space_lock(void)
{
	(void)acpi_os_acquire_mutex(acpi_gbl_space_mutex, ACPI_WAIT_FOREVER);
}

void acpi_space_unlock(void)
{
	acpi_os_release_mutex(acpi_gbl_space_mutex);
}

struct acpi_namespace_node *acpi_node_create(struct acpi_namespace_node *parent,
					      acpi_tag_t tag,
					      acpi_object_type object_type)
{
	struct acpi_namespace_node *node;

	node = acpi_os_allocate_zeroed(sizeof (struct acpi_namespace_node));
	if (!node)
		return NULL;

	node->common.descriptor_type = ACPI_DESC_TYPE_NAMED;
	acpi_reference_set(&node->common.reference_count, 1);
	node->object_type = object_type;
	ACPI_NAMECPY(tag, node->name);
	INIT_LIST_HEAD(&node->sibling);
	INIT_LIST_HEAD(&node->children);

	if (!parent) {
		BUG_ON(acpi_gbl_root_node);
		acpi_gbl_root_node = node;
	} else {
		node->parent = parent;
		list_add(&node->sibling, &parent->children);
	}

	return node;
}

struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node)
{
	if (node)
		acpi_reference_inc(&node->common.reference_count);
	return node;
}

void acpi_node_put(struct acpi_namespace_node *node)
{
	if (!node)
		return;
	if (acpi_reference_dec_and_test(&node->common.reference_count))
		acpi_os_free(node);
}

struct acpi_namespace_node *acpi_space_lookup(const char *name, uint32_t length)
{
	struct acpi_namespace_node *node = NULL;

	acpi_space_lock();
	if (!name) {
		node = acpi_gbl_root_node;
	} else {
	}

	acpi_space_unlock();
	return acpi_node_get(node);
}

acpi_status_t acpi_initialize_namespace(void)
{
	acpi_status_t status;
	struct acpi_namespace_node *node = NULL;

	status = acpi_os_create_mutex(&acpi_gbl_space_mutex);
	if (ACPI_FAILURE(status))
		return status;

	acpi_space_lock();
	node = acpi_node_create(NULL, ACPI_ROOT_TAG, ACPI_TYPE_DEVICE);
	if (!node) {
		acpi_space_unlock();
		return AE_NO_MEMORY;
	}
	acpi_space_unlock();

	return AE_OK;
}
