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
			     acpi_tag_t tag, acpi_type_t object_type)
{
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

static void __acpi_node_exit(struct acpi_object *object)
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
	if (node->operand) {
		acpi_operand_close(node->operand);
		node->operand = NULL;
	}
}

struct acpi_namespace_node *__acpi_node_open(acpi_ddb_t ddb,
					     struct acpi_namespace_node *parent,
					     acpi_tag_t tag,
					     acpi_type_t object_type)
{
	struct acpi_namespace_node *node;
	struct acpi_object *object;

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

void __acpi_node_close(struct acpi_namespace_node *node)
{
	struct acpi_object *object = ACPI_CAST_PTR(struct acpi_object, node);

	if (!acpi_object_is_closing(object))
		acpi_event_space_notify(node, ACPI_EVENT_SPACE_DELETE);
	acpi_object_close(object);
}

struct acpi_namespace_node *__acpi_node_lookup(acpi_ddb_t ddb,
					       struct acpi_namespace_node *scope,
					       acpi_tag_t tag,
					       acpi_type_t object_type,
					       boolean create)
{
	struct acpi_namespace_node *node;

	BUG_ON(!scope);

	list_for_each_entry(struct acpi_namespace_node, node, &scope->children, sibling) {
		if (!acpi_object_is_closing(&node->common) && (node->tag == tag))
			return node;
	}
	node = NULL;
	if (create)
		node = __acpi_node_open(ddb, scope, tag, object_type);
	return node;
}

struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node,
					  const char *hint)
{
	acpi_name_t name;

	if (node) {
		ACPI_NAMECPY(node->tag, name);
		acpi_dbg("[NS-%p-%d-%4.4s] INC(%s)", node,
			 node->common.reference_count.count+1,
			 name, hint);
	}
	acpi_object_get(ACPI_CAST_PTR(struct acpi_object, node));
	return node;
}

struct acpi_namespace_node *__acpi_node_get_graceful(struct acpi_namespace_node *node,
						     const char *hint)
{
	if (!node || acpi_object_is_closing(&node->common))
		return NULL;
	return acpi_node_get(node, hint);
}

void acpi_node_put(struct acpi_namespace_node *node, const char *hint)
{
	acpi_name_t name;

	if (!node)
		return;
	ACPI_NAMECPY(node->tag, name);
	acpi_dbg("[NS-%p-%d-%4.4s] DEC(%s)", node,
		 node->common.reference_count.count-1, name, hint);
	acpi_object_put(ACPI_CAST_PTR(struct acpi_object, node));
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
				      acpi_type_t object_type,
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
			acpi_node_put(node, "depth");
			acpi_space_lock();
		}
	}

	return terminated;
}

#define __ACPI_SPACE_WALK_CALL(_n, _t, _cb, _ctx, _b)	\
	_b = __acpi_space_walk_call(_n, _t, _cb, _ctx);	\
	if (_b)						\
		break;

#define __ACPI_SPACE_VALID_DEPTH(__cur, __max)		\
	(__max == ACPI_SPACE_DEPTH_INFINITE || __cur < __max)

void acpi_space_walk_depth_first(acpi_handle_t scope,
				 acpi_type_t object_type,
				 uint32_t max_depth,
				 acpi_space_cb descending_callback,
				 acpi_space_cb ascending_callback,
				 void *context)
{
	struct acpi_namespace_node *child, *parent;
	struct acpi_namespace_node *scope_node;
	struct acpi_namespace_node *defer_node;
	boolean terminated = false;
	uint32_t level;
	boolean ascending = false;

	defer_node = NULL;
	if (scope)
		scope_node = acpi_node_get(scope, "walk");
	else
		scope_node = acpi_node_get(acpi_gbl_root_node, "walk");
	BUG_ON(!scope_node);

	acpi_space_lock();
	parent = scope_node;
	child = acpi_space_walk_next(parent, NULL);
	level = 1;

	while (level > 0 && child) {
		if (defer_node) {
			acpi_space_unlock();
			acpi_node_put(defer_node, "defer");
			defer_node = NULL;
			acpi_space_lock();
		}
		if (!ascending) {
			__ACPI_SPACE_WALK_CALL(child, object_type,
					       descending_callback,
					       context, terminated);
		} else {
			defer_node = acpi_node_get(child, "defer");
			__ACPI_SPACE_WALK_CALL(child, object_type,
					       ascending_callback,
					       context, terminated);
		}
		if (!ascending && __ACPI_SPACE_VALID_DEPTH(level, max_depth)) {
			if (!list_empty(&child->children)) {
				level++;
				parent = child;
				child = acpi_space_walk_next(parent, NULL);
				continue;
			}
		}
		if (!ascending) {
			ascending = true;
			continue;
		}
		child = acpi_space_walk_next(parent, child);
		if (child)
			ascending = false;
		else {
			level--;
			child = parent;
			parent = parent->parent;
			ascending = true;
		}
	}
	acpi_space_unlock();

	acpi_node_put(scope_node, "walk");
	if (defer_node)
		acpi_node_put(defer_node, "defer");
}

struct acpi_namespace_node *acpi_space_get_node(acpi_ddb_t ddb,
						struct acpi_namespace_node *scope,
						const char *name, uint32_t length,
						acpi_type_t object_type,
						boolean create, const char *hint)
{
	struct acpi_namespace_node *node = NULL;
	struct acpi_namespace_node *scope_node = NULL;
	uint8_t nr_segs = 0;

	acpi_space_lock();
	if (!name) {
		scope_node = acpi_node_get(acpi_gbl_root_node, "lookup");
		goto exit_ref;
	}
	if (*name == AML_ROOT_PFX) {
		if (length > 0) {
			scope_node = acpi_node_get(acpi_gbl_root_node, "lookup");
			name++, length--;
		}
	} else {
		scope_node = acpi_node_get(scope, "lookup");
		while (scope_node && (length > 0) && (*name == AML_PARENT_PFX)) {
			node = acpi_node_get(scope_node->parent, "lookup");
			acpi_node_put(scope_node, "lookup");
			scope_node = node;
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
		node = __acpi_node_lookup(ddb, scope_node, ACPI_NAME2TAG(name),
					  object_type, create);
		name += ACPI_NAME_SIZE, length -= ACPI_NAME_SIZE, nr_segs--;
		node = acpi_node_get(node, "lookup");
		acpi_node_put(scope_node, "lookup");
		scope_node = node;
	}

exit_ref:
	node = __acpi_node_get_graceful(scope_node, hint);
	acpi_space_unlock();
	acpi_node_put(scope_node, "lookup");
	return node;
}

acpi_handle_t acpi_space_open(acpi_ddb_t ddb, acpi_handle_t scope,
			      const char *name, uint32_t length,
			      acpi_type_t object_type,
			      boolean create_node)
{
	BUG_ON(create_node && ACPI_DDB_HANDLE_INVALID == ddb);
	return acpi_space_get_node(ddb, scope, name, length,
				   object_type, create_node, "space");
}

void acpi_space_close_node(acpi_handle_t node)
{
	acpi_space_lock();
	__acpi_node_close(node);
	acpi_space_unlock();
}

void acpi_space_close(acpi_handle_t node, boolean delete_node)
{
	if (delete_node)
		acpi_space_close_node(node);
	acpi_node_put(node, "space");
}

acpi_handle_t acpi_space_open_exist(acpi_handle_t scope,
				    const char *name, uint32_t length)
{
	return acpi_space_get_node(ACPI_DDB_HANDLE_INVALID, scope, name, length,
				   ACPI_TYPE_ANY, false, "space");
}

void acpi_space_close_exist(acpi_handle_t node)
{
	acpi_node_put(node, "space");
}

void acpi_space_increment(acpi_handle_t node)
{
	(void)acpi_node_get(node, "reference");
}

void acpi_space_decrement(acpi_handle_t node)
{
	acpi_node_put(node, "reference");
}

/*
 * acpi_space_get_full_path() - obtain namespace node's full path
 * @node: namespace node
 * @fullpath: namespace path in ASL format
 * @size: buffer size of ASL namespace path
 *
 * Return 1 if the AML path is empty, otherwise returning (length of ASL
 * path + 1) which means the 'fullpath' contains a trailing null.
 * Note that if the size of 'fullpath' isn't large enough to contain the
 * namespace node's ASL path, the actual required buffer length is
 * returned, which should be greater than 'size'. So callers may check the
 * returning value if the buffer size of 'fullpath' cannot be determined.
 */
acpi_path_len_t acpi_space_get_full_path(acpi_handle_t node,
					 char *fullpath, acpi_path_len_t size)
{
	struct acpi_namespace_node *namespace_node = (struct acpi_namespace_node *)node;
	acpi_path_len_t length = 0, i;
	acpi_name_t name;
	boolean trailing;
	char c, *left, *right;

#define ACPI_PATH_PUT8(name, size, byte, index)		\
	do {						\
		if ((index) < (size))			\
			(name)[(index)] = (byte);	\
		(index)++;				\
	} while (0)

	if (!fullpath)
		size = 0;

	while (namespace_node) {
		ACPI_NAMECPY(namespace_node->tag, name);
		trailing = true;
		for (i = 0; i < 4; i++) {
			c = name[4-i-1];
			if (trailing && c != '_')
				trailing = false;
			if (!trailing)
				ACPI_PATH_PUT8(fullpath, size, c, length);
		}
		namespace_node = namespace_node->parent;
		if (namespace_node && namespace_node != acpi_gbl_root_node)
			ACPI_PATH_PUT8(fullpath, size, AML_DUAL_NAME_PFX, length);
	}

	/*
	 * NOTE: No need to append AML_ROOT_PFX here because our
	 * ACPI_ROOT_NAME, so mark it to catch future changes.
	 */
	BUG_ON(strcmp(ACPI_ROOT_NAME, "\\___"));

	/* reverse the path string */
	if (length <= size) {
		left = fullpath;
		right = fullpath+length-1;
		while (left < right) {
			c = *left;
			*left++ = *right;
			*right-- = c;
		}
	}

	/* append the trailing null */
	ACPI_PATH_PUT8(fullpath, size, '\0', length);

#undef ACPI_PATH_PUT8

	return length;
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
	return acpi_space_get_node(ACPI_DDB_HANDLE_INVALID, scope, name, length,
				   ACPI_TYPE_ANY, true, "space");
}

void acpi_space_test_nodes(void)
{
	struct acpi_namespace_node *node1, *node2;
	struct acpi_namespace_node *node11, *node12;
	struct acpi_namespace_node *node21, *node22;
	struct acpi_namespace_node *node3;
	uint8_t aml_path[ACPI_AML_PATH_SIZE];
	uint8_t asl_path[ACPI_ASL_PATH_SIZE];
	acpi_path_len_t len1, len2, saved_len;
	acpi_path_t path = { ACPI_AML_PATH_SIZE, aml_path };
	acpi_name_t name;
	uint8_t parent_aml_path[ACPI_AML_PATH_SIZE];
	acpi_path_t parent = { ACPI_AML_PATH_SIZE, parent_aml_path };

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

#define ACPI_PATH_LEN_SAVE(__path, __saved, __new)	\
	do {						\
		(__saved) = (__path)->length;		\
		(__path)->length = __new;		\
	} while (0)
#define ACPI_PATH_LEN_RESTORE(__path, __saved)		\
	do {						\
		(__path)->length = (__saved);		\
	} while (0)

	BUG_ON(!acpi_gbl_root_node);

	ACPI_PATH_LEN_SAVE(&path, saved_len, 1);
	len1 = acpi_path_encode(ACPI_NAME_OK1, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK1, &path);
	ACPI_PATH_LEN_RESTORE(&path, saved_len);
	BUG_ON(len1 != len2);

	len1 = acpi_path_encode(ACPI_NAME_OK1, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK1, &path);
	BUG_ON(len1 != len2);
	ACPI_PATH_LEN_SAVE(&path, saved_len, len2-1);
	len1 = acpi_path_split(&path, &parent, name);
	ACPI_PATH_LEN_RESTORE(&path, saved_len);
	BUG_ON(len2 != (len1 + ACPI_NAME_SIZE));

	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	ACPI_PATH_LEN_SAVE(&path, saved_len, strlen(path.names)-3);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	ACPI_PATH_LEN_RESTORE(&path, saved_len);
	BUG_ON(len2 != 0);

	len1 = acpi_path_encode(ACPI_NAME_OK2, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK2, &path);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);

	len1 = acpi_path_encode(ACPI_NAME_OK3, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK3, &path);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);

	len1 = acpi_path_encode(ACPI_NAME_OK4, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK4, &path);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);

	len1 = acpi_path_encode(ACPI_NAME_OK5, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK5, &path);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);

	len1 = acpi_path_encode(ACPI_NAME_OK6, NULL);
	len2 = acpi_path_encode(ACPI_NAME_OK6, &path);
	BUG_ON(len1 != len2);
	BUG_ON(len1 == 0);
	len1 = acpi_path_decode(&path, NULL, 0);
	len2 = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
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
	node3 = acpi_space_open_test(acpi_gbl_root_node, "__03", 4);

	len1 = acpi_space_get_full_path(node22, NULL, 0);
	len2 = acpi_space_get_full_path(node22, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);
	len1 = acpi_space_get_full_path(node3, NULL, 0);
	len2 = acpi_space_get_full_path(node3, asl_path, ACPI_ASL_PATH_SIZE);
	BUG_ON(len1 != len2);

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
	acpi_space_close(node3, true);

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
	node = __acpi_node_open(ACPI_DDB_HANDLE_INVALID, NULL,
				ACPI_ROOT_TAG, ACPI_TYPE_DEVICE);
	if (!node) {
		acpi_space_unlock();
		return AE_NO_MEMORY;
	}
	acpi_space_unlock();

	acpi_space_test_nodes();

	return AE_OK;
}
