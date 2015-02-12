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

static void __acpi_node_init(struct acpi_namespace_node *node,
			     acpi_ddb_t ddb,
			     struct acpi_namespace_node *parent,
			     acpi_tag_t tag, acpi_object_type object_type)
{
	acpi_reference_set(&node->common.reference_count, 1);
	INIT_LIST_HEAD(&node->sibling);
	INIT_LIST_HEAD(&node->children);

	node->common.descriptor_type = ACPI_DESC_TYPE_NAMED;
	node->object_type = object_type;

	if (ddb != ACPI_DDB_HANDLE_INVALID)
		acpi_table_increment(ddb);
	node->ddb = ddb;
	node->tag = tag;

	if (!parent) {
		BUG_ON(acpi_gbl_root_node);
		acpi_gbl_root_node = node;
	} else {
		node->parent = acpi_node_get(parent, "object");
		list_add(&node->sibling, &parent->children);
	}
}

static void acpi_node_release(struct acpi_object_header *object)
{
	struct acpi_namespace_node *node =
		ACPI_CAST_PTR(struct acpi_namespace_node, object);

	if (node->parent) {
		acpi_node_put(node->parent, "object");
		node->parent = NULL;
	}
	if (node->ddb != ACPI_DDB_HANDLE_INVALID) {
		acpi_table_decrement(node->ddb);
		node->ddb = ACPI_DDB_HANDLE_INVALID;
	}
}

struct acpi_namespace_node *acpi_node_open(acpi_ddb_t ddb,
					   struct acpi_namespace_node *parent,
					   acpi_tag_t tag,
					   acpi_object_type object_type)
{
	struct acpi_namespace_node *node;
	struct acpi_object_header *object;

	object = acpi_object_open(ACPI_DESC_TYPE_NAMED,
				  sizeof (struct acpi_namespace_node),
				  acpi_node_release);
	node = ACPI_CAST_PTR(struct acpi_namespace_node, object);
	if (node)
		__acpi_node_init(node, ddb, parent, tag, object_type);

	return node;
}

void acpi_node_close(struct acpi_namespace_node *node)
{
	acpi_object_close(ACPI_CAST_PTR(struct acpi_object_header, node));
}

struct acpi_namespace_node *acpi_node_lookup(acpi_ddb_t ddb,
					     struct acpi_namespace_node *scope,
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
		acpi_node_open(ddb, scope, tag, object_type);
	return node;
}

struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node,
					  const char *hint)
{
	if (node)
		acpi_dbg("[NS-%p] INC(%s)", node, hint);
	acpi_object_get(ACPI_CAST_PTR(struct acpi_object_header, node));
	return node;
}

void acpi_node_put(struct acpi_namespace_node *node, const char *hint)
{
	if (!node)
		return;

	acpi_dbg("[NS-%p] DEC(%s)", node, hint);
	acpi_object_put(ACPI_CAST_PTR(struct acpi_object_header, node));
}

struct acpi_namespace_node *acpi_space_get_node(acpi_ddb_t ddb,
						struct acpi_namespace_node *scope,
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
		node = acpi_node_lookup(ddb, scope_node, ACPI_NAME2TAG(name),
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
	node = acpi_node_open(ACPI_DDB_HANDLE_INVALID, NULL,
			      ACPI_ROOT_TAG, ACPI_TYPE_DEVICE);
	if (!node) {
		acpi_space_unlock();
		return AE_NO_MEMORY;
	}
	acpi_space_unlock();

	return AE_OK;
}
