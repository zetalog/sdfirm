#include "acpi_int.h"

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2)
{
	return ACPI_NAME2TAG(name2) - ACPI_NAME2TAG(name2);
}

int acpi_compare_sig_name(acpi_tag_t sig, acpi_name_t name)
{
	return ACPI_NAME2TAG(name) - sig;
}

/*
 * acpi_path_split() - split AML namespace path into parent AML namespace
 *                     path and child AML namespace name
 * @path: namespace path in AML format
 * @parent: parent namespace path in AML format
 * @name: child namespace name in AML format
 *
 * Return 0 if the AML path is in wrong format, 1 if the AML path is
 * empty, otherwise returning (length of parent AML path + 1) which means
 * the parent->names contains a trailing null.
 * Note that if the parent->names isn't large enough to contain the split
 * AML path, the actual required buffer length is returned, which should
 * be greater than parent->length. So callers may check the returning
 * value if the buffer size of parent->names cannot be determined.
 */
acpi_path_len_t acpi_path_split(acpi_path_t *path,
				acpi_path_t *parent, acpi_name_t name)
{
	char *iter, *end;
	int nr_segs;
	acpi_path_len_t length = 0, size, i;

#define ACPI_PATH_PUT8(path, size, byte, index)			\
	do {							\
		if ((index) < (size))				\
			(path)->names[(index)] = (byte);	\
		(index)++;					\
	} while (0)

	if (!parent || !parent->names)
		size = 0;
	else
		size = parent->length;

	if (!path || !path->names) {
		nr_segs = 0;
		iter = ACPI_NULL_NAME;
		end = iter + ACPI_NAME_SIZE;
	} else {
		end = path->names + path->length;
		iter = path->names;
		if (iter < end && *iter == AML_ROOT_PFX) {
			ACPI_PATH_PUT8(parent, size, AML_ROOT_PFX, length);
			while (iter < end && *iter == AML_ROOT_PFX)
				iter++;
		}
		else {
			while (iter < end && *iter == AML_PARENT_PFX) {
				ACPI_PATH_PUT8(parent, size, AML_PARENT_PFX, length);
				iter++;
			}
		}
		if (iter < end) {
			switch (*iter) {
			case 0:
				nr_segs = 0;
				break;
			case AML_DUAL_NAME_PFX:
				nr_segs = 2;
				iter++;
				break;
			case AML_MULTI_NAME_PFX:
				iter++;
				if (iter < end) {
					nr_segs = (uint32_t)(uint8_t)*iter;
					iter++;
					if (nr_segs > 3) {
						ACPI_PATH_PUT8(parent, size, AML_MULTI_NAME_PFX, length);
						ACPI_PATH_PUT8(parent, size, nr_segs - 1, length);
					} else if (nr_segs > 2)
						ACPI_PATH_PUT8(parent, size, AML_DUAL_NAME_PFX, length);
				} else
					return 0;
				break;
			default:
				nr_segs = 1;
				break;
			}
		}
	}
	while ((end - iter) >= ACPI_NAME_SIZE && nr_segs > 1) {
		nr_segs--;
		for (i = 0; i < ACPI_NAME_SIZE; i++) {
			ACPI_PATH_PUT8(parent, size, *iter, length);
			iter++;
		}
        }
	ACPI_PATH_PUT8(parent, size, '\0', length);
	if ((end - iter) < ACPI_NAME_SIZE || nr_segs != 1)
		return 0;
	if (name)
		ACPI_NAMECPY(ACPI_NAME2TAG(iter), name);

#undef ACPI_PATH_PUT8

	return length;
}

static boolean acpi_path_valid_byte(uint8_t byte, boolean leading)
{
	if ((byte >= 'A' && byte <= 'Z') || byte == '_')
		return true;
	if (!leading && (byte >= '0' && byte <= '9'))
		return true;
	return false;
}

boolean acpi_path_has_trailing_null(acpi_path_t *path)
{
	BUG_ON(!path || !path->names);
	return (path->names[path->length - 1] == '\0') ? true : false;
}

/*
 * acpi_path_encode() - convert ASL namespace path into AML namespace path
 * @name: namespace path in ASL format
 * @path: namespace path in AML format
 *
 * Return 0 if the ASL path is in wrong format, 1 if the ASL path is
 * empty, otherwise returning (length of AML path + 1) which means the
 * path->names contains a trailing null.
 * Note that if the path->names isn't large enough to contain the encoded
 * AML path, the actual required buffer length is returned, which should
 * be greater than path->length. So callers may check the returning value
 * if the buffer size of path->names cannot be determined.
 */
acpi_path_len_t acpi_path_encode(const char *name, acpi_path_t *path)
{
	const char *iter, *saved_name;
	uint8_t nr_segs;
	acpi_path_len_t length = 0, size;
	uint8_t seg_bytes;
	boolean leading;

#define ACPI_PATH_PUT8(path, size, byte, index)			\
	do {							\
		if ((index) < (size))				\
			(path)->names[(index)] = (byte);	\
		(index)++;					\
	} while (0)

	if (!path || !path->names)
		size = 0;
	else
		size = path->length;
	iter = name;
	nr_segs = 0;
	if (*iter == AML_ROOT_PFX) {
		ACPI_PATH_PUT8(path, size, AML_ROOT_PFX, length);
		while (*iter == AML_ROOT_PFX)
			iter++;
	} else {
		while (*iter == AML_PARENT_PFX) {
			if (nr_segs < ACPI_MAX_NAME_SEGS) {
				ACPI_PATH_PUT8(path, size, AML_PARENT_PFX, length);
				nr_segs++;
			} else
				return 0;
			iter++;
		}
	}

	saved_name = iter;
	seg_bytes = 4;
	nr_segs = 0;
	while (*iter) {
		if (*iter == AML_DUAL_NAME_PFX) {
			if (nr_segs < ACPI_MAX_NAME_SEGS)
				seg_bytes = 4;
			else
				return 0;
		} else {
			leading = (seg_bytes == 4) ? true : false;
			if (!acpi_path_valid_byte(*iter, leading))
				return 0;
			if (seg_bytes == 4)
				nr_segs++;
			if (seg_bytes > 0)
				seg_bytes--;
			else
				return 0;
		}
		iter++;
	}
	if (nr_segs > 2) {
		ACPI_PATH_PUT8(path, size, AML_MULTI_NAME_PFX, length);
		ACPI_PATH_PUT8(path, size, nr_segs, length);
	} else if (nr_segs > 1)
		ACPI_PATH_PUT8(path, size, AML_DUAL_NAME_PFX, length);

	iter = saved_name;
	seg_bytes = 4;
	while (*iter) {
		if (*iter == AML_DUAL_NAME_PFX) {
			if (seg_bytes != 4) {
				while (seg_bytes) {
					ACPI_PATH_PUT8(path, size, '_', length);
					seg_bytes--;
				}
				seg_bytes = 4;
			}
		} else {
			if (seg_bytes == 4) {
				BUG_ON(!nr_segs);
				nr_segs--;
			}
			BUG_ON(seg_bytes == 0);
			ACPI_PATH_PUT8(path, size, *iter, length);
			seg_bytes--;
		}
		iter++;
	}
	if (seg_bytes != 4) {
		while (seg_bytes) {
			ACPI_PATH_PUT8(path, size, '_', length);
			seg_bytes--;
		}
	}
	ACPI_PATH_PUT8(path, size, 0, length);

#undef ACPI_PATH_PUT8

	return length;
}

/*
 * acpi_path_decode() - convert AML namespace path into ASL namespace path
 * @path: namespace path in AML format
 * @name: namespace path in ASL format
 * @size: buffer size of ASL namespace path
 *
 * Return 0 if the AML path is in wrong format, 1 if the AML path is
 * empty, otherwise returning (length of ASL path + 1) which means the
 * 'name' contains a trailing null.
 * Note that if the size of 'name' isn't large enough to contain the
 * decoded ASL path, the actual required buffer length is returned, which
 * should be greater than 'size'. So callers may check the returning value
 * if the buffer size of 'name' cannot be determined.
 */
acpi_path_len_t acpi_path_decode(acpi_path_t *path,
				 char *name, acpi_path_len_t size)
{
	const char *iter, *end;
	acpi_path_len_t length = 0;
	uint8_t nr_segs;
	uint8_t seg_bytes, i;
	boolean leading;

#define ACPI_PATH_PUT8(name, size, byte, index)		\
	do {						\
		if ((index) < (size))			\
			(name)[(index)] = (byte);	\
		(index)++;				\
	} while (0)
#define ACPI_PATH_UNPUT8(index)				\
	do {						\
		(index)--;				\
	} while (0)

	if (!path || !path->names)
		return 0;
	if (!name)
		size = 0;

	end = path->names + path->length;
	iter = path->names;
	nr_segs = 0;
	if (iter < end && *iter == AML_ROOT_PFX) {
		ACPI_PATH_PUT8(name, size, AML_ROOT_PFX, length);
		while (iter < end && *iter == AML_ROOT_PFX)
			iter++;
	} else {
		while (iter < end && *iter == AML_PARENT_PFX) {
			if (nr_segs < ACPI_MAX_NAME_SEGS) {
				ACPI_PATH_PUT8(name, size, AML_PARENT_PFX, length);
				nr_segs++;
			} else
				return 0;
			iter++;
		}
	}
	if (iter == end || *iter == '\0')
		goto exit_trail;
	nr_segs = 1;
	if (iter < end && *iter == AML_MULTI_NAME_PFX) {
		iter++;
		if (iter < end) {
			nr_segs = *iter;
			iter++;
		}
	} else if (iter < end && *iter == AML_DUAL_NAME_PFX) {
		nr_segs = 2;
		iter++;
	}

	seg_bytes = 0;
	while (iter < end && *iter) {
		if (seg_bytes == 4) {
			seg_bytes = 0;
			ACPI_PATH_PUT8(name, size, AML_DUAL_NAME_PFX, length);
		}
		leading = (seg_bytes == 0) ? true : false;
		if (!acpi_path_valid_byte(*iter, leading))
			return 0;
		ACPI_PATH_PUT8(name, size, *iter, length);
		seg_bytes++;
		if (seg_bytes == 4) {
			for (i = 0; i < 4; i++) {
				if (*(iter - i) == '_')
					ACPI_PATH_UNPUT8(length);
				else
					break;
			}
			nr_segs--;
			if (nr_segs == 0)
				break;
		}
		iter++;
	}
	if (seg_bytes != 4 || nr_segs > 0)
		return 0;
exit_trail:
	ACPI_PATH_PUT8(name, size, 0, length);

#undef ACPI_PATH_PUT8
#undef ACPI_PATH_UNPUT8

	return length;
}
