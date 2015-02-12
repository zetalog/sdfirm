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
					     acpi_tag_t tag, acpi_object_type object_type)
{
	struct acpi_namespace_node *node;

	node = acpi_os_allocate_zeroed(sizeof (struct acpi_namespace_node));
	if (!node)
		return NULL;

	node->common.descriptor_type = ACPI_DESC_TYPE_NAMED;
	acpi_reference_set(&node->common.reference_count, 1);
	node->object_type = object_type;
	node->tag = tag;
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

struct acpi_namespace_node *acpi_node_lookup(struct acpi_namespace_node *scope,
					     acpi_tag_t tag,
					     acpi_object_type object_type,
					     boolean create)
{
	struct acpi_namespace_node *node;

	BUG_ON(!scope);

	list_for_each_entry(struct acpi_namespace_node, node, &scope->children, sibling) {
		if (node->tag == tag)
			return node;
	}
	if (create)
		acpi_node_create(scope, tag, object_type);
	return node;
}

struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node,
					  const char *hint)
{
	if (node) {
		acpi_dbg("[NS-%p] INC(%s)", node, hint);
		acpi_reference_inc(&node->common.reference_count);
	}
	return node;
}

void acpi_node_put(struct acpi_namespace_node *node, const char *hint)
{
	if (!node)
		return;
	acpi_dbg("[NS-%p] DEC(%s)", node, hint);
	if (!acpi_reference_dec_and_test(&node->common.reference_count))
		acpi_os_free(node);
}

struct acpi_namespace_node *acpi_space_get_node(struct acpi_namespace_node *scope,
						const char *name, uint32_t length,
						boolean create, const char *hint)
{
	struct acpi_namespace_node *node = NULL;
	struct acpi_namespace_node *scope_node = NULL;
	uint8_t nr_segs = 0;

	acpi_space_lock();
	if (!name) {
		scope_node = acpi_gbl_root_node;
		goto exit_lock;
	}
	if (*name == AML_ROOT_PFX) {
		if (length > 0) {
			scope_node = acpi_node_get(acpi_gbl_root_node, "lookup");
			name++, length--;
		}
	} else {
		scope_node = acpi_node_get(scope, "lookup");
		while ((length > 0) && (*name == AML_PARENT_PFX)) {
			acpi_node_put(scope_node, "lookup");
			scope_node = acpi_node_get(scope_node->parent, "lookup");
			name++, length--;
		}
	}
	if (length == 0 || *name == 0) {
		nr_segs = 0;
		goto exit_ref;
	}
	switch (*name) {
	default:
		nr_segs = 1;
		break;
	case AML_DUAL_NAME_PFX:
		nr_segs = 2;
		name++, length--;
		break;
	case AML_MULTI_NAME_PFX:
		if (length > 1) {
			name++, length--;
			nr_segs = (uint8_t)*name;
			name++, length--;
		}
		break;
	}
	while ((length > 0) && nr_segs && scope_node) {
		node = acpi_node_lookup(scope_node, ACPI_NAME2TAG(name),
					ACPI_TYPE_ANY, create);
		name += ACPI_NAME_SIZE, length -= ACPI_NAME_SIZE;
		acpi_node_put(scope_node, "lookup");
		scope_node = acpi_node_get(node, "lookup");
	}

exit_ref:
	acpi_node_put(scope_node, "lookup");
exit_lock:
	node = acpi_node_get(scope_node, hint);
	acpi_space_unlock();
	return node;
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
