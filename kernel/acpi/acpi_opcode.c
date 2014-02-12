#include "acpi_int.h"

#define MAX_EXTENDED_OPCODE		0x88
#define NUM_EXTENDED_OPCODE		(MAX_EXTENDED_OPCODE + 1)
#define MAX_INTERNAL_OPCODE
#define NUM_INTERNAL_OPCODE		(MAX_INTERNAL_OPCODE + 1)

/* Used for non-assigned opcodes */
#define _UNK				0x6B

/*
 * Reserved ASCII characters. Do not use any of these for
 * internal opcodes, since they are used to differentiate
 * name strings from AML opcodes
 */
#define _ASC				0x6C
#define _NAM				0x6C
#define _PFX				0x6D

/*
 * This table is directly indexed by the opcodes It returns
 * an index into the opcode table (AcpiGbl_AmlOpInfo)
 */
const uint8_t acpi_gbl_short_opcode_indexes[256] =
{
/*              0     1     2     3     4     5     6     7  */
/*              8     9     A     B     C     D     E     F  */
/* 0x00 */    0x00, 0x01, _UNK, _UNK, _UNK, _UNK, 0x02, _UNK,
/* 0x08 */    0x03, _UNK, 0x04, 0x05, 0x06, 0x07, 0x6E, _UNK,
/* 0x10 */    0x08, 0x09, 0x0a, 0x6F, 0x0b, _UNK, _UNK, _UNK,
/* 0x18 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x20 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x28 */    _UNK, _UNK, _UNK, _UNK, _UNK, 0x63, _PFX, _PFX,
/* 0x30 */    0x67, 0x66, 0x68, 0x65, 0x69, 0x64, 0x6A, 0x7D,
/* 0x38 */    0x7F, 0x80, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x40 */    _UNK, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x48 */    _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x50 */    _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x58 */    _ASC, _ASC, _ASC, _UNK, _PFX, _UNK, _PFX, _ASC,
/* 0x60 */    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
/* 0x68 */    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, _UNK,
/* 0x70 */    0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22,
/* 0x78 */    0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
/* 0x80 */    0x2b, 0x2c, 0x2d, 0x2e, 0x70, 0x71, 0x2f, 0x30,
/* 0x88 */    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x72,
/* 0x90 */    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x73, 0x74,
/* 0x98 */    0x75, 0x76, _UNK, _UNK, 0x77, 0x78, 0x79, 0x7A,
/* 0xA0 */    0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x60, 0x61,
/* 0xA8 */    0x62, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xB0 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xB8 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xC0 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xC8 */    _UNK, _UNK, _UNK, _UNK, 0x44, _UNK, _UNK, _UNK,
/* 0xD0 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xD8 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xE0 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xE8 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xF0 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0xF8 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, 0x45,
};

/*
 * This table is indexed by the second opcode of the extended opcode
 * pair. It returns an index into the opcode table (AcpiGbl_AmlOpInfo)
 */
const uint8_t acpi_gbl_long_opcode_indexes[NUM_EXTENDED_OPCODE] =
{
/*              0     1     2     3     4     5     6     7  */
/*              8     9     A     B     C     D     E     F  */
/* 0x00 */    _UNK, 0x46, 0x47, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x08 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x10 */    _UNK, _UNK, 0x48, 0x49, _UNK, _UNK, _UNK, _UNK,
/* 0x18 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, 0x7B,
/* 0x20 */    0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51,
/* 0x28 */    0x52, 0x53, 0x54, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x30 */    0x55, 0x56, 0x57, 0x7e, _UNK, _UNK, _UNK, _UNK,
/* 0x38 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x40 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x48 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x50 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x58 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x60 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x68 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x70 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x78 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x80 */    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
/* 0x88 */    0x7C,
};

/* Total number of aml opcodes defined */

#define AML_NUM_OPCODES			0x81

struct acpi_opcode_info {
	char *name;
	uint32_t runtime_args;
	uint16_t flags;
	acpi_object_type object_type;
	uint8_t op_class;
	uint8_t op_type;
};

#define ACPI_OP(name, args, obj_type, op_class, op_type, flags)	\
	{(name), (uint32_t)(args), (uint32_t)(flags),		\
	 (obj_type), (op_class), (op_type)}

#define ARG_TYPE_WIDTH			5
#define ARG_1(x)			((uint32_t)(x))
#define ARG_2(x)			((uint32_t)(x) << (1 * ARG_TYPE_WIDTH))
#define ARG_3(x)			((uint32_t)(x) << (2 * ARG_TYPE_WIDTH))
#define ARG_4(x)			((uint32_t)(x) << (3 * ARG_TYPE_WIDTH))
#define ARG_5(x)			((uint32_t)(x) << (4 * ARG_TYPE_WIDTH))
#define ARG_6(x)			((uint32_t)(x) << (5 * ARG_TYPE_WIDTH))

#define ARGI_LIST1(a)			(ARG_1(a))
#define ARGI_LIST2(a, b)		(ARG_1(b)|ARG_2(a))
#define ARGI_LIST3(a, b, c)		(ARG_1(c)|ARG_2(b)|ARG_3(a))
#define ARGI_LIST4(a, b, c, d)		(ARG_1(d)|ARG_2(c)|ARG_3(b)|ARG_4(a))
#define ARGI_LIST5(a, b, c, d, e)	(ARG_1(e)|ARG_2(d)|ARG_3(c)|ARG_4(b)|ARG_5(a))
#define ARGI_LIST6(a, b, c, d, e, f)	(ARG_1(f)|ARG_2(e)|ARG_3(d)|ARG_4(c)|ARG_5(b)|ARG_6(a))

#define ARGP_LIST1(a)			(ARG_1(a))
#define ARGP_LIST2(a, b)		(ARG_1(a)|ARG_2(b))
#define ARGP_LIST3(a, b, c)		(ARG_1(a)|ARG_2(b)|ARG_3(c))
#define ARGP_LIST4(a, b, c, d)		(ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d))
#define ARGP_LIST5(a, b, c, d, e)	(ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d)|ARG_5(e))
#define ARGP_LIST6(a, b, c, d, e, f)	(ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d)|ARG_5(e)|ARG_6(f))

/*
 * All AML opcodes and the runtime arguments for each. Used by the AML
 * interpreter  Each list is compressed into a 32-bit number and stored
 * in the master opcode table (in psopcode.c).
 *
 * (Used by prep_operands procedure and the ASL Compiler)
 */
#define ARGI_ACCESSFIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_ACQUIRE_OP			ARGI_LIST2(ARGI_MUTEX,      ARGI_INTEGER)
#define ARGI_ADD_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_ALIAS_OP			ARGI_INVALID_OPCODE
#define ARGI_ARG0			ARG_NONE
#define ARGI_ARG1			ARG_NONE
#define ARGI_ARG2			ARG_NONE
#define ARGI_ARG3			ARG_NONE
#define ARGI_ARG4			ARG_NONE
#define ARGI_ARG5			ARG_NONE
#define ARGI_ARG6			ARG_NONE
#define ARGI_BANK_FIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_BIT_AND_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_BIT_NAND_OP		ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_BIT_NOR_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_BIT_NOT_OP			ARGI_LIST2(ARGI_INTEGER,    ARGI_TARGETREF)
#define ARGI_BIT_OR_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_BIT_XOR_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_BREAK_OP			ARG_NONE
#define ARGI_BREAK_POINT_OP		ARG_NONE
#define ARGI_BUFFER_OP			ARGI_LIST1(ARGI_INTEGER)
#define ARGI_BYTE_OP			ARGI_INVALID_OPCODE
#define ARGI_BYTELIST_OP		ARGI_INVALID_OPCODE
#define ARGI_CONCAT_OP			ARGI_LIST3(ARGI_COMPUTEDATA,ARGI_COMPUTEDATA,   ARGI_TARGETREF)
#define ARGI_CONCAT_RES_OP		ARGI_LIST3(ARGI_BUFFER,     ARGI_BUFFER,        ARGI_TARGETREF)
#define ARGI_COND_REF_OF_OP		ARGI_LIST2(ARGI_OBJECT_REF, ARGI_TARGETREF)
#define ARGI_CONNECTFIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_CONTINUE_OP		ARGI_INVALID_OPCODE
#define ARGI_COPY_OP			ARGI_LIST2(ARGI_ANYTYPE,    ARGI_SIMPLE_TARGET)
#define ARGI_CREATE_BIT_FIELD_OP	ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_REFERENCE)
#define ARGI_CREATE_BYTE_FIELD_OP	ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_REFERENCE)
#define ARGI_CREATE_DWORD_FIELD_OP	ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_REFERENCE)
#define ARGI_CREATE_FIELD_OP		ARGI_LIST4(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_INTEGER,      ARGI_REFERENCE)
#define ARGI_CREATE_QWORD_FIELD_OP	ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_REFERENCE)
#define ARGI_CREATE_WORD_FIELD_OP	ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_REFERENCE)
#define ARGI_DATA_REGION_OP		ARGI_LIST3(ARGI_STRING,     ARGI_STRING,        ARGI_STRING)
#define ARGI_DEBUG_OP			ARG_NONE
#define ARGI_DECREMENT_OP		ARGI_LIST1(ARGI_TARGETREF)
#define ARGI_DEREF_OF_OP		ARGI_LIST1(ARGI_REF_OR_STRING)
#define ARGI_DEVICE_OP			ARGI_INVALID_OPCODE
#define ARGI_DIVIDE_OP			ARGI_LIST4(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF,    ARGI_TARGETREF)
#define ARGI_DWORD_OP			ARGI_INVALID_OPCODE
#define ARGI_ELSE_OP			ARGI_INVALID_OPCODE
#define ARGI_EVENT_OP			ARGI_INVALID_OPCODE
#define ARGI_FATAL_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_INTEGER)
#define ARGI_FIELD_OP			ARGI_INVALID_OPCODE
#define ARGI_FIND_SET_LEFT_BIT_OP	ARGI_LIST2(ARGI_INTEGER,    ARGI_TARGETREF)
#define ARGI_FIND_SET_RIGHT_BIT_OP	ARGI_LIST2(ARGI_INTEGER,    ARGI_TARGETREF)
#define ARGI_FROM_BCD_OP		ARGI_LIST2(ARGI_INTEGER,    ARGI_FIXED_TARGET)
#define ARGI_IF_OP			ARGI_INVALID_OPCODE
#define ARGI_INCREMENT_OP		ARGI_LIST1(ARGI_TARGETREF)
#define ARGI_INDEX_FIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_INDEX_OP			ARGI_LIST3(ARGI_COMPLEXOBJ, ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_LAND_OP			ARGI_LIST2(ARGI_INTEGER,    ARGI_INTEGER)
#define ARGI_LEQUAL_OP			ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_COMPUTEDATA)
#define ARGI_LGREATER_OP		ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_COMPUTEDATA)
#define ARGI_LGREATEREQUAL_OP		ARGI_INVALID_OPCODE
#define ARGI_LLESS_OP			ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_COMPUTEDATA)
#define ARGI_LLESSEQUAL_OP		ARGI_INVALID_OPCODE
#define ARGI_LNOT_OP			ARGI_LIST1(ARGI_INTEGER)
#define ARGI_LNOTEQUAL_OP		ARGI_INVALID_OPCODE
#define ARGI_LOAD_OP			ARGI_LIST2(ARGI_REGION_OR_BUFFER,ARGI_TARGETREF)
#define ARGI_LOAD_TABLE_OP		ARGI_LIST6(ARGI_STRING,     ARGI_STRING,        ARGI_STRING,       ARGI_STRING,    ARGI_STRING, ARGI_ANYTYPE)
#define ARGI_LOCAL0			ARG_NONE
#define ARGI_LOCAL1			ARG_NONE
#define ARGI_LOCAL2			ARG_NONE
#define ARGI_LOCAL3			ARG_NONE
#define ARGI_LOCAL4			ARG_NONE
#define ARGI_LOCAL5			ARG_NONE
#define ARGI_LOCAL6			ARG_NONE
#define ARGI_LOCAL7			ARG_NONE
#define ARGI_LOR_OP			ARGI_LIST2(ARGI_INTEGER,    ARGI_INTEGER)
#define ARGI_MATCH_OP			ARGI_LIST6(ARGI_PACKAGE,    ARGI_INTEGER,   ARGI_COMPUTEDATA,      ARGI_INTEGER,ARGI_COMPUTEDATA,ARGI_INTEGER)
#define ARGI_METHOD_OP			ARGI_INVALID_OPCODE
#define ARGI_METHODCALL_OP		ARGI_INVALID_OPCODE
#define ARGI_MID_OP			ARGI_LIST4(ARGI_BUFFER_OR_STRING,ARGI_INTEGER,  ARGI_INTEGER,      ARGI_TARGETREF)
#define ARGI_MOD_OP			ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_MULTIPLY_OP		ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_MUTEX_OP			ARGI_INVALID_OPCODE
#define ARGI_NAME_OP			ARGI_INVALID_OPCODE
#define ARGI_NAMEDFIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_NAMEPATH_OP		ARGI_INVALID_OPCODE
#define ARGI_NOOP_OP			ARG_NONE
#define ARGI_NOTIFY_OP			ARGI_LIST2(ARGI_DEVICE_REF, ARGI_INTEGER)
#define ARGI_ONE_OP			ARG_NONE
#define ARGI_ONES_OP			ARG_NONE
#define ARGI_PACKAGE_OP			ARGI_LIST1(ARGI_INTEGER)
#define ARGI_POWER_RES_OP		ARGI_INVALID_OPCODE
#define ARGI_PROCESSOR_OP		ARGI_INVALID_OPCODE
#define ARGI_QWORD_OP			ARGI_INVALID_OPCODE
#define ARGI_REF_OF_OP			ARGI_LIST1(ARGI_OBJECT_REF)
#define ARGI_REGION_OP			ARGI_LIST2(ARGI_INTEGER,    ARGI_INTEGER)
#define ARGI_RELEASE_OP			ARGI_LIST1(ARGI_MUTEX)
#define ARGI_RESERVEDFIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_RESET_OP			ARGI_LIST1(ARGI_EVENT)
#define ARGI_RETURN_OP			ARGI_INVALID_OPCODE
#define ARGI_REVISION_OP		ARG_NONE
#define ARGI_SCOPE_OP			ARGI_INVALID_OPCODE
#define ARGI_SERIALFIELD_OP		ARGI_INVALID_OPCODE
#define ARGI_SHIFT_LEFT_OP		ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_SHIFT_RIGHT_OP		ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_SIGNAL_OP			ARGI_LIST1(ARGI_EVENT)
#define ARGI_SIZE_OF_OP			ARGI_LIST1(ARGI_DATAOBJECT)
#define ARGI_SLEEP_OP			ARGI_LIST1(ARGI_INTEGER)
#define ARGI_STALL_OP			ARGI_LIST1(ARGI_INTEGER)
#define ARGI_STATICSTRING_OP		ARGI_INVALID_OPCODE
#define ARGI_STORE_OP			ARGI_LIST2(ARGI_DATAREFOBJ, ARGI_TARGETREF)
#define ARGI_STRING_OP			ARGI_INVALID_OPCODE
#define ARGI_SUBTRACT_OP		ARGI_LIST3(ARGI_INTEGER,    ARGI_INTEGER,       ARGI_TARGETREF)
#define ARGI_THERMAL_ZONE_OP		ARGI_INVALID_OPCODE
#define ARGI_TIMER_OP			ARG_NONE
#define ARGI_TO_BCD_OP			ARGI_LIST2(ARGI_INTEGER,    ARGI_FIXED_TARGET)
#define ARGI_TO_BUFFER_OP		ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_FIXED_TARGET)
#define ARGI_TO_DEC_STR_OP		ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_FIXED_TARGET)
#define ARGI_TO_HEX_STR_OP		ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_FIXED_TARGET)
#define ARGI_TO_INTEGER_OP		ARGI_LIST2(ARGI_COMPUTEDATA,ARGI_FIXED_TARGET)
#define ARGI_TO_STRING_OP		ARGI_LIST3(ARGI_BUFFER,     ARGI_INTEGER,       ARGI_FIXED_TARGET)
#define ARGI_TYPE_OP			ARGI_LIST1(ARGI_ANYTYPE)
#define ARGI_UNLOAD_OP			ARGI_LIST1(ARGI_DDBHANDLE)
#define ARGI_VAR_PACKAGE_OP		ARGI_LIST1(ARGI_INTEGER)
#define ARGI_WAIT_OP			ARGI_LIST2(ARGI_EVENT,      ARGI_INTEGER)
#define ARGI_WHILE_OP			ARGI_INVALID_OPCODE
#define ARGI_WORD_OP			ARGI_INVALID_OPCODE
#define ARGI_ZERO_OP			ARG_NONE

/*
 * Master Opcode information table. A summary of everything we know about each
 * opcode, all in one place.
 */
const struct acpi_opcode_info acpi_gbl_opcode_info[AML_NUM_OPCODES] =
{
/* Index          Name                     Interpreter Args            ObjectType                   Class                      Type                      Flags */
/* 00 */ ACPI_OP("Zero",                   ARGI_ZERO_OP,               ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_CONSTANT,        AML_CONSTANT),
/* 01 */ ACPI_OP("One",                    ARGI_ONE_OP,                ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_CONSTANT,        AML_CONSTANT),
/* 02 */ ACPI_OP("Alias",                  ARGI_ALIAS_OP,              ACPI_TYPE_LOCAL_ALIAS,       AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_SIMPLE,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 03 */ ACPI_OP("Name",                   ARGI_NAME_OP,               ACPI_TYPE_ANY,               AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_COMPLEX,   AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 04 */ ACPI_OP("ByteConst",              ARGI_BYTE_OP,               ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_CONSTANT),
/* 05 */ ACPI_OP("WordConst",              ARGI_WORD_OP,               ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_CONSTANT),
/* 06 */ ACPI_OP("DwordConst",             ARGI_DWORD_OP,              ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_CONSTANT),
/* 07 */ ACPI_OP("String",                 ARGI_STRING_OP,             ACPI_TYPE_STRING,            AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_CONSTANT),
/* 08 */ ACPI_OP("Scope",                  ARGI_SCOPE_OP,              ACPI_TYPE_LOCAL_SCOPE,       AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_NO_OBJ,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 09 */ ACPI_OP("Buffer",                 ARGI_BUFFER_OP,             ACPI_TYPE_BUFFER,            AML_CLASS_CREATE,          AML_TYPE_CREATE_OBJECT,   AML_HAS_ARGS | AML_DEFER | AML_CONSTANT),
/* 0A */ ACPI_OP("Package",                ARGI_PACKAGE_OP,            ACPI_TYPE_PACKAGE,           AML_CLASS_CREATE,          AML_TYPE_CREATE_OBJECT,   AML_HAS_ARGS | AML_DEFER | AML_CONSTANT),
/* 0B */ ACPI_OP("Method",                 ARGI_METHOD_OP,             ACPI_TYPE_METHOD,            AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_COMPLEX,   AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED | AML_DEFER),
/* 0C */ ACPI_OP("Local0",                 ARGI_LOCAL0,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 0D */ ACPI_OP("Local1",                 ARGI_LOCAL1,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 0E */ ACPI_OP("Local2",                 ARGI_LOCAL2,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 0F */ ACPI_OP("Local3",                 ARGI_LOCAL3,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 10 */ ACPI_OP("Local4",                 ARGI_LOCAL4,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 11 */ ACPI_OP("Local5",                 ARGI_LOCAL5,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 12 */ ACPI_OP("Local6",                 ARGI_LOCAL6,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 13 */ ACPI_OP("Local7",                 ARGI_LOCAL7,                ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_LOCAL_VARIABLE,  0),
/* 14 */ ACPI_OP("Arg0",                   ARGI_ARG0,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 15 */ ACPI_OP("Arg1",                   ARGI_ARG1,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 16 */ ACPI_OP("Arg2",                   ARGI_ARG2,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 17 */ ACPI_OP("Arg3",                   ARGI_ARG3,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 18 */ ACPI_OP("Arg4",                   ARGI_ARG4,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 19 */ ACPI_OP("Arg5",                   ARGI_ARG5,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 1A */ ACPI_OP("Arg6",                   ARGI_ARG6,                  ACPI_TYPE_LOCAL_REFERENCE,   AML_CLASS_ARGUMENT,        AML_TYPE_METHOD_ARGUMENT, 0),
/* 1B */ ACPI_OP("Store",                  ARGI_STORE_OP,              ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R),
/* 1C */ ACPI_OP("RefOf",                  ARGI_REF_OF_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R),
/* 1D */ ACPI_OP("Add",                    ARGI_ADD_OP,                ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 1E */ ACPI_OP("Concatenate",            ARGI_CONCAT_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_CONSTANT),
/* 1F */ ACPI_OP("Subtract",               ARGI_SUBTRACT_OP,           ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 20 */ ACPI_OP("Increment",              ARGI_INCREMENT_OP,          ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R | AML_CONSTANT),
/* 21 */ ACPI_OP("Decrement",              ARGI_DECREMENT_OP,          ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R | AML_CONSTANT),
/* 22 */ ACPI_OP("Multiply",               ARGI_MULTIPLY_OP,           ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 23 */ ACPI_OP("Divide",                 ARGI_DIVIDE_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_2T_1R,   AML_FLAGS_EXEC_2A_2T_1R | AML_CONSTANT),
/* 24 */ ACPI_OP("ShiftLeft",              ARGI_SHIFT_LEFT_OP,         ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 25 */ ACPI_OP("ShiftRight",             ARGI_SHIFT_RIGHT_OP,        ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 26 */ ACPI_OP("And",                    ARGI_BIT_AND_OP,            ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 27 */ ACPI_OP("NAnd",                   ARGI_BIT_NAND_OP,           ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 28 */ ACPI_OP("Or",                     ARGI_BIT_OR_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 29 */ ACPI_OP("NOr",                    ARGI_BIT_NOR_OP,            ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 2A */ ACPI_OP("XOr",                    ARGI_BIT_XOR_OP,            ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_MATH | AML_CONSTANT),
/* 2B */ ACPI_OP("Not",                    ARGI_BIT_NOT_OP,            ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 2C */ ACPI_OP("FindSetLeftBit",         ARGI_FIND_SET_LEFT_BIT_OP,  ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 2D */ ACPI_OP("FindSetRightBit",        ARGI_FIND_SET_RIGHT_BIT_OP, ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 2E */ ACPI_OP("DerefOf",                ARGI_DEREF_OF_OP,           ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R),
/* 2F */ ACPI_OP("Notify",                 ARGI_NOTIFY_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_0R,   AML_FLAGS_EXEC_2A_0T_0R),
/* 30 */ ACPI_OP("SizeOf",                 ARGI_SIZE_OF_OP,            ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R | AML_NO_OPERAND_RESOLVE),
/* 31 */ ACPI_OP("Index",                  ARGI_INDEX_OP,              ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R),
/* 32 */ ACPI_OP("Match",                  ARGI_MATCH_OP,              ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_6A_0T_1R,   AML_FLAGS_EXEC_6A_0T_1R | AML_CONSTANT),
/* 33 */ ACPI_OP("CreateDWordField",       ARGI_CREATE_DWORD_FIELD_OP, ACPI_TYPE_BUFFER_FIELD,      AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_CREATE),
/* 34 */ ACPI_OP("CreateWordField",        ARGI_CREATE_WORD_FIELD_OP,  ACPI_TYPE_BUFFER_FIELD,      AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_CREATE),
/* 35 */ ACPI_OP("CreateByteField",        ARGI_CREATE_BYTE_FIELD_OP,  ACPI_TYPE_BUFFER_FIELD,      AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_CREATE),
/* 36 */ ACPI_OP("CreateBitField",         ARGI_CREATE_BIT_FIELD_OP,   ACPI_TYPE_BUFFER_FIELD,      AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_CREATE),
/* 37 */ ACPI_OP("ObjectType",             ARGI_TYPE_OP,               ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R | AML_NO_OPERAND_RESOLVE),
/* 38 */ ACPI_OP("LAnd",                   ARGI_LAND_OP,               ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R | AML_LOGICAL_NUMERIC | AML_CONSTANT),
/* 39 */ ACPI_OP("LOr",                    ARGI_LOR_OP,                ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R | AML_LOGICAL_NUMERIC | AML_CONSTANT),
/* 3A */ ACPI_OP("LNot",                   ARGI_LNOT_OP,               ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_1R,   AML_FLAGS_EXEC_1A_0T_1R | AML_CONSTANT),
/* 3B */ ACPI_OP("LEqual",                 ARGI_LEQUAL_OP,             ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R | AML_LOGICAL | AML_CONSTANT),
/* 3C */ ACPI_OP("LGreater",               ARGI_LGREATER_OP,           ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R | AML_LOGICAL | AML_CONSTANT),
/* 3D */ ACPI_OP("LLess",                  ARGI_LLESS_OP,              ACPI_TYPE_ANY,               AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R | AML_LOGICAL | AML_CONSTANT),
/* 3E */ ACPI_OP("If",                     ARGI_IF_OP,                 ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         AML_HAS_ARGS),
/* 3F */ ACPI_OP("Else",                   ARGI_ELSE_OP,               ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         AML_HAS_ARGS),
/* 40 */ ACPI_OP("While",                  ARGI_WHILE_OP,              ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         AML_HAS_ARGS),
/* 41 */ ACPI_OP("Noop",                   ARGI_NOOP_OP,               ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         0),
/* 42 */ ACPI_OP("Return",                 ARGI_RETURN_OP,             ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         AML_HAS_ARGS),
/* 43 */ ACPI_OP("Break",                  ARGI_BREAK_OP,              ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         0),
/* 44 */ ACPI_OP("BreakPoint",             ARGI_BREAK_POINT_OP,        ACPI_TYPE_ANY,               AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         0),
/* 45 */ ACPI_OP("Ones",                   ARGI_ONES_OP,               ACPI_TYPE_INTEGER,           AML_CLASS_ARGUMENT,        AML_TYPE_CONSTANT,        AML_CONSTANT),

/* Prefixed opcodes (Two-byte opcodes with a prefix op) */

/* 46 */ ACPI_OP ("Mutex",                  ARGI_MUTEX_OP,              ACPI_TYPE_MUTEX,            AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_SIMPLE,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 47 */ ACPI_OP ("Event",                  ARGI_EVENT_OP,              ACPI_TYPE_EVENT,            AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_SIMPLE,    AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED ),
/* 48 */ ACPI_OP ("CondRefOf",              ARGI_COND_REF_OF_OP,        ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R),
/* 49 */ ACPI_OP ("CreateField",            ARGI_CREATE_FIELD_OP,       ACPI_TYPE_BUFFER_FIELD,     AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_FIELD | AML_CREATE),
/* 4A */ ACPI_OP ("Load",                   ARGI_LOAD_OP,               ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_0R,   AML_FLAGS_EXEC_1A_1T_0R),
/* 4B */ ACPI_OP ("Stall",                  ARGI_STALL_OP,              ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 4C */ ACPI_OP ("Sleep",                  ARGI_SLEEP_OP,              ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 4D */ ACPI_OP ("Acquire",                ARGI_ACQUIRE_OP,            ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R),
/* 4E */ ACPI_OP ("Signal",                 ARGI_SIGNAL_OP,             ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 4F */ ACPI_OP ("Wait",                   ARGI_WAIT_OP,               ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_0T_1R,   AML_FLAGS_EXEC_2A_0T_1R),
/* 50 */ ACPI_OP ("Reset",                  ARGI_RESET_OP,              ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 51 */ ACPI_OP ("Release",                ARGI_RELEASE_OP,            ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 52 */ ACPI_OP ("FromBCD",                ARGI_FROM_BCD_OP,           ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 53 */ ACPI_OP ("ToBCD",                  ARGI_TO_BCD_OP,             ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 54 */ ACPI_OP ("Unload",                 ARGI_UNLOAD_OP,             ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_0T_0R,   AML_FLAGS_EXEC_1A_0T_0R),
/* 55 */ ACPI_OP ("Revision",               ARGI_REVISION_OP,           ACPI_TYPE_INTEGER,          AML_CLASS_ARGUMENT,        AML_TYPE_CONSTANT,        0),
/* 56 */ ACPI_OP ("Debug",                  ARGI_DEBUG_OP,              ACPI_TYPE_LOCAL_REFERENCE,  AML_CLASS_ARGUMENT,        AML_TYPE_CONSTANT,        0),
/* 57 */ ACPI_OP ("Fatal",                  ARGI_FATAL_OP,              ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_3A_0T_0R,   AML_FLAGS_EXEC_3A_0T_0R),
/* 58 */ ACPI_OP ("OperationRegion",        ARGI_REGION_OP,             ACPI_TYPE_REGION,           AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_COMPLEX,   AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED | AML_DEFER),
/* 59 */ ACPI_OP ("Field",                  ARGI_FIELD_OP,              ACPI_TYPE_ANY,              AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_FIELD,     AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_FIELD),
/* 5A */ ACPI_OP ("Device",                 ARGI_DEVICE_OP,             ACPI_TYPE_DEVICE,           AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_NO_OBJ,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 5B */ ACPI_OP ("Processor",              ARGI_PROCESSOR_OP,          ACPI_TYPE_PROCESSOR,        AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_SIMPLE,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 5C */ ACPI_OP ("PowerResource",          ARGI_POWER_RES_OP,          ACPI_TYPE_POWER,            AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_SIMPLE,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 5D */ ACPI_OP ("ThermalZone",            ARGI_THERMAL_ZONE_OP,       ACPI_TYPE_THERMAL,          AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_NO_OBJ,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED),
/* 5E */ ACPI_OP ("IndexField",             ARGI_INDEX_FIELD_OP,        ACPI_TYPE_ANY,              AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_FIELD,     AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_FIELD),
/* 5F */ ACPI_OP ("BankField",              ARGI_BANK_FIELD_OP,         ACPI_TYPE_LOCAL_BANK_FIELD, AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_FIELD,     AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_FIELD | AML_DEFER),

/* Internal opcodes that map to invalid AML opcodes */
/* 60 */ ACPI_OP ("LNotEqual",              ARGI_LNOTEQUAL_OP,          ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           AML_HAS_ARGS | AML_CONSTANT),
/* 61 */ ACPI_OP ("LLessEqual",             ARGI_LLESSEQUAL_OP,         ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           AML_HAS_ARGS | AML_CONSTANT),
/* 62 */ ACPI_OP ("LGreaterEqual",          ARGI_LGREATEREQUAL_OP,      ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           AML_HAS_ARGS | AML_CONSTANT),
/* 63 */ ACPI_OP ("-NamePath-",             ARGI_NAMEPATH_OP,           ACPI_TYPE_LOCAL_REFERENCE,  AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_NSOBJECT | AML_NSNODE ),
/* 64 */ ACPI_OP ("-MethodCall-",           ARGI_METHODCALL_OP,         ACPI_TYPE_METHOD,           AML_CLASS_METHOD_CALL,     AML_TYPE_METHOD_CALL,     AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE),
/* 65 */ ACPI_OP ("-ByteList-",             ARGI_BYTELIST_OP,           ACPI_TYPE_ANY,              AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         0),
/* 66 */ ACPI_OP ("-ReservedField-",        ARGI_RESERVEDFIELD_OP,      ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           0),
/* 67 */ ACPI_OP ("-NamedField-",           ARGI_NAMEDFIELD_OP,         ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED ),
/* 68 */ ACPI_OP ("-AccessField-",          ARGI_ACCESSFIELD_OP,        ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           0),
/* 69 */ ACPI_OP ("-StaticString",          ARGI_STATICSTRING_OP,       ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           0),
/* 6A */ ACPI_OP ("-Return Value-",         ARG_NONE,                   ACPI_TYPE_ANY,              AML_CLASS_RETURN_VALUE,    AML_TYPE_RETURN,          AML_HAS_ARGS | AML_HAS_RETVAL),
/* 6B */ ACPI_OP ("-UNKNOWN_OP-",           ARG_NONE,                   ACPI_TYPE_INVALID,          AML_CLASS_UNKNOWN,         AML_TYPE_BOGUS,           AML_HAS_ARGS),
/* 6C */ ACPI_OP ("-ASCII_ONLY-",           ARG_NONE,                   ACPI_TYPE_ANY,              AML_CLASS_ASCII,           AML_TYPE_BOGUS,           AML_HAS_ARGS),
/* 6D */ ACPI_OP ("-PREFIX_ONLY-",          ARG_NONE,                   ACPI_TYPE_ANY,              AML_CLASS_PREFIX,          AML_TYPE_BOGUS,           AML_HAS_ARGS),

/* ACPI 2.0 opcodes */

/* 6E */ ACPI_OP ("QwordConst",             ARGI_QWORD_OP,              ACPI_TYPE_INTEGER,          AML_CLASS_ARGUMENT,        AML_TYPE_LITERAL,         AML_CONSTANT),
/* 6F */ ACPI_OP ("Package", /* Var */      ARGI_VAR_PACKAGE_OP,        ACPI_TYPE_PACKAGE,          AML_CLASS_CREATE,          AML_TYPE_CREATE_OBJECT,   AML_HAS_ARGS | AML_DEFER),
/* 70 */ ACPI_OP ("ConcatenateResTemplate", ARGI_CONCAT_RES_OP,         ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_CONSTANT),
/* 71 */ ACPI_OP ("Mod",                    ARGI_MOD_OP,                ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_CONSTANT),
/* 72 */ ACPI_OP ("CreateQWordField",       ARGI_CREATE_QWORD_FIELD_OP, ACPI_TYPE_BUFFER_FIELD,     AML_CLASS_CREATE,          AML_TYPE_CREATE_FIELD,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSNODE | AML_DEFER | AML_CREATE),
/* 73 */ ACPI_OP ("ToBuffer",               ARGI_TO_BUFFER_OP,          ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 74 */ ACPI_OP ("ToDecimalString",        ARGI_TO_DEC_STR_OP,         ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 75 */ ACPI_OP ("ToHexString",            ARGI_TO_HEX_STR_OP,         ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 76 */ ACPI_OP ("ToInteger",              ARGI_TO_INTEGER_OP,         ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R | AML_CONSTANT),
/* 77 */ ACPI_OP ("ToString",               ARGI_TO_STRING_OP,          ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_2A_1T_1R,   AML_FLAGS_EXEC_2A_1T_1R | AML_CONSTANT),
/* 78 */ ACPI_OP ("CopyObject",             ARGI_COPY_OP,               ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_1A_1T_1R,   AML_FLAGS_EXEC_1A_1T_1R),
/* 79 */ ACPI_OP ("Mid",                    ARGI_MID_OP,                ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_3A_1T_1R,   AML_FLAGS_EXEC_3A_1T_1R | AML_CONSTANT),
/* 7A */ ACPI_OP ("Continue",               ARGI_CONTINUE_OP,           ACPI_TYPE_ANY,              AML_CLASS_CONTROL,         AML_TYPE_CONTROL,         0),
/* 7B */ ACPI_OP ("LoadTable",              ARGI_LOAD_TABLE_OP,         ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_6A_0T_1R,   AML_FLAGS_EXEC_6A_0T_1R),
/* 7C */ ACPI_OP ("DataTableRegion",        ARGI_DATA_REGION_OP,        ACPI_TYPE_REGION,           AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_COMPLEX,   AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE | AML_NAMED | AML_DEFER),
/* 7D */ ACPI_OP ("[EvalSubTree]",          ARGI_SCOPE_OP,              ACPI_TYPE_ANY,              AML_CLASS_NAMED_OBJECT,    AML_TYPE_NAMED_NO_OBJ,    AML_HAS_ARGS | AML_NSOBJECT | AML_NSOPCODE | AML_NSNODE),

/* ACPI 3.0 opcodes */
/* 7E */ ACPI_OP ("Timer",                  ARGI_TIMER_OP,              ACPI_TYPE_ANY,              AML_CLASS_EXECUTE,         AML_TYPE_EXEC_0A_0T_1R,   AML_FLAGS_EXEC_0A_0T_1R),

/* ACPI 5.0 opcodes */
/* 7F */ ACPI_OP ("-ConnectField-",         ARGI_CONNECTFIELD_OP,       ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           AML_HAS_ARGS),
/* 80 */ ACPI_OP ("-ExtAccessField-",       ARGI_CONNECTFIELD_OP,       ACPI_TYPE_ANY,              AML_CLASS_INTERNAL,        AML_TYPE_BOGUS,           0)
};

const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode)
{
	const char *opcode_name = "Unknown AML opcode";

	/*
	 * Detect normal 8-bit opcode or extended 16-bit opcode
	 */
	if (!(opcode & 0xFF00))
		return (&acpi_gbl_opcode_info[acpi_gbl_short_opcode_indexes[(uint8_t)opcode]]);
	if (((opcode & 0xFF00) == AML_EXTENDED_OPCODE) &&
	    (((uint8_t)opcode) <= MAX_EXTENDED_OPCODE))
		return (&acpi_gbl_opcode_info[acpi_gbl_long_opcode_indexes[(uint8_t)opcode]]);

	/* Unknown AML opcode */
	acpi_warn("%s [%4.4X]\n", opcode_name, opcode);
	return &acpi_gbl_opcode_info[_UNK];
}

union acpi_opcode *acpi_opcode_alloc(uint16_t opcode)
{
	union acpi_opcode *op;
	const struct acpi_opcode_info *op_info;
	uint8_t flags = ACPI_OPCODE_GENERIC;
	
	op_info = acpi_opcode_get_info(opcode);
	
	if (op_info->flags & AML_DEFER)
		flags = ACPI_OPCODE_DEFERRED;
	else if (op_info->flags & AML_NAMED)
		flags = ACPI_OPCODE_NAMED;
	else if (opcode == AML_INT_BYTELIST_OP)
		flags = ACPI_OPCODE_BYTELIST;
	
	/* Allocate the minimum required size object */
	if (flags == ACPI_OPCODE_GENERIC)
		op = acpi_os_allocate_zeroed(sizeof (struct acpi_opcode_common));
	else
		op = acpi_os_allocate_zeroed(sizeof (struct acpi_opcode_named));
	if (op) {
		op->common.descriptor_type = ACPI_DESC_TYPE_OPCODE;
		op->common.aml_opcode = opcode;
		op->common.flags = flags;
	}
	
	return op;
}

void acpi_opcode_free(union acpi_opcode *op)
{
	if (op->common.aml_opcode == AML_INT_RETURN_VALUE_OP)
		acpi_dbg("Free retval op: %p\n", op);
	acpi_os_free(op);
}
