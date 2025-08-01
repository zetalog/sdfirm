/* SPDX-License-Identifier: GPL-2.0-only */

#include <target/acpi_gen.h>
#include <target/heap.h>
#include <target/panic.h>

/* How much nesting do we support? */
#define ACPIGEN_LENSTACK_SIZE 15

/* If you need to change this, change acpigen_pop_len too */
#define ACPIGEN_RSVD_PKGLEN_BYTES	3

static char *gencurrent;

char *len_stack[ACPIGEN_LENSTACK_SIZE];
int ltop = 0;

void acpigen_write_len_f(void)
{
	BUG_ON(ltop >= (ACPIGEN_LENSTACK_SIZE - 1));
	len_stack[ltop++] = gencurrent;
	/* Reserve ACPIGEN_RSVD_PKGLEN_BYTES bytes for PkgLength. The actual byte values will
	   be written later in the corresponding acpigen_pop_len call. */
	for (size_t i = 0; i < ACPIGEN_RSVD_PKGLEN_BYTES; i++)
		acpigen_emit_byte(0);
}

void acpigen_pop_len(void)
{
	size_t len;
	BUG_ON(ltop <= 0);
	char *p = len_stack[--ltop];
	len = gencurrent - p;
	const size_t payload_len = len - ACPIGEN_RSVD_PKGLEN_BYTES;

	if (len <= 0x3f + 2) {
		/* PkgLength of up to 0x3f can be encoded in one PkgLength byte instead of the
		   reserved 3 bytes. Since only 1 PkgLength byte will be written, the payload
		   data needs to be moved by 2 bytes */
		memmove(&p[ACPIGEN_RSVD_PKGLEN_BYTES - 2],
			&p[ACPIGEN_RSVD_PKGLEN_BYTES], payload_len);
		/* Adjust the PkgLength to take into account that we only use 1 of the 3
		   reserved bytes */
		len -= 2;
		/* The two most significant bits of PkgLength get the value of 0 to indicate
		   there are no additional PkgLength bytes. In this case the single PkgLength
		   byte encodes the length in its lower 6 bits */
		p[0] = len;
		/* Adjust pointer for next ACPI bytecode byte */
		acpigen_set_current(p + len);
	} else if (len <= 0xfff + 1) {
		/* PkgLength of up to 0xfff can be encoded in 2 PkgLength bytes instead of the
		   reserved 3 bytes. Since only 2 PkgLength bytes will be written, the payload
		   data needs to be moved by 1 byte */
		memmove(&p[ACPIGEN_RSVD_PKGLEN_BYTES - 1],
			&p[ACPIGEN_RSVD_PKGLEN_BYTES], payload_len);
		/* Adjust the PkgLength to take into account that we only use 2 of the 3
		   reserved bytes */
		len -= 1;
		/* The two most significant bits of PkgLength get the value of 1 to indicate
		   there's a second PkgLength byte. The lower 4 bits of the first PkgLength
		   byte and the second PkgLength byte encode the length */
		p[0] = (0x1 << 6 | (len & 0xf));
		p[1] = (len >> 4 & 0xff);
		/* Adjust pointer for next ACPI bytecode byte */
		acpigen_set_current(p + len);
	} else if (len <= 0xfffff) {
		/* PkgLength of up to 0xfffff can be encoded in 3 PkgLength bytes. Since this
		   is the amount of reserved bytes, no need to move the payload in this case */
		/* The two most significant bits of PkgLength get the value of 2 to indicate
		   there are two more PkgLength bytes following the first one. The lower 4 bits
		   of the first PkgLength byte and the two following PkgLength bytes encode the
		   length */
		p[0] = (0x2 << 6 | (len & 0xf));
		p[1] = (len >> 4 & 0xff);
		p[2] = (len >> 12 & 0xff);
		/* No need to adjust pointer for next ACPI bytecode byte */
	} else {
		/* The case of PkgLength up to 0xfffffff isn't supported at the moment */
		con_err("%s: package length exceeds maximum of 0xfffff.\n", __func__);
	}
}

void acpigen_set_current(char *curr)
{
	gencurrent = curr;
}

char *acpigen_get_current(void)
{
	return gencurrent;
}

void acpigen_emit_byte(unsigned char b)
{
	(*gencurrent++) = b;
}

void acpigen_emit_ext_op(uint8_t op)
{
	acpigen_emit_byte(EXT_OP_PREFIX);
	acpigen_emit_byte(op);
}

void acpigen_emit_word(unsigned int data)
{
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
}

void acpigen_emit_dword(unsigned int data)
{
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
	acpigen_emit_byte((data >> 16) & 0xff);
	acpigen_emit_byte((data >> 24) & 0xff);
}

char *acpigen_write_package(int nr_el)
{
	char *p;
	acpigen_emit_byte(PACKAGE_OP);
	acpigen_write_len_f();
	p = acpigen_get_current();
	acpigen_emit_byte(nr_el);
	return p;
}

void acpigen_write_byte(unsigned int data)
{
	acpigen_emit_byte(BYTE_PREFIX);
	acpigen_emit_byte(data & 0xff);
}

void acpigen_write_word(unsigned int data)
{
	acpigen_emit_byte(WORD_PREFIX);
	acpigen_emit_word(data);
}

void acpigen_write_dword(unsigned int data)
{
	acpigen_emit_byte(DWORD_PREFIX);
	acpigen_emit_dword(data);
}

void acpigen_write_qword(uint64_t data)
{
	acpigen_emit_byte(QWORD_PREFIX);
	acpigen_emit_dword(data & 0xffffffff);
	acpigen_emit_dword((data >> 32) & 0xffffffff);
}

void acpigen_write_zero(void)
{
	acpigen_emit_byte(ZERO_OP);
}

void acpigen_write_one(void)
{
	acpigen_emit_byte(ONE_OP);
}

void acpigen_write_ones(void)
{
	acpigen_emit_byte(ONES_OP);
}

void acpigen_write_integer(uint64_t data)
{
	if (data == 0)
		acpigen_write_zero();
	else if (data == 1)
		acpigen_write_one();
	else if (data <= 0xff)
		acpigen_write_byte((unsigned char)data);
	else if (data <= 0xffff)
		acpigen_write_word((unsigned int)data);
	else if (data <= 0xffffffff)
		acpigen_write_dword((unsigned int)data);
	else
		acpigen_write_qword(data);
}

void acpigen_write_name_byte(const char *name, uint8_t val)
{
	acpigen_write_name(name);
	acpigen_write_byte(val);
}

void acpigen_write_name_dword(const char *name, uint32_t val)
{
	acpigen_write_name(name);
	acpigen_write_dword(val);
}

void acpigen_write_name_qword(const char *name, uint64_t val)
{
	acpigen_write_name(name);
	acpigen_write_qword(val);
}

void acpigen_write_name_integer(const char *name, uint64_t val)
{
	acpigen_write_name(name);
	acpigen_write_integer(val);
}

void acpigen_write_name_string(const char *name, const char *string)
{
	acpigen_write_name(name);
	acpigen_write_string(string);
}

void acpigen_write_name_unicode(const char *name, const char *string)
{
	const size_t len = strlen(string) + 1;
	acpigen_write_name(name);
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_write_integer(2 * len);
	for (size_t i = 0; i < len; i++) {
		const signed char c = string[i];
		/* Simple ASCII to UTF-16 conversion, replace non ASCII characters */
		acpigen_emit_word(c >= 0 ? c : '?');
	}
	acpigen_pop_len();
}

void acpigen_emit_stream(const char *data, int size)
{
	int i;
	for (i = 0; i < size; i++)
		acpigen_emit_byte(data[i]);
}

void acpigen_emit_string(const char *string)
{
	acpigen_emit_stream(string, string ? strlen(string) : 0);
	acpigen_emit_byte('\0'); /* NUL */
}

void acpigen_write_string(const char *string)
{
	acpigen_emit_byte(STRING_PREFIX);
	acpigen_emit_string(string);
}

void acpigen_write_coreboot_hid(enum coreboot_acpi_ids id)
{
	char hid[9]; /* BOOTxxxx */

	snprintf(hid, sizeof(hid), "%.4s%04X", COREBOOT_ACPI_ID, id);
	acpigen_write_name_string("_HID", hid);
}

/*
 * The naming conventions for ACPI namespace names are a bit tricky as
 * each element has to be 4 chars wide ("All names are a fixed 32 bits.")
 * and "By convention, when an ASL compiler pads a name shorter than 4
 * characters, it is done so with trailing underscores ('_')".
 *
 * Check sections 5.3, 18.2.2 and 18.4 of ACPI spec 3.0 for details.
 */

static void acpigen_emit_simple_namestring(const char *name)
{
	int i;
	char ud[] = "____";
	for (i = 0; i < 4; i++) {
		if ((name[i] == '\0') || (name[i] == '.')) {
			acpigen_emit_stream(ud, 4 - i);
			break;
		}
		acpigen_emit_byte(name[i]);
	}
}

static void acpigen_emit_double_namestring(const char *name, int dotpos)
{
	acpigen_emit_byte(DUAL_NAME_PREFIX);
	acpigen_emit_simple_namestring(name);
	acpigen_emit_simple_namestring(&name[dotpos + 1]);
}

static void acpigen_emit_multi_namestring(const char *name)
{
	int count = 0;
	unsigned char *pathlen;
	acpigen_emit_byte(MULTI_NAME_PREFIX);
	acpigen_emit_byte(ZERO_OP);
	pathlen = ((unsigned char *)acpigen_get_current()) - 1;

	while (name[0] != '\0') {
		acpigen_emit_simple_namestring(name);
		/* find end or next entity */
		while ((name[0] != '.') && (name[0] != '\0'))
			name++;
		/* forward to next */
		if (name[0] == '.')
			name++;
		count++;
	}

	pathlen[0] = count;
}

void acpigen_emit_namestring(const char *namepath)
{
	int dotcount = 0, i;
	int dotpos = 0;

	/* Check for NULL pointer */
	if (!namepath)
		return;

	/* We can start with a '\'. */
	if (namepath[0] == '\\') {
		acpigen_emit_byte('\\');
		namepath++;
	}

	/* And there can be any number of '^' */
	while (namepath[0] == '^') {
		acpigen_emit_byte('^');
		namepath++;
	}

	/* If we have only \\ or only ^...^. Then we need to put a null
	   name (0x00). */
	if (namepath[0] == '\0') {
		acpigen_emit_byte(ZERO_OP);
		return;
	}

	i = 0;
	while (namepath[i] != '\0') {
		if (namepath[i] == '.') {
			dotcount++;
			dotpos = i;
		}
		i++;
	}

	if (dotcount == 0)
		acpigen_emit_simple_namestring(namepath);
	else if (dotcount == 1)
		acpigen_emit_double_namestring(namepath, dotpos);
	else
		acpigen_emit_multi_namestring(namepath);
}

void acpigen_write_name(const char *name)
{
	acpigen_emit_byte(NAME_OP);
	acpigen_emit_namestring(name);
}

void acpigen_write_scope(const char *name)
{
	acpigen_emit_byte(SCOPE_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
}

void acpigen_get_package_op_element(uint8_t package_op, unsigned int element, uint8_t dest_op)
{
	/* <dest_op> = DeRefOf (<package_op>[<element>]) */
	acpigen_write_store();
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_byte(package_op);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
	acpigen_emit_byte(dest_op);
}

void acpigen_set_package_op_element_int(uint8_t package_op, unsigned int element, uint64_t src)
{
	/* DeRefOf (<package>[<element>]) = <src> */
	acpigen_write_store();
	acpigen_write_integer(src);
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_byte(package_op);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
}

void acpigen_get_package_element(const char *package, unsigned int element, uint8_t dest_op)
{
	/* <dest_op> = <package>[<element>] */
	acpigen_write_store();
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
	acpigen_emit_byte(dest_op);
}

void acpigen_set_package_element_int(const char *package, unsigned int element, uint64_t src)
{
	/* <package>[<element>] = <src> */
	acpigen_write_store();
	acpigen_write_integer(src);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
}

void acpigen_set_package_element_namestr(const char *package, unsigned int element,
					 const char *src)
{
	/* <package>[<element>] = <src> */
	acpigen_write_store();
	acpigen_emit_namestring(src);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
}

void acpigen_write_processor_namestring(unsigned int cpu_index)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "\\_SB." CONFIG_ACPI_CPU_STRING, (cpu_t)cpu_index);
	acpigen_emit_namestring(buffer);
}

/* Processor() operator is deprecated as of ACPI 6.0, use Device() instead. */
void acpigen_write_processor(uint8_t cpuindex, uint32_t pblock_addr, uint8_t pblock_len)
{
/*
	Processor (\_SB.CPcpuindex, cpuindex, pblock_addr, pblock_len)
	{
*/
	acpigen_emit_ext_op(PROCESSOR_OP);
	acpigen_write_len_f();
	acpigen_write_processor_namestring(cpuindex);
	acpigen_emit_byte(cpuindex);
	acpigen_emit_dword(pblock_addr);
	acpigen_emit_byte(pblock_len);
}

void acpigen_write_processor_device(unsigned int cpu_index)
{
	acpigen_emit_ext_op(DEVICE_OP);
	acpigen_write_len_f();
	acpigen_write_processor_namestring(cpu_index);
	acpigen_write_name_string("_HID", "ACPI0007");
	acpigen_write_name_integer("_UID", cpu_index);
}

void acpigen_write_processor_package(const char *const name, const unsigned int first_core,
				     const unsigned int core_count)
{
	unsigned int i;

	acpigen_write_name(name);
	acpigen_write_package(core_count);

	for (i = first_core; i < first_core + core_count; ++i)
		acpigen_write_processor_namestring(i);

	acpigen_pop_len();
}

/* Method to notify all CPU cores */
void acpigen_write_processor_cnot(const unsigned int number_of_cores)
{
	int core_id;

	acpigen_write_method("\\_SB.CNOT", 1);
	for (core_id = 0; core_id < number_of_cores; core_id++) {
		acpigen_emit_byte(NOTIFY_OP);
		acpigen_write_processor_namestring(core_id);
		acpigen_emit_byte(ARG0_OP);
	}
	acpigen_pop_len();
}

/*
 * Generate ACPI AML code for OperationRegion
 * Arg0: Pointer to struct opregion opreg = OPREGION(rname, space, offset, len)
 * where rname is region name, space is region space, offset is region offset &
 * len is region length.
 * OperationRegion(regionname, regionspace, regionoffset, regionlength)
 */
void acpigen_write_opregion(const struct opregion *opreg)
{
	/* OpregionOp */
	acpigen_emit_ext_op(OPREGION_OP);
	/* NameString 4 chars only */
	acpigen_emit_simple_namestring(opreg->name);
	/* RegionSpace */
	acpigen_emit_byte(opreg->regionspace);
	/* RegionOffset & RegionLen, it can be byte word or double word */
	acpigen_write_integer(opreg->regionoffset);
	acpigen_write_integer(opreg->regionlen);
}

/*
 * Generate ACPI AML code for Mutex
 * Arg0: Pointer to name of mutex
 * Arg1: Initial value of mutex
 */
void acpigen_write_mutex(const char *name, const uint8_t flags)
{
	/* MutexOp */
	acpigen_emit_ext_op(MUTEX_OP);
	acpigen_emit_namestring(name);
	acpigen_emit_byte(flags);
}

void acpigen_write_acquire(const char *name, const uint16_t val)
{
	/* AcquireOp */
	acpigen_emit_ext_op(ACQUIRE_OP);
	acpigen_emit_namestring(name);
	acpigen_emit_word(val);
}

void acpigen_write_release(const char *name)
{
	/* ReleaseOp */
	acpigen_emit_ext_op(RELEASE_OP);
	acpigen_emit_namestring(name);
}

static void acpigen_write_field_length(uint32_t len)
{
	uint8_t i, j;
	uint8_t emit[4];

	i = 1;
	if (len < 0x40) {
		emit[0] = len & 0x3F;
	} else {
		emit[0] = len & 0xF;
		len >>= 4;
		while (len) {
			emit[i] = len & 0xFF;
			i++;
			len >>= 8;
		}
	}
	/* Update bit 7:6 : Number of bytes followed by emit[0] */
	emit[0] |= (i - 1) << 6;

	for (j = 0; j < i; j++)
		acpigen_emit_byte(emit[j]);
}

static void acpigen_write_field_offset(uint32_t offset, uint32_t current_bit_pos)
{
	uint32_t diff_bits;

	if (offset < current_bit_pos) {
		con_err("%s: Cannot move offset backward", __func__);
		return;
	}

	diff_bits = offset - current_bit_pos;
	/* Upper limit */
	if (diff_bits > 0xFFFFFFF) {
		con_err("%s: Offset very large to encode", __func__);
		return;
	}

	acpigen_emit_byte(0);
	acpigen_write_field_length(diff_bits);
}

void acpigen_write_field_name(const char *name, uint32_t size)
{
	acpigen_emit_simple_namestring(name);
	acpigen_write_field_length(size);
}

static void acpigen_write_field_reserved(uint32_t size)
{
	acpigen_emit_byte(0);
	acpigen_write_field_length(size);
}

/*
 * Generate ACPI AML code for Field
 * Arg0: region name
 * Arg1: Pointer to struct fieldlist.
 * Arg2: no. of entries in Arg1
 * Arg3: flags which indicate filed access type, lock rule  & update rule.
 * Example with fieldlist
 * struct fieldlist l[] = {
 *	FIELDLIST_OFFSET(0x84),
 *	FIELDLIST_NAMESTR("PMCS", 2),
 *	FIELDLIST_RESERVED(6),
 *	};
 * acpigen_write_field("UART", l, ARRAY_SIZE(l), FIELD_ANYACC | FIELD_NOLOCK |
 *								FIELD_PRESERVE);
 * Output:
 * Field (UART, AnyAcc, NoLock, Preserve)
 *	{
 *		Offset (0x84),
 *		PMCS,   2,
 *              , 6,
 *	}
 */
void acpigen_write_field(const char *name, const struct fieldlist *l, size_t count,
			 uint8_t flags)
{
	uint16_t i;
	uint32_t current_bit_pos = 0;

	/* FieldOp */
	acpigen_emit_ext_op(FIELD_OP);
	/* Package Length */
	acpigen_write_len_f();
	/* NameString 4 chars only */
	acpigen_emit_simple_namestring(name);
	/* Field Flag */
	acpigen_emit_byte(flags);

	for (i = 0; i < count; i++) {
		switch (l[i].type) {
		case NAME_STRING:
			acpigen_write_field_name(l[i].name, l[i].bits);
			current_bit_pos += l[i].bits;
			break;
		case RESERVED:
			acpigen_write_field_reserved(l[i].bits);
			current_bit_pos += l[i].bits;
			break;
		case OFFSET:
			acpigen_write_field_offset(l[i].bits, current_bit_pos);
			current_bit_pos = l[i].bits;
			break;
		default:
			con_err("%s: Invalid field type 0x%X\n", __func__, l[i].type);
			break;
		}
	}
	acpigen_pop_len();
}

/*
 * Generate ACPI AML code for IndexField
 * Arg0: region name
 * Arg1: Pointer to struct fieldlist.
 * Arg2: no. of entries in Arg1
 * Arg3: flags which indicate filed access type, lock rule  & update rule.
 * Example with fieldlist
 * struct fieldlist l[] = {
 *	FIELDLIST_OFFSET(0x84),
 *	FIELDLIST_NAMESTR("PMCS", 2),
 *	};
 * acpigen_write_field("IDX", "DATA" l, ARRAY_SIZE(l), FIELD_ANYACC |
 *						       FIELD_NOLOCK |
 *						       FIELD_PRESERVE);
 * Output:
 * IndexField (IDX, DATA, AnyAcc, NoLock, Preserve)
 *	{
 *		Offset (0x84),
 *		PMCS,   2
 *	}
 */
void acpigen_write_indexfield(const char *idx, const char *data, struct fieldlist *l,
			      size_t count, uint8_t flags)
{
	uint16_t i;
	uint32_t current_bit_pos = 0;

	/* FieldOp */
	acpigen_emit_ext_op(INDEX_FIELD_OP);
	/* Package Length */
	acpigen_write_len_f();
	/* NameString 4 chars only */
	acpigen_emit_simple_namestring(idx);
	/* NameString 4 chars only */
	acpigen_emit_simple_namestring(data);
	/* Field Flag */
	acpigen_emit_byte(flags);

	for (i = 0; i < count; i++) {
		switch (l[i].type) {
		case NAME_STRING:
			acpigen_write_field_name(l[i].name, l[i].bits);
			current_bit_pos += l[i].bits;
			break;
		case OFFSET:
			acpigen_write_field_offset(l[i].bits, current_bit_pos);
			current_bit_pos = l[i].bits;
			break;
		default:
			con_err("%s: Invalid field type 0x%X\n", __func__, l[i].type);
			break;
		}
	}
	acpigen_pop_len();
}

void acpigen_write_empty_PCT(void)
{
/*
	Name (_PCT, Package (0x02)
	{
		ResourceTemplate ()
		{
			Register (FFixedHW,
				0x00,               // Bit Width
				0x00,               // Bit Offset
				0x0000000000000000, // Address
				,)
		},

		ResourceTemplate ()
		{
			Register (FFixedHW,
				0x00,               // Bit Width
				0x00,               // Bit Offset
				0x0000000000000000, // Address
				,)
		}
	})
*/
	static char stream[] = {
		/* 00000030    "0._PCT.," */
		0x08, 0x5F, 0x50, 0x43, 0x54, 0x12, 0x2C,
		/* 00000038    "........" */
		0x02, 0x11, 0x14, 0x0A, 0x11, 0x82, 0x0C, 0x00,
		/* 00000040    "........" */
		0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00000048    "....y..." */
		0x00, 0x00, 0x00, 0x00, 0x79, 0x00, 0x11, 0x14,
		/* 00000050    "........" */
		0x0A, 0x11, 0x82, 0x0C, 0x00, 0x7F, 0x00, 0x00,
		/* 00000058    "........" */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x79, 0x00
	};
	acpigen_emit_stream(stream, ARRAY_SIZE(stream));
}

void acpigen_write_PTC(uint8_t duty_width, uint8_t duty_offset, uint16_t p_cnt)
{
/*
	Name (_PTC, Package (0x02)
	{
		ResourceTemplate ()
		{
			Register (FFixedHW,
				0x00,               // Duty Width
				0x00,               // Duty Offset
				0x0000000000000000, // P_CNT IO Address
				,)
		},

		ResourceTemplate ()
		{
			Register (FFixedHW,
				0x00,               // Duty Width
				0x00,               // Duty Offset
				0x0000000000000000, // P_CNT IO Address
				,)
		}
	})
*/
	acpi_addr_t addr = {
		.bit_width   = duty_width,
		.bit_offset  = duty_offset,
		.access_size = ACPI_ACCESS_SIZE_UNDEFINED,
		.addrl       = p_cnt,
		.addrh       = 0,
	};

	if (addr.addrl != 0)
		addr.space_id = ACPI_ADDRESS_SPACE_IO;
	else
		addr.space_id = ACPI_ADDRESS_SPACE_FIXED;

	acpigen_write_name("_PTC");
	acpigen_write_package(2);

	/* ControlRegister */
	acpigen_write_register_resource(&addr);

	/* StatusRegister */
	acpigen_write_register_resource(&addr);

	acpigen_pop_len();
}

void acpigen_write_empty_PTC(void)
{
	acpigen_write_PTC(0, 0, 0);
}

static void __acpigen_write_method(const char *name, uint8_t flags)
{
	acpigen_emit_byte(METHOD_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
	acpigen_emit_byte(flags);
}

/* Method (name, nargs, NotSerialized) */
void acpigen_write_method(const char *name, int nargs)
{
	__acpigen_write_method(name, (nargs & 7));
}

/* Method (name, nargs, Serialized) */
void acpigen_write_method_serialized(const char *name, int nargs)
{
	__acpigen_write_method(name, (nargs & 7) | (1 << 3));
}

void acpigen_write_device(const char *name)
{
	acpigen_emit_ext_op(DEVICE_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
}

void acpigen_write_thermal_zone(const char *name)
{
	acpigen_emit_ext_op(THERMAL_ZONE_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
}

void acpigen_write_STA(uint8_t status)
{
	/*
	 * Method (_STA, 0, NotSerialized) { Return (status) }
	 */
	acpigen_write_method("_STA", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(status);
	acpigen_pop_len();
}

void acpigen_write_STA_ext(const char *namestring)
{
	/*
	 * Method (_STA, 0, NotSerialized) { Return (ext_val) }
	 */
	acpigen_write_method("_STA", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(namestring);
	acpigen_pop_len();
}

void acpigen_write_BBN(uint8_t base_bus_number)
{
	/*
	 * Method (_BBN, 0, NotSerialized) { Return (status) }
	 */
	acpigen_write_method("_BBN", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(base_bus_number);
	acpigen_pop_len();
}

void acpigen_write_SEG(uint8_t segment_group_number)
{
	/*
	 * Method (_SEG, 0, NotSerialized) { Return (status) }
	 */
	acpigen_write_method("_SEG", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(segment_group_number);
	acpigen_pop_len();
}

void acpigen_write_LPI_package(uint64_t level, const struct acpi_lpi_state *states, uint16_t nentries)
{
	/*
	* Name (_LPI, Package (0x06)  // _LPI: Low Power Idle States
	* {
	*     0x0000,
	*     0x0000000000000000,
	*     0x0003,
	*     Package (0x0A)
	*     {
	*         0x00000002,
	*         0x00000001,
	*         0x00000001,
	*         0x00000000,
	*         0x00000000,
	*         0x00000000,
	*         ResourceTemplate ()
	*         {
	*             Register (FFixedHW,
	*                 0x02,               // Bit Width
	*                 0x02,               // Bit Offset
	*                 0x0000000000000000, // Address
	*                 ,)
	*         },
	*
	*        ResourceTemplate ()
	*        {
	*            Register (SystemMemory,
	*                0x00,               // Bit Width
	*                0x00,               // Bit Offset
	*                0x0000000000000000, // Address
	*                ,)
	*        },
	*
	*        ResourceTemplate ()
	*        {
	*            Register (SystemMemory,
	*                0x00,               // Bit Width
	*                0x00,               // Bit Offset
	*                0x0000000000000000, // Address
	*                ,)
	*        },
	*
	*        "C1"
	*    },
	*    ...
	* }
	*/

	acpigen_write_name("_LPI");
	acpigen_write_package(3 + nentries);
	acpigen_write_word(0); /* Revision */
	acpigen_write_qword(level);
	acpigen_write_word(nentries);

	for (size_t i = 0; i < nentries; i++, states++) {
		acpigen_write_package(0xA);
		acpigen_write_dword(states->min_residency_us);
		acpigen_write_dword(states->worst_case_wakeup_latency_us);
		acpigen_write_dword(states->flags);
		acpigen_write_dword(states->arch_context_lost_flags);
		acpigen_write_dword(states->residency_counter_frequency_hz);
		acpigen_write_dword(states->enabled_parent_state);
		acpigen_write_register_resource(&states->entry_method);
		acpigen_write_register_resource(&states->residency_counter_register);
		acpigen_write_register_resource(&states->usage_counter_register);
		acpigen_write_string(states->state_name);
		acpigen_pop_len();
	}
	acpigen_pop_len();
}

/*
 * Generates a func with max supported P-states.
 */
void acpigen_write_PPC(uint8_t nr)
{
/*
	Method (_PPC, 0, NotSerialized)
	{
		Return (nr)
	}
*/
	acpigen_write_method("_PPC", 0);
	acpigen_emit_byte(RETURN_OP);
	/* arg */
	acpigen_write_byte(nr);
	acpigen_pop_len();
}

/*
 * Generates a func with max supported P-states saved
 * in the variable PPCM.
 */
void acpigen_write_PPC_NVS(void)
{
/*
	Method (_PPC, 0, NotSerialized)
	{
		Return (PPCM)
	}
*/
	acpigen_write_method("_PPC", 0);
	acpigen_emit_byte(RETURN_OP);
	/* arg */
	acpigen_emit_namestring("PPCM");
	acpigen_pop_len();
}

void acpigen_write_TPC(const char *gnvs_tpc_limit)
{
/*
	// Sample _TPC method
	Method (_TPC, 0, NotSerialized)
	{
		Return (\TLVL)
	}
*/
	acpigen_write_method("_TPC", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(gnvs_tpc_limit);
	acpigen_pop_len();
}

void acpigen_write_PRW(uint32_t wake, uint32_t level)
{
	/*
	 * Name (_PRW, Package () { wake, level }
	 */
	acpigen_write_name("_PRW");
	acpigen_write_package(2);
	acpigen_write_integer(wake);
	acpigen_write_integer(level);
	acpigen_pop_len();
}

void acpigen_write_PSS_package(uint32_t coreFreq, uint32_t power, uint32_t transLat, uint32_t busmLat, uint32_t control,
			       uint32_t status)
{
	acpigen_write_package(6);
	acpigen_write_dword(coreFreq);
	acpigen_write_dword(power);
	acpigen_write_dword(transLat);
	acpigen_write_dword(busmLat);
	acpigen_write_dword(control);
	acpigen_write_dword(status);
	acpigen_pop_len();

	con_dbg("PSS: %uMHz power %u control 0x%x status 0x%x\n", coreFreq, power,
	       control, status);
}

void acpigen_write_pss_object(const struct acpi_sw_pstate *pstate_values, size_t nentries)
{
	size_t pstate;

	acpigen_write_name("_PSS");
	acpigen_write_package(nentries);
	for (pstate = 0; pstate < nentries; pstate++) {
		acpigen_write_PSS_package(
			pstate_values->core_freq, pstate_values->power,
			pstate_values->transition_latency, pstate_values->bus_master_latency,
			pstate_values->control_value, pstate_values->status_value);
		pstate_values++;
	}

	acpigen_pop_len();
}

void acpigen_write_PSD_package(uint32_t domain, uint32_t numprocs, PSD_coord coordtype)
{
	acpigen_write_name("_PSD");
	acpigen_write_package(1);
	acpigen_write_package(5);
	acpigen_write_byte(5);	// 5 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_pop_len();
	acpigen_pop_len();
}

void acpigen_write_CST_package_entry(const acpi_cstate_t *cstate)
{
	acpigen_write_package(4);
	acpigen_write_register_resource(&cstate->resource);
	acpigen_write_byte(cstate->ctype);
	acpigen_write_word(cstate->latency);
	acpigen_write_dword(cstate->power);
	acpigen_pop_len();
}

void acpigen_write_CST_package(const acpi_cstate_t *cstate, int nentries)
{
	int i;
	acpigen_write_name("_CST");
	acpigen_write_package(nentries+1);
	acpigen_write_integer(nentries);

	for (i = 0; i < nentries; i++)
		acpigen_write_CST_package_entry(cstate + i);

	acpigen_pop_len();
}

void acpigen_write_CSD_package(uint32_t domain, uint32_t numprocs, CSD_coord coordtype,
	uint32_t index)
{
	acpigen_write_name("_CSD");
	acpigen_write_package(1);
	acpigen_write_package(6);
	acpigen_write_integer(6);	// 6 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_write_dword(index);
	acpigen_pop_len();
	acpigen_pop_len();
}

void acpigen_write_TSS_package(int entries, acpi_tstate_t *tstate_list)
{
/*
	Sample _TSS package with 100% and 50% duty cycles
	Name (_TSS, Package (0x02)
	{
		Package(){100, 1000, 0, 0x00, 0)
		Package(){50, 520, 0, 0x18, 0)
	})
*/
	int i;
	acpi_tstate_t *tstate = tstate_list;

	acpigen_write_name("_TSS");
	acpigen_write_package(entries);

	for (i = 0; i < entries; i++) {
		acpigen_write_package(5);
		acpigen_write_dword(tstate->percent);
		acpigen_write_dword(tstate->power);
		acpigen_write_dword(tstate->latency);
		acpigen_write_dword(tstate->control);
		acpigen_write_dword(tstate->status);
		acpigen_pop_len();
		tstate++;
	}

	acpigen_pop_len();
}

void acpigen_write_TSD_package(uint32_t domain, uint32_t numprocs, PSD_coord coordtype)
{
	acpigen_write_name("_TSD");
	acpigen_write_package(1);
	acpigen_write_package(5);
	acpigen_write_byte(5);	// 5 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_pop_len();
	acpigen_pop_len();
}

void acpigen_write_mem32fixed(int readwrite, uint32_t base, uint32_t size)
{
	/*
	 * ACPI 4.0 section 6.4.3.4: 32-Bit Fixed Memory Range Descriptor
	 * Byte 0:
	 *   Bit7  : 1 => big item
	 *   Bit6-0: 0000110 (0x6) => 32-bit fixed memory
	 */
	acpigen_emit_byte(0x86);
	/* Byte 1+2: length (0x0009) */
	acpigen_emit_byte(0x09);
	acpigen_emit_byte(0x00);
	/* bit1-7 are ignored */
	acpigen_emit_byte(readwrite ? 0x01 : 0x00);
	acpigen_emit_dword(base);
	acpigen_emit_dword(size);
}

static void acpigen_write_register(const acpi_addr_t *addr)
{
	acpigen_emit_byte(0x82);		/* Register Descriptor */
	acpigen_emit_byte(0x0c);		/* Register Length 7:0 */
	acpigen_emit_byte(0x00);		/* Register Length 15:8 */
	acpigen_emit_byte(addr->space_id);	/* Address Space ID */
	acpigen_emit_byte(addr->bit_width);	/* Register Bit Width */
	acpigen_emit_byte(addr->bit_offset);	/* Register Bit Offset */
	acpigen_emit_byte(addr->access_size);	/* Register Access Size */
	acpigen_emit_dword(addr->addrl);	/* Register Address Low */
	acpigen_emit_dword(addr->addrh);	/* Register Address High */
}

void acpigen_write_register_resource(const acpi_addr_t *addr)
{
	acpigen_write_resourcetemplate_header();
	acpigen_write_register(addr);
	acpigen_write_resourcetemplate_footer();
}

void acpigen_write_irq(uint16_t mask)
{
	/*
	 * ACPI 3.0b section 6.4.2.1: IRQ Descriptor
	 * Byte 0:
	 *   Bit7  : 0 => small item
	 *   Bit6-3: 0100 (0x4) => IRQ port descriptor
	 *   Bit2-0: 010 (0x2) => 2 Bytes long
	 */
	acpigen_emit_byte(0x22);
	acpigen_emit_byte(mask & 0xff);
	acpigen_emit_byte((mask >> 8) & 0xff);
}

void acpigen_write_io16(uint16_t min, uint16_t max, uint8_t align, uint8_t len, uint8_t decode16)
{
	/*
	 * ACPI 4.0 section 6.4.2.6: I/O Port Descriptor
	 * Byte 0:
	 *   Bit7  : 0 => small item
	 *   Bit6-3: 1000 (0x8) => I/O port descriptor
	 *   Bit2-0: 111 (0x7) => 7 Bytes long
	 */
	acpigen_emit_byte(0x47);
	/* Does the device decode all 16 or just 10 bits? */
	/* bit1-7 are ignored */
	acpigen_emit_byte(decode16 ? 0x01 : 0x00);
	/* minimum base address the device may be configured for */
	acpigen_emit_byte(min & 0xff);
	acpigen_emit_byte((min >> 8) & 0xff);
	/* maximum base address the device may be configured for */
	acpigen_emit_byte(max & 0xff);
	acpigen_emit_byte((max >> 8) & 0xff);
	/* alignment for min base */
	acpigen_emit_byte(align & 0xff);
	acpigen_emit_byte(len & 0xff);
}

void acpigen_write_resourcetemplate_header(void)
{
	/*
	 * A ResourceTemplate() is a Buffer() with a
	 * (Byte|Word|DWord) containing the length, followed by one or more
	 * resource items, terminated by the end tag.
	 * (small item 0xf, len 1)
	 */
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(WORD_PREFIX);
	len_stack[ltop++] = acpigen_get_current();
	/* Add 2 dummy bytes for the ACPI word (keep aligned with
	   the calculation in acpigen_write_resourcetemplate() below). */
	acpigen_emit_byte(0x00);
	acpigen_emit_byte(0x00);
}

void acpigen_write_resourcetemplate_footer(void)
{
	char *p = len_stack[--ltop];
	int len;
	/*
	 * end tag (acpi 4.0 Section 6.4.2.8)
	 * 0x79 <checksum>
	 * 0x00 is treated as a good checksum according to the spec
	 * and is what iasl generates.
	 */
	acpigen_emit_byte(0x79);
	acpigen_emit_byte(0x00);

	/* Start counting past the 2-bytes length added in
	   acpigen_write_resourcetemplate() above. */
	len = acpigen_get_current() - (p + 2);

	/* patch len word */
	p[0] = len & 0xff;
	p[1] = (len >> 8) & 0xff;
	/* patch len field */
	acpigen_pop_len();
}

#ifdef CONFIG_ACPI_GEN_MAINBOARD_RESOURCE
static void acpigen_add_mainboard_rsvd_mem32(void *gp, struct device *dev, struct resource *res)
{
	acpigen_write_mem32fixed(0, res->base, res->size);
}

static void acpigen_add_mainboard_rsvd_io(void *gp, struct device *dev, struct resource *res)
{
	resource_t base = res->base;
	resource_t size = res->size;
	while (size > 0) {
		resource_t sz = size > 255 ? 255 : size;
		acpigen_write_io16(base, base, 0, sz, 1);
		size -= sz;
		base += sz;
	}
}

void acpigen_write_mainboard_resource_template(void)
{
	acpigen_write_resourcetemplate_header();

	/* Add reserved memory ranges. */
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_RESERVE,
		IORESOURCE_MEM | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_mem32, 0);

	/* Add reserved io ranges. */
	search_global_resources(
		IORESOURCE_IO | IORESOURCE_RESERVE,
		IORESOURCE_IO | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_io, 0);

	acpigen_write_resourcetemplate_footer();
}

void acpigen_write_mainboard_resources(const char *scope, const char *name)
{
	acpigen_write_scope(scope);
	acpigen_write_name(name);
	acpigen_write_mainboard_resource_template();
	acpigen_pop_len();
}
#endif

static int hex2bin(const char c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return c - '0';
}

void acpigen_emit_eisaid(const char *eisaid)
{
	uint32_t compact = 0;

	/* Clamping individual values would be better but
	   there is a disagreement over what is a valid
	   EISA id, so accept anything and don't clamp,
	   parent code should create a valid EISAid.
	 */
	compact |= (eisaid[0] - 'A' + 1) << 26;
	compact |= (eisaid[1] - 'A' + 1) << 21;
	compact |= (eisaid[2] - 'A' + 1) << 16;
	compact |= hex2bin(eisaid[3]) << 12;
	compact |= hex2bin(eisaid[4]) << 8;
	compact |= hex2bin(eisaid[5]) << 4;
	compact |= hex2bin(eisaid[6]);

	acpigen_emit_byte(0xc);
	acpigen_emit_byte((compact >> 24) & 0xff);
	acpigen_emit_byte((compact >> 16) & 0xff);
	acpigen_emit_byte((compact >> 8) & 0xff);
	acpigen_emit_byte(compact & 0xff);
}

/*
 * ToUUID(uuid)
 *
 * ACPI 6.1 Section 19.6.136 table 19-385 defines a special output
 * order for the bytes that make up a UUID Buffer object.
 * UUID byte order for input:
 *   aabbccdd-eeff-gghh-iijj-kkllmmnnoopp
 * UUID byte order for output:
 *   ddccbbaa-ffee-hhgg-iijj-kkllmmnnoopp
 */
#define UUID_LEN 16
void acpigen_write_uuid(const char *uuid)
{
	uint8_t buf[UUID_LEN];
	size_t i, order[UUID_LEN] = { 3, 2, 1, 0, 5, 4, 7, 6,
				      8, 9, 10, 11, 12, 13, 14, 15 };

	/* Parse UUID string into bytes */
	if (hexstrtobin(uuid, buf, UUID_LEN) < UUID_LEN)
		return;

	/* BufferOp */
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();

	/* Buffer length in bytes */
	acpigen_write_word(UUID_LEN);

	/* Output UUID in expected order */
	for (i = 0; i < UUID_LEN; i++)
		acpigen_emit_byte(buf[order[i]]);

	acpigen_pop_len();
}

/*
 * Name (_PRx, Package(One) { name })
 * ...
 * PowerResource (name, level, order)
 */
void acpigen_write_power_res(const char *name, uint8_t level, uint16_t order,
			     const char * const dev_states[], size_t dev_states_count)
{
	size_t i;
	for (i = 0; i < dev_states_count; i++) {
		acpigen_write_name(dev_states[i]);
		acpigen_write_package(1);
		acpigen_emit_simple_namestring(name);
		acpigen_pop_len();		/* Package */
	}

	acpigen_emit_ext_op(POWER_RES_OP);

	acpigen_write_len_f();

	acpigen_emit_simple_namestring(name);
	acpigen_emit_byte(level);
	acpigen_emit_word(order);
}

/* Sleep (ms) */
void acpigen_write_sleep(uint64_t sleep_ms)
{
	acpigen_emit_ext_op(SLEEP_OP);
	acpigen_write_integer(sleep_ms);
}

void acpigen_write_store(void)
{
	acpigen_emit_byte(STORE_OP);
}

/* Store (src, dst) */
void acpigen_write_store_ops(uint8_t src, uint8_t dst)
{
	acpigen_write_store();
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

/* Store (src, "namestr") */
void acpigen_write_store_op_to_namestr(uint8_t src, const char *dst)
{
	acpigen_write_store();
	acpigen_emit_byte(src);
	acpigen_emit_namestring(dst);
}

/* Store (src, "namestr") */
void acpigen_write_store_int_to_namestr(uint64_t src, const char *dst)
{
	acpigen_write_store();
	acpigen_write_integer(src);
	acpigen_emit_namestring(dst);
}

/* Store ("namestr", dst) */
void acpigen_write_store_namestr_to_op(const char *src, uint8_t dst)
{
	acpigen_write_store();
	acpigen_emit_namestring(src);
	acpigen_emit_byte(dst);
}

/* Store (src, dst) */
void acpigen_write_store_int_to_op(uint64_t src, uint8_t dst)
{
	acpigen_write_store();
	acpigen_write_integer(src);
	acpigen_emit_byte(dst);
}

/* Store ("namestr", "namestr") */
void acpigen_write_store_namestr_to_namestr(const char *src, const char *dst)
{
	acpigen_write_store();
	acpigen_emit_namestring(src);
	acpigen_emit_namestring(dst);
}

/* Or (arg1, arg2, res) */
void acpigen_write_or(uint8_t arg1, uint8_t arg2, uint8_t res)
{
	acpigen_emit_byte(OR_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
	acpigen_emit_byte(res);
}

/* Xor (arg1, arg2, res) */
void acpigen_write_xor(uint8_t arg1, uint8_t arg2, uint8_t res)
{
	acpigen_emit_byte(XOR_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
	acpigen_emit_byte(res);
}

/* And (arg1, arg2, res) */
void acpigen_write_and(uint8_t arg1, uint8_t arg2, uint8_t res)
{
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
	acpigen_emit_byte(res);
}

/* Not (arg, res) */
void acpigen_write_not(uint8_t arg, uint8_t res)
{
	acpigen_emit_byte(NOT_OP);
	acpigen_emit_byte(arg);
	acpigen_emit_byte(res);
}

/* Concatenate (str, src_res, dest_res) */
void acpigen_concatenate_string_op(const char *str, uint8_t src_res, uint8_t dest_res)
{
	acpigen_emit_byte(CONCATENATE_OP);
	acpigen_write_string(str);
	acpigen_emit_byte(src_res);
	acpigen_emit_byte(dest_res);
}

/* Store (str, DEBUG) */
void acpigen_write_debug_string(const char *str)
{
	acpigen_write_store();
	acpigen_write_string(str);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Store (val, DEBUG) */
void acpigen_write_debug_integer(uint64_t val)
{
	acpigen_write_store();
	acpigen_write_integer(val);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Store (op, DEBUG) */
void acpigen_write_debug_op(uint8_t op)
{
	acpigen_write_store();
	acpigen_emit_byte(op);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Store (str, DEBUG) */
void acpigen_write_debug_namestr(const char *str)
{
	acpigen_write_store();
	acpigen_emit_namestring(str);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Concatenate (str1, res, tmp_res)
   Store(tmp_res, DEBUG) */
void acpigen_write_debug_concatenate_string_op(const char *str, uint8_t res,
	uint8_t tmp_res)
{
	acpigen_concatenate_string_op(str, res, tmp_res);
	acpigen_write_debug_op(tmp_res);
}

static void acpigen_tx_byte(unsigned char byte, void *data)
{
	acpigen_emit_byte(byte);
}

/* Store("formatted string", DEBUG) */
void acpigen_write_debug_sprintf(const char *fmt, ...)
{
	va_list args;

	acpigen_write_store();

	acpigen_emit_byte(STRING_PREFIX);
	va_start(args, fmt);
	vtxprintf(acpigen_tx_byte, fmt, args, NULL);
	va_end(args);
	acpigen_emit_byte('\0');

	acpigen_emit_ext_op(DEBUG_OP);
}

void acpigen_write_if(void)
{
	acpigen_emit_byte(IF_OP);
	acpigen_write_len_f();
}

/* If (And (arg1, arg2)) */
void acpigen_write_if_and(uint8_t arg1, uint8_t arg2)
{
	acpigen_write_if();
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are equal.
 * Both operand1 and operand2 are ACPI ops.
 *
 * If (Lequal (op1, op2))
 */
void acpigen_write_if_lequal_op_op(uint8_t op1, uint8_t op2)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(op1);
	acpigen_emit_byte(op2);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are not equal.
 * Both operand1 and operand2 are ACPI ops.
 *
 * If (Lnotequal (op1, op2))
 *
 * This is equivalent to
 *
 * If (Lnot (Lequal (op1, op2)))
 */
void acpigen_write_if_lnotequal_op_op(uint8_t op1, uint8_t op2)
{
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(op1);
	acpigen_emit_byte(op2);
}

/*
 * Generates ACPI code for checking if operand1 is greater than operand2.
 * Both operand1 and operand2 are ACPI ops.
 *
 * If (Lgreater (op1 op2))
 */
void acpigen_write_if_lgreater_op_op(uint8_t op1, uint8_t op2)
{
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(op1);
	acpigen_emit_byte(op2);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are equal, where,
 * operand1 is ACPI op and operand2 is an integer.
 *
 * If (Lequal (op, val))
 */
void acpigen_write_if_lequal_op_int(uint8_t op, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(op);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are not equal, where,
 * operand1 is ACPI op and operand2 is an integer.
 *
 * If (Lnotequal (op, val))
 *
 * This is equivalent to
 *
 * If (Lnot (Lequal (op, val)))
 */
void acpigen_write_if_lnotequal_op_int(uint8_t op, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(op);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code for checking if operand is greater than the value, where,
 * operand is ACPI op and val is an integer.
 *
 * If (Lgreater (op, val))
 */
void acpigen_write_if_lgreater_op_int(uint8_t op, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(op);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are equal, where,
 * operand1 is namestring and operand2 is an integer.
 *
 * If (Lequal ("namestr", val))
 */
void acpigen_write_if_lequal_namestr_int(const char *namestr, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring(namestr);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are not equal, where,
 * operand1 is namestring and operand2 is an integer.
 *
 * If (Lnotequal ("namestr", val))
 *
 * * This is equivalent to
 *
 * If (Lnot (Lequal ("namestr", val)))
 */
void acpigen_write_if_lnotequal_namestr_int(const char *namestr, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring(namestr);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code for checking if operand1 and operand2 are equal, where,
 * operand1 is namestring and operand2 is an integer.
 *
 * If (Lgreater ("namestr", val))
 */
void acpigen_write_if_lgreater_namestr_int(const char *namestr, uint64_t val)
{
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_namestring(namestr);
	acpigen_write_integer(val);
}

/*
 * Generates ACPI code to check at runtime if an object named `namestring`
 * exists, and leaves the If scope open to continue execute code when this
 * is true. NOTE: Requires matching acpigen_write_if_end().
 *
 * If (CondRefOf (NAME))
 */
void acpigen_write_if_cond_ref_of(const char *namestring)
{
	acpigen_write_if();
	acpigen_emit_ext_op(COND_REFOF_OP);
	acpigen_emit_namestring(namestring);
	acpigen_emit_byte(ZERO_OP); /* ignore COND_REFOF_OP destination */
}

/* Closes previously opened if statement and generates ACPI code for else statement. */
void acpigen_write_else(void)
{
	acpigen_pop_len();
	acpigen_emit_byte(ELSE_OP);
	acpigen_write_len_f();
}

void acpigen_write_shiftleft_op_int(uint8_t src_result, uint64_t count)
{
	acpigen_emit_byte(SHIFT_LEFT_OP);
	acpigen_emit_byte(src_result);
	acpigen_write_integer(count);
	acpigen_emit_byte(ZERO_OP);
}

void acpigen_write_to_buffer(uint8_t src, uint8_t dst)
{
	acpigen_emit_byte(TO_BUFFER_OP);
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

void acpigen_write_to_integer(uint8_t src, uint8_t dst)
{
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

void acpigen_write_to_integer_from_namestring(const char *source, uint8_t dst_op)
{
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_namestring(source);
	acpigen_emit_byte(dst_op);
}

/* The initializer byte array 'arr' is optional. When 'arr' is NULL, the AML interpreter will
   create a 0-initialized byte buffer */
void acpigen_write_byte_buffer(uint8_t *arr, size_t size)
{
	size_t i;

	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_write_integer(size);

	if (arr != NULL) {
		for (i = 0; i < size; i++)
			acpigen_emit_byte(arr[i]);
	}

	acpigen_pop_len();
}

void acpigen_write_return_byte_buffer(uint8_t *arr, size_t size)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte_buffer(arr, size);
}

void acpigen_write_return_singleton_buffer(uint8_t arg)
{
	acpigen_write_return_byte_buffer(&arg, 1);
}

void acpigen_write_return_op(uint8_t arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_byte(arg);
}

void acpigen_write_return_byte(uint8_t arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(arg);
}

void acpigen_write_return_integer(uint64_t arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_integer(arg);
}

void acpigen_write_return_namestr(const char *arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(arg);
}

void acpigen_write_return_string(const char *arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_string(arg);
}

void acpigen_write_upc(enum acpi_upc_type type)
{
	acpigen_write_name("_UPC");
	acpigen_write_package(4);
	/* Connectable */
	acpigen_write_byte(type == UPC_TYPE_UNUSED ? 0 : 0xff);
	/* Type */
	acpigen_write_byte(type);
	/* Reserved0 */
	acpigen_write_zero();
	/* Reserved1 */
	acpigen_write_zero();
	acpigen_pop_len();
}

void acpigen_write_pld(const struct acpi_pld *pld)
{
	uint8_t buf[20];

	if (acpi_pld_to_buffer(pld, buf, ARRAY_SIZE(buf)) < 0)
		return;

	acpigen_write_name("_PLD");
	acpigen_write_package(1);
	acpigen_write_byte_buffer(buf, ARRAY_SIZE(buf));
	acpigen_pop_len();
}

void acpigen_write_dsm(const char *uuid, void (**callbacks)(void *), size_t count, void *arg)
{
	struct dsm_uuid id = DSM_UUID(uuid, callbacks, count, arg);
	acpigen_write_dsm_uuid_arr(&id, 1);
}

/*
 * Create a supported functions bitmask
 * bit 0:    other functions than 0 are supported
 * bits 1-x: function x supported
 */
/* On GCC aarch64 the compiler is worried about alloca() having unbounded stack usage. */
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif
static void acpigen_dsm_uuid_enum_functions(const struct dsm_uuid *id)
{
	const size_t bytes = DIV_ROUND_UP(id->count, BITS_PER_BYTE);
	uint8_t *buffer = (uint8_t *)heap_alloc(bytes);
	bool set = false;
	size_t cb_idx = 0;

	memset((void *)buffer, 0, bytes);

	for (size_t i = 0; i < bytes; i++) {
		for (size_t j = 0; j < BITS_PER_BYTE; j++) {
			if (cb_idx >= id->count)
				break;

			if (id->callbacks[cb_idx++]) {
				set = true;
				buffer[i] |= BIT(j);
			}
		}
	}

	if (set)
		buffer[0] |= BIT(0);

	acpigen_write_return_byte_buffer(buffer, bytes);
}

static void acpigen_write_dsm_uuid(struct dsm_uuid *id)
{
	size_t i;

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(id->uuid);

	/* ToInteger (Arg2, Local1) */
	acpigen_write_to_integer(ARG2_OP, LOCAL1_OP);

	/* If (LEqual(Local1, 0)) */
	{
		acpigen_write_if_lequal_op_int(LOCAL1_OP, 0);
		if (id->callbacks[0])
			id->callbacks[0](id->arg);
		else if (id->count)
			acpigen_dsm_uuid_enum_functions(id);
		acpigen_write_if_end();
	}

	for (i = 1; i < id->count; i++) {
		/* If (LEqual (Local1, i)) */
		acpigen_write_if_lequal_op_int(LOCAL1_OP, i);

		/* Callback to write if handler. */
		if (id->callbacks[i])
			id->callbacks[i](id->arg);

		acpigen_write_if_end();	/* If */
	}

	/* Default case: Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);

	acpigen_write_if_end(); /* If (LEqual (Local0, ToUUID(uuid))) */
}

/*
 * Generate ACPI AML code for _DSM method.
 * This function takes as input array of uuid for the device, set of callbacks
 * and argument to pass into the callbacks. Callbacks should ensure that Local0
 * and Local1 are left untouched. Use of Local2-Local7 is permitted in
 * callbacks.
 *
 * Arguments passed into _DSM method:
 * Arg0 = UUID
 * Arg1 = Revision
 * Arg2 = Function index
 * Arg3 = Function specific arguments
 *
 * AML code generated would look like:
 * Method (_DSM, 4, Serialized) {
 *	ToBuffer (Arg0, Local0)
 *	If (LEqual (Local0, ToUUID(uuid))) {
 *		ToInteger (Arg2, Local1)
 *		If (LEqual (Local1, 0)) {
 *			<acpigen by callback[0]>
 *		}
 *		...
 *		If (LEqual (Local1, n)) {
 *			<acpigen by callback[n]>
 *		}
 *		Return (Buffer (One) { 0x0 })
 *	}
 *	...
 *	If (LEqual (Local0, ToUUID(uuidn))) {
 *	...
 *	}
 *	Return (Buffer (One) { 0x0 })
 * }
 */
void acpigen_write_dsm_uuid_arr(struct dsm_uuid *ids, size_t count)
{
	size_t i;

	/* Method (_DSM, 4, Serialized) */
	acpigen_write_method_serialized("_DSM", 0x4);

	/* ToBuffer (Arg0, Local0) */
	acpigen_write_to_buffer(ARG0_OP, LOCAL0_OP);

	for (i = 0; i < count; i++)
		acpigen_write_dsm_uuid(&ids[i]);

	/* Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);

	acpigen_pop_len();	/* Method _DSM */
}

void acpigen_write_CPPC_package(const struct cppc_config *config)
{
	uint32_t i;
	uint32_t max;
	switch (config->version) {
	case 1:
		max = CPPC_MAX_FIELDS_VER_1;
		break;
	case 2:
		max = CPPC_MAX_FIELDS_VER_2;
		break;
	case 3:
		max = CPPC_MAX_FIELDS_VER_3;
		break;
	default:
		con_err("CPPC version %u is not implemented\n", config->version);
		return;
	}
	acpigen_write_name(CPPC_PACKAGE_NAME);

	/* Adding 2 to account for length and version fields */
	acpigen_write_package(max + 2);
	acpigen_write_dword(max + 2);

	acpigen_write_byte(config->version);

	for (i = 0; i < max; ++i) {
		const cppc_entry_t *entry = &config->entries[i];
		if (entry->type == CPPC_TYPE_DWORD)
			acpigen_write_dword(entry->dword);
		else
			acpigen_write_register_resource(&entry->reg);
	}
	acpigen_pop_len();
}

void acpigen_write_CPPC_method(void)
{
	char pscope[17];
	snprintf(pscope, sizeof(pscope),
		 "\\_SB." CONFIG_ACPI_CPU_STRING "." CPPC_PACKAGE_NAME, (cpu_t)0);

	acpigen_write_method("_CPC", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(pscope);
	acpigen_pop_len();
}

/*
 * Generate ACPI AML code for _ROM method.
 * This function takes as input ROM data and ROM length.
 *
 * The ACPI spec isn't clear about what should happen at the end of the
 * ROM. Tests showed that it shouldn't truncate, but fill the remaining
 * bytes in the returned buffer with zeros.
 *
 * Arguments passed into _DSM method:
 * Arg0 = Offset in Bytes
 * Arg1 = Bytes to return
 *
 * Example:
 *   acpigen_write_rom(0xdeadbeef, 0x10000)
 *
 * AML code generated would look like:
 * Method (_ROM, 2, NotSerialized) {
 *
 *	OperationRegion("ROMS", SYSTEMMEMORY, 0xdeadbeef, 0x10000)
 *	Field (ROMS, AnyAcc, NoLock, Preserve)
 *	{
 *		Offset (0),
 *		RBF0,   0x80000
 *	}
 *
 *	Store (Arg0, Local0)
 *	Store (Arg1, Local1)
 *
 *	If (LGreater (Local1, 0x1000))
 *	{
 *		Store (0x1000, Local1)
 *	}
 *
 *	Store (Local1, Local3)
 *
 *	If (LGreater (Local0, 0x10000))
 *	{
 *		Return(Buffer(Local1){0})
 *	}
 *
 *	If (LGreater (Local0, 0x0f000))
 *	{
 *		Subtract (0x10000, Local0, Local2)
 *		If (LGreater (Local1, Local2))
 *		{
 *			Store (Local2, Local1)
 *		}
 *	}
 *
 *	Name (ROM1, Buffer (Local3) {0})
 *
 *	Multiply (Local0, 0x08, Local0)
 *	Multiply (Local1, 0x08, Local1)
 *
 *	CreateField (RBF0, Local0, Local1, TMPB)
 *	Store (TMPB, ROM1)
 *	Return (ROM1)
 * }
 */

void acpigen_write_rom(void *bios, const size_t length)
{
	BUG_ON(!bios);
	BUG_ON(!length);

	/* Method (_ROM, 2, Serialized) */
	acpigen_write_method_serialized("_ROM", 2);

	/* OperationRegion("ROMS", SYSTEMMEMORY, current, length) */
	struct opregion opreg = OPREGION("ROMS", SYSTEMMEMORY, (uintptr_t)bios, length);
	acpigen_write_opregion(&opreg);

	struct fieldlist l[] = {
		FIELDLIST_OFFSET(0),
		FIELDLIST_NAMESTR("RBF0", 8 * length),
	};

	/* Field (ROMS, AnyAcc, NoLock, Preserve)
	 * {
	 *  Offset (0),
	 *  RBF0,   0x80000
	 * } */
	acpigen_write_field(opreg.name, l, 2, FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/* Store (Arg0, Local0) */
	acpigen_write_store_ops(ARG0_OP, LOCAL0_OP);

	/* Store (Arg1, Local1) */
	acpigen_write_store_ops(ARG1_OP, LOCAL1_OP);

	/* ACPI SPEC requires to return at maximum 4KiB */
	/* If (LGreater (Local1, 0x1000)) */
	acpigen_write_if_lgreater_op_int(LOCAL1_OP, 0x1000);

	/* Store (0x1000, Local1) */
	acpigen_write_store_int_to_op(0x1000, LOCAL1_OP);

	/* Pop if */
	acpigen_pop_len();

	/* Store (Local1, Local3) */
	acpigen_write_store_ops(LOCAL1_OP, LOCAL3_OP);

	/* If (LGreater (Local0, length)) */
	acpigen_write_if_lgreater_op_int(LOCAL0_OP, length);

	/* Return(Buffer(Local1){0}) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(0);
	acpigen_pop_len();

	/* Pop if */
	acpigen_pop_len();

	/* If (LGreater (Local0, length - 4096)) */
	acpigen_write_if_lgreater_op_int(LOCAL0_OP, length - 4096);

	/* Subtract (length, Local0, Local2) */
	acpigen_emit_byte(SUBTRACT_OP);
	acpigen_write_integer(length);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(LOCAL2_OP);

	/* If (LGreater (Local1, Local2)) */
	acpigen_write_if_lgreater_op_op(LOCAL1_OP, LOCAL2_OP);

	/* Store (Local2, Local1) */
	acpigen_write_store_ops(LOCAL2_OP, LOCAL1_OP);

	/* Pop if */
	acpigen_pop_len();

	/* Pop if */
	acpigen_pop_len();

	/* Name (ROM1, Buffer (Local3) {0}) */
	acpigen_write_name("ROM1");
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LOCAL3_OP);
	acpigen_emit_byte(0);
	acpigen_pop_len();

	/* Multiply (Local1, 0x08, Local1) */
	acpigen_emit_byte(MULTIPLY_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_integer(0x08);
	acpigen_emit_byte(LOCAL1_OP);

	/* Multiply (Local0, 0x08, Local0) */
	acpigen_emit_byte(MULTIPLY_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(0x08);
	acpigen_emit_byte(LOCAL0_OP);

	/* CreateField (RBF0, Local0, Local1, TMPB) */
	acpigen_emit_ext_op(CREATEFIELD_OP);
	acpigen_emit_namestring("RBF0");
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_namestring("TMPB");

	/* Store (TMPB, ROM1) */
	acpigen_write_store_namestr_to_namestr("TMPB", "ROM1");

	/* Return (ROM1) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("ROM1");

	/* Pop method */
	acpigen_pop_len();
}

#ifdef CONFIG_ACPI_GPIO
/*
 * Helper functions for enabling/disabling Tx GPIOs based on the GPIO
 * polarity. These functions end up calling acpigen_soc_{set,clear}_tx_gpio to
 * make callbacks into SoC acpigen code.
 *
 * Returns 0 on success and -1 on error.
 */
int acpigen_enable_tx_gpio(const struct acpi_gpio *gpio)
{
	if (gpio->active_low)
		return acpigen_soc_clear_tx_gpio(gpio->pins[0]);
	else
		return acpigen_soc_set_tx_gpio(gpio->pins[0]);
}

int acpigen_disable_tx_gpio(const struct acpi_gpio *gpio)
{
	if (gpio->active_low)
		return acpigen_soc_set_tx_gpio(gpio->pins[0]);
	else
		return acpigen_soc_clear_tx_gpio(gpio->pins[0]);
}

void acpigen_get_rx_gpio(const struct acpi_gpio *gpio)
{
	acpigen_soc_read_rx_gpio(gpio->pins[0]);

	if (gpio->active_low)
		acpigen_write_xor(LOCAL0_OP, 1, LOCAL0_OP);
}

void acpigen_get_tx_gpio(const struct acpi_gpio *gpio)
{
	acpigen_soc_get_tx_gpio(gpio->pins[0]);

	if (gpio->active_low)
		acpigen_write_xor(LOCAL0_OP, 1, LOCAL0_OP);
}
#endif

/* refer to ACPI 6.4.3.5.3 Word Address Space Descriptor section for details */
void acpigen_resource_word(uint16_t res_type, uint16_t gen_flags, uint16_t type_flags, uint16_t gran, uint16_t range_min,
			   uint16_t range_max, uint16_t translation, uint16_t length)
{
	/* Byte 0: Type 1, Large Item Value 0x8: Word Address Space Descriptor */
	acpigen_emit_byte(0x88);
	/* Byte 1+2: length (0x000d) */
	acpigen_emit_byte(0x0d);
	acpigen_emit_byte(0x00);
	/* resource type */
	acpigen_emit_byte(res_type); // 0 - mem, 1 - io, 2 - bus
	/* general flags */
	acpigen_emit_byte(gen_flags);
	/* type flags */
	// refer to ACPI Table 6-234 (Memory), 6-235 (IO), 6-236 (Bus) for details
	acpigen_emit_byte(type_flags);
	/* granularity, min, max, translation, length */
	acpigen_emit_word(gran);
	acpigen_emit_word(range_min);
	acpigen_emit_word(range_max);
	acpigen_emit_word(translation);
	acpigen_emit_word(length);
}

/* refer to ACPI 6.4.3.5.2 DWord Address Space Descriptor section for details */
void acpigen_resource_dword(uint16_t res_type, uint16_t gen_flags, uint16_t type_flags, uint32_t gran,
			    uint32_t range_min, uint32_t range_max, uint32_t translation, uint32_t length)
{
	/* Byte 0: Type 1, Large Item Value 0x7: DWord Address Space Descriptor */
	acpigen_emit_byte(0x87);
	/* Byte 1+2: length (0023) */
	acpigen_emit_byte(23);
	acpigen_emit_byte(0x00);
	/* resource type */
	acpigen_emit_byte(res_type); // 0 - mem, 1 - io, 2 - bus
	/* general flags */
	acpigen_emit_byte(gen_flags);
	/* type flags */
	// refer to ACPI Table 6-234 (Memory), 6-235 (IO), 6-236 (Bus) for details
	acpigen_emit_byte(type_flags);
	/* granularity, min, max, translation, length */
	acpigen_emit_dword(gran);
	acpigen_emit_dword(range_min);
	acpigen_emit_dword(range_max);
	acpigen_emit_dword(translation);
	acpigen_emit_dword(length);
}

static void acpigen_emit_qword(uint64_t data)
{
	acpigen_emit_dword(data & 0xffffffff);
	acpigen_emit_dword((data >> 32) & 0xffffffff);
}

/* refer to ACPI 6.4.3.5.1 QWord Address Space Descriptor section for details */
void acpigen_resource_qword(uint16_t res_type, uint16_t gen_flags, uint16_t type_flags, uint64_t gran,
			    uint64_t range_min, uint64_t range_max, uint64_t translation, uint64_t length)
{
	/* Byte 0: Type 1, Large Item Value 0xa: QWord Address Space Descriptor */
	acpigen_emit_byte(0x8a);
	/* Byte 1+2: length (0x002b) */
	acpigen_emit_byte(0x2b);
	acpigen_emit_byte(0x00);
	/* resource type */
	acpigen_emit_byte(res_type); // 0 - mem, 1 - io, 2 - bus
	/* general flags */
	acpigen_emit_byte(gen_flags);
	/* type flags */
	// refer to ACPI Table 6-234 (Memory), 6-235 (IO), 6-236 (Bus) for details
	acpigen_emit_byte(type_flags);
	/* granularity, min, max, translation, length */
	acpigen_emit_qword(gran);
	acpigen_emit_qword(range_min);
	acpigen_emit_qword(range_max);
	acpigen_emit_qword(translation);
	acpigen_emit_qword(length);
}

void acpigen_resource_producer_bus_number(uint16_t bus_base, uint16_t bus_limit)
{
	acpigen_resource_word(RSRC_TYPE_BUS, /* res_type */
			      ADDR_SPACE_GENERAL_FLAG_MAX_FIXED
			      | ADDR_SPACE_GENERAL_FLAG_MIN_FIXED
			      | ADDR_SPACE_GENERAL_FLAG_DEC_POS
			      | ADDR_SPACE_GENERAL_FLAG_PRODUCER, /* gen_flags */
			      BUS_NUM_RANGE_RESOURCE_FLAG, /* type_flags */
			      0, /* gran */
			      bus_base, /* range_min */
			      bus_limit, /* range_max */
			      0x0, /* translation */
			      bus_limit - bus_base + 1); /* length */
}

void acpigen_resource_producer_io(uint16_t io_base, uint16_t io_limit)
{
	acpigen_resource_dword(RSRC_TYPE_IO, /* res_type */
			      ADDR_SPACE_GENERAL_FLAG_MAX_FIXED
			      | ADDR_SPACE_GENERAL_FLAG_MIN_FIXED
			      | ADDR_SPACE_GENERAL_FLAG_DEC_POS
			      | ADDR_SPACE_GENERAL_FLAG_PRODUCER, /* gen_flags */
			      IO_RSRC_FLAG_ENTIRE_RANGE, /* type_flags */
			      0, /* gran */
			      io_base, /* range_min */
			      io_limit, /* range_max */
			      0x0, /* translation */
			      io_limit - io_base + 1); /* length */
}

static void acpigen_resource_mmio32(uint32_t mmio_base, uint32_t mmio_limit, uint16_t gen_flags,
				    uint16_t type_flags)
{
	acpigen_resource_dword(RSRC_TYPE_MEM, /* res_type */
			       gen_flags, /* gen_flags */
			       type_flags, /* type_flags */
			       0, /* gran */
			       mmio_base, /* range_min */
			       mmio_limit, /* range_max */
			       0x0, /* translation */
			       mmio_limit - mmio_base + 1); /* length */
}

static void acpigen_resource_mmio64(uint64_t mmio_base, uint64_t mmio_limit, uint16_t gen_flags,
				    uint16_t type_flags)
{
	acpigen_resource_qword(RSRC_TYPE_MEM, /* res_type */
			       gen_flags, /* gen_flags */
			       type_flags, /* type_flags */
			       0, /* gran */
			       mmio_base, /* range_min */
			       mmio_limit, /* range_max */
			       0x0, /* translation */
			       mmio_limit - mmio_base + 1); /* length */
}

static void acpigen_resource_mmio(uint64_t mmio_base, uint64_t mmio_limit, bool is_producer, uint16_t type_flags)
{
	const uint16_t gen_flags = ADDR_SPACE_GENERAL_FLAG_MAX_FIXED
		| ADDR_SPACE_GENERAL_FLAG_MIN_FIXED
		| ADDR_SPACE_GENERAL_FLAG_DEC_POS
		| (is_producer ? ADDR_SPACE_GENERAL_FLAG_PRODUCER
		   : ADDR_SPACE_GENERAL_FLAG_CONSUMER);

	if (mmio_base < SZ_4G && mmio_limit < SZ_4G)
		acpigen_resource_mmio32(mmio_base, mmio_limit, gen_flags, type_flags);
	else
		acpigen_resource_mmio64(mmio_base, mmio_limit, gen_flags, type_flags);
}

void acpigen_resource_producer_mmio(uint64_t mmio_base, uint64_t mmio_limit, uint16_t type_flags)
{
	acpigen_resource_mmio(mmio_base, mmio_limit, true, type_flags);
}

void acpigen_resource_consumer_mmio(uint64_t mmio_base, uint64_t mmio_limit, uint16_t type_flags)
{
	acpigen_resource_mmio(mmio_base, mmio_limit, false, type_flags);
}

void acpigen_write_ADR(uint64_t adr)
{
	acpigen_write_name_qword("_ADR", adr);
}

/**
 * acpigen_write_ADR_soundwire_device() - SoundWire ACPI Device Address Encoding.
 * @address: SoundWire device address properties.
 *
 * From SoundWire Discovery and Configuration Specification Version 1.0 Table 3.
 *
 *   63..52 - Reserved (0)
 *   51..48 - Zero-based SoundWire Link ID, relative to the immediate parent.
 *            Used when a Controller has multiple master devices, each producing a
 *            separate SoundWire Link.  Set to 0 for single-link controllers.
 *   47..0  - SoundWire Device ID Encoding from specification version 1.2 table 88
 *   47..44 - SoundWire specification version that this device supports
 *   43..40 - Unique ID for multiple devices
 *   39..24 - MIPI standard manufacturer code
 *   23..08 - Vendor defined part ID
 *   07..00 - MIPI class encoding
 */
void acpigen_write_ADR_soundwire_device(const struct soundwire_address *address)
{
	acpigen_write_ADR((((uint64_t)address->link_id & 0xf) << 48) |
			  (((uint64_t)address->version & 0xf) << 44) |
			  (((uint64_t)address->unique_id & 0xf) << 40) |
			  (((uint64_t)address->manufacturer_id & 0xffff) << 24) |
			  (((uint64_t)address->part_id & 0xffff) << 8) |
			  (((uint64_t)address->class & 0xff)));
}

void acpigen_notify(const char *namestr, int value)
{
	acpigen_emit_byte(NOTIFY_OP);
	acpigen_emit_namestring(namestr);
	acpigen_write_integer(value);
}

static void _create_field(uint8_t aml_op, uint8_t srcop, size_t byte_offset, const char *name)
{
	acpigen_emit_byte(aml_op);
	acpigen_emit_byte(srcop);
	acpigen_write_integer(byte_offset);
	acpigen_emit_namestring(name);
}

void acpigen_write_create_bit_field(uint8_t op, size_t bit_offset, const char *name)
{
	_create_field(CREATE_BIT_OP, op, bit_offset, name);
}

void acpigen_write_create_byte_field(uint8_t op, size_t byte_offset, const char *name)
{
	_create_field(CREATE_BYTE_OP, op, byte_offset, name);
}

void acpigen_write_create_word_field(uint8_t op, size_t byte_offset, const char *name)
{
	_create_field(CREATE_WORD_OP, op, byte_offset, name);
}

void acpigen_write_create_dword_field(uint8_t op, size_t byte_offset, const char *name)
{
	_create_field(CREATE_DWORD_OP, op, byte_offset, name);
}

void acpigen_write_create_qword_field(uint8_t op, size_t byte_offset, const char *name)
{
	_create_field(CREATE_QWORD_OP, op, byte_offset, name);
}

static void _create_buffer_field(uint8_t aml_op, const char *src_buf, size_t byte_offset,
	const char *field)
{
	acpigen_emit_byte(aml_op);
	acpigen_emit_namestring(src_buf);
	acpigen_write_integer(byte_offset);
	acpigen_emit_namestring(field);
}

void acpigen_write_create_buffer_bit_field(const char *src_buf, size_t bit_offset, const char *field)
{
	_create_buffer_field(CREATE_BIT_OP, src_buf, bit_offset, field);
}

void acpigen_write_create_buffer_byte_field(const char *src_buf, size_t byte_offset, const char *field)
{
	_create_buffer_field(CREATE_BYTE_OP, src_buf, byte_offset, field);
}

void acpigen_write_create_buffer_word_field(const char *src_buf, size_t byte_offset, const char *field)
{
	_create_buffer_field(CREATE_WORD_OP, src_buf, byte_offset, field);
}

void acpigen_write_create_buffer_dword_field(const char *src_buf, size_t byte_offset, const char *field)
{
	_create_buffer_field(CREATE_DWORD_OP, src_buf, byte_offset, field);
}

void acpigen_write_create_buffer_qword_field(const char *src_buf, size_t byte_offset, const char *field)
{
	_create_buffer_field(CREATE_QWORD_OP, src_buf, byte_offset, field);
}

void acpigen_write_pct_package(const acpi_addr_t *perf_ctrl, const acpi_addr_t *perf_sts)
{
	acpigen_write_name("_PCT");
	acpigen_write_package(0x02);
	acpigen_write_register_resource(perf_ctrl);
	acpigen_write_register_resource(perf_sts);

	acpigen_pop_len();
}

void acpigen_write_xpss_package(const struct acpi_xpss_sw_pstate *pstate_value)
{
	acpigen_write_package(0x08);
	acpigen_write_dword(pstate_value->core_freq);
	acpigen_write_dword(pstate_value->power);
	acpigen_write_dword(pstate_value->transition_latency);
	acpigen_write_dword(pstate_value->bus_master_latency);

	acpigen_write_byte_buffer((uint8_t *)&pstate_value->control_value, sizeof(uint64_t));
	acpigen_write_byte_buffer((uint8_t *)&pstate_value->status_value, sizeof(uint64_t));
	acpigen_write_byte_buffer((uint8_t *)&pstate_value->control_mask, sizeof(uint64_t));
	acpigen_write_byte_buffer((uint8_t *)&pstate_value->status_mask, sizeof(uint64_t));

	acpigen_pop_len();
}

void acpigen_write_xpss_object(const struct acpi_xpss_sw_pstate *pstate_values, size_t nentries)
{
	size_t pstate;

	acpigen_write_name("XPSS");
	acpigen_write_package(nentries);
	for (pstate = 0; pstate < nentries; pstate++) {
		acpigen_write_xpss_package(pstate_values);
		pstate_values++;
	}

	acpigen_pop_len();
}

/* Delay up to wait_ms until provided namestr matches expected value. */
void acpigen_write_delay_until_namestr_int(uint32_t wait_ms, const char *name, uint64_t value)
{
	uint32_t wait_ms_segment = 1;
	uint32_t segments = wait_ms;

	/* Sleep in 2ms segments if delay is more than 2ms. */
	if (wait_ms > 2) {
		wait_ms_segment = 2;
		segments = wait_ms / wait_ms_segment;
	}

	acpigen_write_store_int_to_op(segments, LOCAL7_OP);
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_emit_byte(ZERO_OP);

	/* If name is not provided then just delay in a loop. */
	if (name) {
		acpigen_write_if_lequal_namestr_int(name, value);
		acpigen_emit_byte(BREAK_OP);
		acpigen_pop_len(); /* If */
	}

	acpigen_write_sleep(wait_ms_segment);
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_pop_len(); /* While */

	if (name) {
		acpigen_write_if_lequal_op_op(LOCAL7_OP, ZERO_OP);
		acpigen_write_debug_sprintf("WARN: Wait loop timeout for variable %s",
					    name);
		acpigen_pop_len(); /* If */
	}
}

void acpigen_ssdt_override_sleep_states(bool enable_s1, bool enable_s2, bool enable_s3,
					bool enable_s4)
{
#ifdef CONFIG_ACPI_S1
	BUG_ON(enable_s1);
#endif
#ifdef CONFIG_ACPI_S3
	BUG_ON(enable_s3);
#endif
#ifdef CONFIG_ACPI_S4
	BUG_ON(enable_s4);
#endif

	acpigen_write_scope("\\");
	uint32_t sleep_enable = (enable_s1 << 0) | (enable_s2 << 1)
		| (enable_s3 << 2) | (enable_s4 << 3);
	acpigen_write_name_dword("OSFG", sleep_enable);
	acpigen_pop_len();
}

int acpi_pld_fill_usb(struct acpi_pld *pld, enum acpi_upc_type type,
		      struct acpi_pld_group *group)
{
	if (!pld)
		return -1;

	memset(pld, 0, sizeof(struct acpi_pld));

	/* Set defaults */
	pld->ignore_color = 1;
	pld->panel = PLD_PANEL_UNKNOWN;
	pld->vertical_position = PLD_VERTICAL_POSITION_CENTER;
	pld->horizontal_position = PLD_HORIZONTAL_POSITION_CENTER;
	pld->rotation = PLD_ROTATE_0;
	pld->visible = 1;
	pld->group.token = group->token;
	pld->group.position = group->position;

	/* Set the shape based on port type */
	switch (type) {
	case UPC_TYPE_A:
	case UPC_TYPE_USB3_A:
	case UPC_TYPE_USB3_POWER_B:
		pld->shape = PLD_SHAPE_HORIZONTAL_RECTANGLE;
		break;
	case UPC_TYPE_MINI_AB:
	case UPC_TYPE_USB3_B:
		pld->shape = PLD_SHAPE_CHAMFERED;
		break;
	case UPC_TYPE_USB3_MICRO_B:
	case UPC_TYPE_USB3_MICRO_AB:
		pld->shape = PLD_SHAPE_HORIZONTAL_TRAPEZOID;
		break;
	case UPC_TYPE_C_USB2_ONLY:
	case UPC_TYPE_C_USB2_SS_SWITCH:
	case UPC_TYPE_C_USB2_SS:
		pld->shape = PLD_SHAPE_OVAL;
		break;
	case UPC_TYPE_INTERNAL:
	default:
		pld->shape = PLD_SHAPE_UNKNOWN;
		pld->visible = 0;
		break;
	}

	return 0;
}

int acpi_pld_to_buffer(const struct acpi_pld *pld, uint8_t *buf, int buf_len)
{
	if (!pld || !buf)
		return -1;

	memset(buf, 0, buf_len);

	/* [0] Revision (=2) */
	buf[0] = 0x2;

	if (pld->ignore_color) {
		/* [1] Ignore Color */
		buf[0] |= 0x80;
	} else {
		/* [15:8] Red Color */
		buf[1] = pld->color_red;
		/* [23:16] Green Color */
		buf[2] = pld->color_green;
		/* [31:24] Blue Color */
		buf[3] = pld->color_blue;
	}

	/* [47:32] Width */
	buf[4] = pld->width & 0xff;
	buf[5] = pld->width >> 8;

	/* [63:48] Height */
	buf[6] = pld->height & 0xff;
	buf[7] = pld->height >> 8;

	/* [64] User Visible */
	buf[8] |= (pld->visible & 0x1);

	/* [65] Dock */
	buf[8] |= (pld->dock & 0x1) << 1;

	/* [66] Lid */
	buf[8] |= (pld->lid & 0x1) << 2;

	/* [69:67] Panel */
	buf[8] |= (pld->panel & 0x7) << 3;

	/* [71:70] Vertical Position */
	buf[8] |= (pld->vertical_position & 0x3) << 6;

	/* [73:72] Horizontal Position */
	buf[9] |= (pld->horizontal_position & 0x3);

	/* [77:74] Shape */
	buf[9] |= (pld->shape & 0xf) << 2;

	/* [78] Orientation */
	buf[9] |= (pld->orientation & 0x1) << 6;

	/* [86:79] Group Token (incorrectly defined as 1 bit in ACPI 6.2A) */
	buf[9] |= (pld->group.token & 0x1) << 7;
	buf[10] |= (pld->group.token >> 0x1) & 0x7f;

	/* [94:87] Group Position */
	buf[10] |= (pld->group.position & 0x1) << 7;
	buf[11] |= (pld->group.position >> 0x1) & 0x7f;

	/* [95] Bay */
	buf[11] |= (pld->bay & 0x1) << 7;

	/* [96] Ejectable */
	buf[12] |= (pld->ejectable & 0x1);

	/* [97] Ejectable with OSPM help */
	buf[12] |= (pld->ejectable_ospm & 0x1) << 1;

	/* [105:98] Cabinet Number */
	buf[12] |= (pld->cabinet_number & 0x3f) << 2;
	buf[13] |= (pld->cabinet_number >> 6) & 0x3;

	/* [113:106] Card Cage Number */
	buf[13] |= (pld->card_cage_number & 0x3f) << 2;
	buf[14] |= (pld->card_cage_number >> 6) & 0x3;

	/* [114] PLD is a Reference Shape */
	buf[14] |= (pld->reference_shape & 0x1) << 2;

	/* [118:115] Rotation */
	buf[14] |= (pld->rotation & 0xf) << 3;

	/* [123:119] Draw Order */
	buf[14] |= (pld->draw_order & 0x1) << 7;
	buf[15] |= (pld->draw_order >> 1) & 0xf;

	/* [127:124] Reserved */

	/* Both 16 byte and 20 byte buffers are supported by the spec */
	if (buf_len == 20) {
		/* [143:128] Vertical Offset */
		buf[16] = pld->vertical_offset & 0xff;
		buf[17] = pld->vertical_offset >> 8;

		/* [159:144] Horizontal Offset */
		buf[18] = pld->horizontal_offset & 0xff;
		buf[19] = pld->horizontal_offset >> 8;
	}

	return 0;
}
