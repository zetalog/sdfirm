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
		list_add_tail(&node->sibling, &parent->children);
	}
}

static void __acpi_node_exit(struct acpi_object_header *object)
{
	struct acpi_namespace_node *node =
		ACPI_CAST_PTR(struct acpi_namespace_node, object);

	acpi_space_lock();
	list_del(&node->sibling);
	acpi_space_unlock();

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
				  __acpi_node_exit);
	node = ACPI_CAST_PTR(struct acpi_namespace_node, object);
	if (node) {
		__acpi_node_init(node, ddb, parent, tag, object_type);
		acpi_event_space_notify(node, ACPI_EVENT_SPACE_CREATE);
	}

	return node;
}

void acpi_node_close(struct acpi_namespace_node *node)
{
	acpi_event_space_notify(node, ACPI_EVENT_SPACE_DELETE);
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
		if (!acpi_object_is_closing(&node->common) && (node->tag == tag))
			return node;
	}
	if (create)
		node = acpi_node_open(ddb, scope, tag, object_type);
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

struct acpi_namespace_node *acpi_node_get_graceful(struct acpi_namespace_node *node,
						   const char *hint)
{
	struct acpi_object_header *object;

	if (node && !acpi_object_is_closing(&node->common))
		acpi_dbg("[NS-%p] INC(%s)", node, hint);
	object = acpi_object_get_graceful(ACPI_CAST_PTR(struct acpi_object_header, node));
	return ACPI_CAST_PTR(struct acpi_namespace_node, object);
}

void acpi_node_put(struct acpi_namespace_node *node, const char *hint)
{
	if (!node)
		return;

	acpi_dbg("[NS-%p] DEC(%s)", node, hint);
	acpi_object_put(ACPI_CAST_PTR(struct acpi_object_header, node));
}

static struct acpi_namespace_node *acpi_space_walk_next(struct acpi_namespace_node *scope,
							struct acpi_namespace_node *iter)
{
	struct acpi_namespace_node *next;

	if (iter)
		next = list_entry(iter->sibling.next, struct acpi_namespace_node, sibling);
	else
		next = list_entry(scope->children.next, struct acpi_namespace_node, sibling);
	if (&next->sibling == &scope->children)
		return NULL;
	else
		return next;
}

#if 0
static struct acpi_namespace_node *acpi_space_walk_prev(struct acpi_namespace_node *scope,
							struct acpi_namespace_node *iter)
{
	struct acpi_namespace_node *prev;

	if (iter)
		prev = list_entry(iter->sibling.prev, struct acpi_namespace_node, sibling);
	else
		prev = list_entry(scope->children.prev, struct acpi_namespace_node, sibling);
	if (&prev->sibling == &scope->children)
		return NULL;
	else
		return prev;
}
#endif

static boolean __acpi_space_walk_call(struct acpi_namespace_node *node,
				      acpi_object_type object_type,
				      acpi_space_cb callback, void *context)
{
	boolean terminated = false;

	if (callback && node &&
	    (object_type == ACPI_TYPE_ANY ||
	     object_type == node->object_type)) {
		if (!acpi_object_is_closing(&node->common)) {
			acpi_node_get(node, "depth");
			acpi_space_unlock();
			terminated = callback(node, context);
			acpi_space_lock();
			acpi_node_put(node, "depth");
		}
	}

	return terminated;
}

#define __ACPI_SPACE_WALK_CALL(_n, _t, _cb, _ctx, _b)	\
	_b = __acpi_space_walk_call(_n, _t, _cb, _ctx);	\
	if (_b)						\
		break;

void acpi_space_walk_depth_first(struct acpi_namespace_node *scope,
				 acpi_object_type object_type,
				 uint32_t max_depth,
				 acpi_space_cb descending_callback,
				 acpi_space_cb ascending_callback,
				 void *context)
{
	struct acpi_namespace_node *child, *parent, *node;
	struct acpi_namespace_node *scope_node;
	boolean terminated = false;
	uint32_t level;

	if (scope)
		scope_node = acpi_node_get(scope, "walk");
	else
		scope_node = acpi_node_get(acpi_gbl_root_node, "walk");
	BUG_ON(!scope_node);

	acpi_space_lock();
	parent = scope_node;
	child = NULL;
	level = 1;

	while (parent) {
		node = child = acpi_space_walk_next(parent, child);
		while (child && level < max_depth) {
			__ACPI_SPACE_WALK_CALL(child, object_type,
					       descending_callback,
					       context, terminated);
			child = acpi_space_walk_next(node, NULL);
			if (child) {
				parent = node;
				node = child;
				level++;
			}
		}
		if (!terminated) {
			if (node) {
				__ACPI_SPACE_WALK_CALL(node, object_type,
						       ascending_callback,
						       context, terminated);
				child = node;
			} else {
				if (parent == scope_node)
					break;
				__ACPI_SPACE_WALK_CALL(parent, object_type,
						       ascending_callback,
						       context, terminated);
				parent = acpi_space_walk_next(parent->parent, parent);
				__ACPI_SPACE_WALK_CALL(parent, object_type,
						       descending_callback,
						       context, terminated);
			}
		}
	}

	acpi_node_put(scope_node, "walk");
	acpi_space_unlock();
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
		while (scope_node && (length > 0) && (*name == AML_PARENT_PFX)) {
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
	node = acpi_node_get_graceful(scope_node, hint);
	acpi_space_unlock();
	return node;
}

struct acpi_namespace_node *acpi_space_open(acpi_ddb_t ddb,
					    struct acpi_namespace_node *scope,
					    const char *name, uint32_t length,
					    boolean create_node)
{
	BUG_ON(create_node && ACPI_DDB_HANDLE_INVALID == ddb);
	return acpi_space_get_node(ddb, scope, name, length, create_node, "space");
}

void acpi_space_close(struct acpi_namespace_node *node, boolean delete_node)
{
	if (delete_node)
		acpi_node_close(node);
	acpi_node_put(node, "space");
}

struct acpi_namespace_node *acpi_space_open_exist(struct acpi_namespace_node *scope,
						  const char *name, uint32_t length)
{
	return acpi_space_get_node(ACPI_DDB_HANDLE_INVALID, scope, name,
				   length, false, "space");
}

void acpi_space_close_exist(struct acpi_namespace_node *node)
{
	acpi_node_put(node, "space");
}

#ifdef CONFIG_ACPI_DEBUG
static boolean acpi_space_descend_test(struct acpi_namespace_node *node,
				       void *unused)
{
	acpi_name_t name;

	ACPI_NAMECPY(node->tag, name);
	acpi_dbg("Descending [%4.4s]", name);
	return false;
}

static boolean acpi_space_ascend_test(struct acpi_namespace_node *node,
				      void *unused)
{
	acpi_name_t name;

	ACPI_NAMECPY(node->tag, name);
	acpi_dbg("Ascending [%4.4s]", name);
	return false;
}

struct acpi_namespace_node *acpi_space_open_test(struct acpi_namespace_node *scope,
						 const char *name, uint32_t length)
{
	return acpi_space_get_node(ACPI_DDB_HANDLE_INVALID, scope, name,
				   length, true, "space");
}

void acpi_space_test_nodes(void)
{
	struct acpi_namespace_node *node1, *node2;
	struct acpi_namespace_node *node11, *node12;
	struct acpi_namespace_node *node21, *node22;
	uint8_t buf[ACPI_AML_PATH_SIZE];
	acpi_path_len_t len1, len2;
	acpi_path_t path = { ACPI_AML_PATH_SIZE, buf };

#define _N9			"ZYX9.WVU8.TSR7.QPO6.NML5.KJI4.HGF3.EDC2.BA10"
#define _R4			"\\\\\\\\"
#define _P4			"^^^^"
#define _P16			_P4 _P4 _P4 _P4
#define _P64			_P16 _P16 _P16 _P16
#define _N18			_N9 _N9
#define _N72			_N18 _N18 _N18 _N18

#define ACPI_NAME_OK1		_R4 _N9
#define ACPI_NAME_OK2		_P4 _N9
#define ACPI_NAME_OK3		"ZYX"
#define ACPI_NAME_OK4		"ZYX9..WVU8"
#define ACPI_NAME_OK5		"..ZYX9..WVU8"
#define ACPI_NAME_OK6		"ZYX.."
#define ACPI_NAME_NG1		_P64 _P64 _P64 _P64 _N9
#define ACPI_NAME_NG2		_N72 _N72 _N72 _N72
#define ACPI_NAME_NG3		"ZYX9W"
#define ACPI_NAME_NG4		"ZYX\\"
#define ACPI_NAME_NG5		"ZYX^"

	BUG_ON(!acpi_gbl_root_node);

	path.length = 1;
	len1 = acpi_path_encode(ACPI_NAME_OK1, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK1, &path);
	BUG_ON(len1 != len2);
	path.length = ACPI_AML_PATH_SIZE;
	len1 = acpi_path_encode(ACPI_NAME_OK1, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK1, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_OK2, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK2, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_OK3, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK3, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_OK4, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK4, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_OK5, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK5, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_OK6, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK6, &path);
	BUG_ON(len1 != len2);
	len1 = acpi_path_encode(ACPI_NAME_NG1, NULL);
	BUG_ON(len1 != 0);
	len1 = acpi_path_encode(ACPI_NAME_NG2, NULL);
	BUG_ON(len1 != 0);
	len1 = acpi_path_encode(ACPI_NAME_NG3, NULL);
	BUG_ON(len1 != 0);
	len1 = acpi_path_encode(ACPI_NAME_NG4, NULL);
	BUG_ON(len1 != 0);
	len1 = acpi_path_encode(ACPI_NAME_NG5, NULL);
	BUG_ON(len1 != 0);

	node1 = acpi_space_open_test(acpi_gbl_root_node, "N001", 4);
	node2 = acpi_space_open_test(acpi_gbl_root_node, "N002", 4);
	node11 = acpi_space_open_test(node1, "N011", 4);
	node12 = acpi_space_open_test(node1, "N012", 4);
	node21 = acpi_space_open_test(node2, "N021", 4);
	node22 = acpi_space_open_test(node2, "N022", 4);

	acpi_space_walk_depth_first(NULL, ACPI_TYPE_ANY, 3,
				    acpi_space_descend_test,
				    NULL, NULL);
	acpi_space_walk_depth_first(NULL, ACPI_TYPE_ANY, 3, NULL,
				    acpi_space_ascend_test,
				    NULL);

	acpi_space_close(node1, true);
	acpi_space_close(node11, true);
	acpi_space_close(node12, true);
	acpi_space_close(node21, true);
	acpi_space_close(node22, true);
	acpi_space_close(node2, true);

	BUG_ON(acpi_space_open_exist(acpi_gbl_root_node, "N001", 4));
	BUG_ON(acpi_space_open_exist(acpi_gbl_root_node, "N002", 4));
}
#endif

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

	acpi_space_test_nodes();

	return AE_OK;
}
