#ifndef __ACPI_AML_H_INCLUDE__
#define __ACPI_AML_H_INCLUDE__

/* primary opcodes */

#define AML_NULL_CHAR			(uint16_t)0x00

#define AML_ZERO_OP			(uint16_t)0x00
#define AML_ONE_OP			(uint16_t)0x01
#define AML_UNASSIGNED			(uint16_t)0x02
#define AML_ALIAS_OP			(uint16_t)0x06
#define AML_NAME_OP			(uint16_t)0x08
#define AML_BYTE_OP			(uint16_t)0x0a
#define AML_WORD_OP			(uint16_t)0x0b
#define AML_DWORD_OP			(uint16_t)0x0c
#define AML_STRING_OP			(uint16_t)0x0d
#define AML_QWORD_OP			(uint16_t)0x0e /* ACPI 2.0 */
#define AML_SCOPE_OP			(uint16_t)0x10
#define AML_BUFFER_OP			(uint16_t)0x11
#define AML_PACKAGE_OP			(uint16_t)0x12
#define AML_VAR_PACKAGE_OP		(uint16_t)0x13 /* ACPI 2.0 */
#define AML_METHOD_OP			(uint16_t)0x14
#define AML_DUAL_NAME_PREFIX		(uint16_t)0x2e
#define AML_MULTI_NAME_PREFIX_OP	(uint16_t)0x2f
#define AML_NAME_CHAR_SUBSEQ		(uint16_t)0x30
#define AML_NAME_CHAR_FIRST		(uint16_t)0x41
#define AML_EXTENDED_OP_PREFIX		(uint16_t)0x5b
#define AML_ROOT_PREFIX			(uint16_t)0x5c
#define AML_PARENT_PREFIX		(uint16_t)0x5e
#define AML_LOCAL_OP			(uint16_t)0x60
#define AML_LOCAL0			(uint16_t)0x60
#define AML_LOCAL1			(uint16_t)0x61
#define AML_LOCAL2			(uint16_t)0x62
#define AML_LOCAL3			(uint16_t)0x63
#define AML_LOCAL4			(uint16_t)0x64
#define AML_LOCAL5			(uint16_t)0x65
#define AML_LOCAL6			(uint16_t)0x66
#define AML_LOCAL7			(uint16_t)0x67
#define AML_ARG_OP			(uint16_t)0x68
#define AML_ARG0			(uint16_t)0x68
#define AML_ARG1			(uint16_t)0x69
#define AML_ARG2			(uint16_t)0x6a
#define AML_ARG3			(uint16_t)0x6b
#define AML_ARG4			(uint16_t)0x6c
#define AML_ARG5			(uint16_t)0x6d
#define AML_ARG6			(uint16_t)0x6e
#define AML_STORE_OP			(uint16_t)0x70
#define AML_REF_OF_OP			(uint16_t)0x71
#define AML_ADD_OP			(uint16_t)0x72
#define AML_CONCAT_OP			(uint16_t)0x73
#define AML_SUBTRACT_OP			(uint16_t)0x74
#define AML_INCREMENT_OP		(uint16_t)0x75
#define AML_DECREMENT_OP		(uint16_t)0x76
#define AML_MULTIPLY_OP			(uint16_t)0x77
#define AML_DIVIDE_OP			(uint16_t)0x78
#define AML_SHIFT_LEFT_OP		(uint16_t)0x79
#define AML_SHIFT_RIGHT_OP		(uint16_t)0x7a
#define AML_BIT_AND_OP			(uint16_t)0x7b
#define AML_BIT_NAND_OP			(uint16_t)0x7c
#define AML_BIT_OR_OP			(uint16_t)0x7d
#define AML_BIT_NOR_OP			(uint16_t)0x7e
#define AML_BIT_XOR_OP			(uint16_t)0x7f
#define AML_BIT_NOT_OP			(uint16_t)0x80
#define AML_FIND_SET_LEFT_BIT_OP	(uint16_t)0x81
#define AML_FIND_SET_RIGHT_BIT_OP	(uint16_t)0x82
#define AML_DEREF_OF_OP			(uint16_t)0x83
#define AML_CONCAT_RES_OP		(uint16_t)0x84 /* ACPI 2.0 */
#define AML_MOD_OP			(uint16_t)0x85 /* ACPI 2.0 */
#define AML_NOTIFY_OP			(uint16_t)0x86
#define AML_SIZE_OF_OP			(uint16_t)0x87
#define AML_INDEX_OP			(uint16_t)0x88
#define AML_MATCH_OP			(uint16_t)0x89
#define AML_CREATE_DWORD_FIELD_OP	(uint16_t)0x8a
#define AML_CREATE_WORD_FIELD_OP	(uint16_t)0x8b
#define AML_CREATE_BYTE_FIELD_OP	(uint16_t)0x8c
#define AML_CREATE_BIT_FIELD_OP		(uint16_t)0x8d
#define AML_TYPE_OP			(uint16_t)0x8e
#define AML_CREATE_QWORD_FIELD_OP	(uint16_t)0x8f /* ACPI 2.0 */
#define AML_LAND_OP			(uint16_t)0x90
#define AML_LOR_OP			(uint16_t)0x91
#define AML_LNOT_OP			(uint16_t)0x92
#define AML_LEQUAL_OP			(uint16_t)0x93
#define AML_LGREATER_OP			(uint16_t)0x94
#define AML_LLESS_OP			(uint16_t)0x95
#define AML_TO_BUFFER_OP		(uint16_t)0x96 /* ACPI 2.0 */
#define AML_TO_DECSTRING_OP		(uint16_t)0x97 /* ACPI 2.0 */
#define AML_TO_HEXSTRING_OP		(uint16_t)0x98 /* ACPI 2.0 */
#define AML_TO_INTEGER_OP		(uint16_t)0x99 /* ACPI 2.0 */
#define AML_TO_STRING_OP		(uint16_t)0x9c /* ACPI 2.0 */
#define AML_COPY_OP			(uint16_t)0x9d /* ACPI 2.0 */
#define AML_MID_OP			(uint16_t)0x9e /* ACPI 2.0 */
#define AML_CONTINUE_OP			(uint16_t)0x9f /* ACPI 2.0 */
#define AML_IF_OP			(uint16_t)0xa0
#define AML_ELSE_OP			(uint16_t)0xa1
#define AML_WHILE_OP			(uint16_t)0xa2
#define AML_NOOP_OP			(uint16_t)0xa3
#define AML_RETURN_OP			(uint16_t)0xa4
#define AML_BREAK_OP			(uint16_t)0xa5
#define AML_BREAK_POINT_OP		(uint16_t)0xcc
#define AML_ONES_OP			(uint16_t)0xff

/* prefixed opcodes */

#define AML_EXTENDED_OPCODE		(uint16_t)0x5b00 /* prefix for 2-byte opcodes */

#define AML_MUTEX_OP			(uint16_t)0x5b01
#define AML_EVENT_OP			(uint16_t)0x5b02
#define AML_SHIFT_RIGHT_BIT_OP		(uint16_t)0x5b10
#define AML_SHIFT_LEFT_BIT_OP		(uint16_t)0x5b11
#define AML_COND_REF_OF_OP		(uint16_t)0x5b12
#define AML_CREATE_FIELD_OP		(uint16_t)0x5b13
#define AML_LOAD_TABLE_OP		(uint16_t)0x5b1f /* ACPI 2.0 */
#define AML_LOAD_OP			(uint16_t)0x5b20
#define AML_STALL_OP			(uint16_t)0x5b21
#define AML_SLEEP_OP			(uint16_t)0x5b22
#define AML_ACQUIRE_OP			(uint16_t)0x5b23
#define AML_SIGNAL_OP			(uint16_t)0x5b24
#define AML_WAIT_OP			(uint16_t)0x5b25
#define AML_RESET_OP			(uint16_t)0x5b26
#define AML_RELEASE_OP			(uint16_t)0x5b27
#define AML_FROM_BCD_OP			(uint16_t)0x5b28
#define AML_TO_BCD_OP			(uint16_t)0x5b29
#define AML_UNLOAD_OP			(uint16_t)0x5b2a
#define AML_REVISION_OP			(uint16_t)0x5b30
#define AML_DEBUG_OP			(uint16_t)0x5b31
#define AML_FATAL_OP			(uint16_t)0x5b32
#define AML_TIMER_OP			(uint16_t)0x5b33 /* ACPI 3.0 */
#define AML_REGION_OP			(uint16_t)0x5b80
#define AML_FIELD_OP			(uint16_t)0x5b81
#define AML_DEVICE_OP			(uint16_t)0x5b82
#define AML_PROCESSOR_OP		(uint16_t)0x5b83
#define AML_POWER_RES_OP		(uint16_t)0x5b84
#define AML_THERMAL_ZONE_OP		(uint16_t)0x5b85
#define AML_INDEX_FIELD_OP		(uint16_t)0x5b86
#define AML_BANK_FIELD_OP		(uint16_t)0x5b87
#define AML_DATA_REGION_OP		(uint16_t)0x5b88 /* ACPI 2.0 */


/*
 * Combination opcodes (actually two one-byte opcodes)
 * Used by the disassembler and iASL compiler
 */
#define AML_LGREATEREQUAL_OP		(uint16_t)0x9295
#define AML_LLESSEQUAL_OP		(uint16_t)0x9294
#define AML_LNOTEQUAL_OP		(uint16_t)0x9293


/*
 * Opcodes for "Field" operators
 */
#define AML_FIELD_OFFSET_OP		(uint8_t)0x00
#define AML_FIELD_ACCESS_OP		(uint8_t)0x01
#define AML_FIELD_CONNECTION_OP		(uint8_t)0x02 /* ACPI 5.0 */
#define AML_FIELD_EXT_ACCESS_OP		(uint8_t)0x03 /* ACPI 5.0 */


/*
 * Internal opcodes
 * Use only "Unknown" AML opcodes, don't attempt to use
 * any valid ACPI ASCII values (A-Z, 0-9, '-')
 */
#define AML_INT_NAMEPATH_OP		(uint16_t)0x002d
#define AML_INT_NAMEDFIELD_OP		(uint16_t)0x0030
#define AML_INT_RESERVEDFIELD_OP	(uint16_t)0x0031
#define AML_INT_ACCESSFIELD_OP		(uint16_t)0x0032
#define AML_INT_BYTELIST_OP		(uint16_t)0x0033
#define AML_INT_STATICSTRING_OP		(uint16_t)0x0034
#define AML_INT_METHODCALL_OP		(uint16_t)0x0035
#define AML_INT_RETURN_VALUE_OP		(uint16_t)0x0036
#define AML_INT_EVAL_SUBTREE_OP		(uint16_t)0x0037
#define AML_INT_CONNECTION_OP		(uint16_t)0x0038
#define AML_INT_EXTACCESSFIELD_OP	(uint16_t)0x0039

#define ARG_NONE			0x0

/*
 * Resolved argument types for the AML Interpreter
 * Each field in the ArgTypes UINT32 is 5 bits, allowing for a maximum of 6 arguments.
 * There can be up to 31 unique argument types (0 is end-of-arg-list indicator)
 *
 * Note1: These values are completely independent from the ACPI_TYPEs
 *        i.e., ARGI_INTEGER != ACPI_TYPE_INTEGER
 *
 * Note2: If and when 5 bits becomes insufficient, it would probably be best
 * to convert to a 6-byte array of argument types, allowing 8 bits per argument.
 */

/* Single, simple types */

#define ARGI_ANYTYPE			0x01 /* Don't care */
#define ARGI_PACKAGE			0x02
#define ARGI_EVENT			0x03
#define ARGI_MUTEX			0x04
#define ARGI_DDBHANDLE			0x05

/* Interchangeable types (via implicit conversion) */

#define ARGI_INTEGER			0x06
#define ARGI_STRING			0x07
#define ARGI_BUFFER			0x08
#define ARGI_BUFFER_OR_STRING		0x09 /* Used by MID op only */
#define ARGI_COMPUTEDATA		0x0A /* Buffer, String, or Integer */

/* Reference objects */

#define ARGI_INTEGER_REF		0x0B
#define ARGI_OBJECT_REF			0x0C
#define ARGI_DEVICE_REF			0x0D
#define ARGI_REFERENCE			0x0E
#define ARGI_TARGETREF			0x0F /* Target, subject to implicit conversion */
#define ARGI_FIXED_TARGET		0x10 /* Target, no implicit conversion */
#define ARGI_SIMPLE_TARGET		0x11 /* Name, Local, Arg -- no implicit conversion */

/* Multiple/complex types */

#define ARGI_DATAOBJECT			0x12 /* Buffer, String, package or reference to a Node - Used only by SizeOf operator*/
#define ARGI_COMPLEXOBJ			0x13 /* Buffer, String, or package (Used by INDEX op only) */
#define ARGI_REF_OR_STRING		0x14 /* Reference or String (Used by DEREFOF op only) */
#define ARGI_REGION_OR_BUFFER		0x15 /* Used by LOAD op only */
#define ARGI_DATAREFOBJ			0x16

/* Note: types above can expand to 0x1F maximum */

#define ARGI_INVALID_OPCODE		0xFFFFFFFF

/*
 * Opcode information
 */

/* Opcode flags */

#define AML_LOGICAL                 0x0001
#define AML_LOGICAL_NUMERIC         0x0002
#define AML_MATH                    0x0004
#define AML_CREATE                  0x0008
#define AML_FIELD                   0x0010
#define AML_DEFER                   0x0020
#define AML_NAMED                   0x0040
#define AML_NSNODE                  0x0080
#define AML_NSOPCODE                0x0100
#define AML_NSOBJECT                0x0200
#define AML_HAS_RETVAL              0x0400
#define AML_HAS_TARGET              0x0800
#define AML_HAS_ARGS                0x1000
#define AML_CONSTANT                0x2000
#define AML_NO_OPERAND_RESOLVE      0x4000

/* Convenient flag groupings */

#define AML_FLAGS_EXEC_0A_0T_1R		AML_HAS_RETVAL
#define AML_FLAGS_EXEC_1A_0T_0R		AML_HAS_ARGS /* Monadic1  */
#define AML_FLAGS_EXEC_1A_0T_1R		AML_HAS_ARGS | AML_HAS_RETVAL /* Monadic2  */
#define AML_FLAGS_EXEC_1A_1T_0R		AML_HAS_ARGS | AML_HAS_TARGET
#define AML_FLAGS_EXEC_1A_1T_1R		AML_HAS_ARGS | AML_HAS_TARGET | AML_HAS_RETVAL /* monadic2_r */
#define AML_FLAGS_EXEC_2A_0T_0R		AML_HAS_ARGS /* Dyadic1   */
#define AML_FLAGS_EXEC_2A_0T_1R		AML_HAS_ARGS | AML_HAS_RETVAL /* Dyadic2   */
#define AML_FLAGS_EXEC_2A_1T_1R		AML_HAS_ARGS | AML_HAS_TARGET | AML_HAS_RETVAL /* dyadic2_r  */
#define AML_FLAGS_EXEC_2A_2T_1R		AML_HAS_ARGS | AML_HAS_TARGET | AML_HAS_RETVAL
#define AML_FLAGS_EXEC_3A_0T_0R		AML_HAS_ARGS
#define AML_FLAGS_EXEC_3A_1T_1R		AML_HAS_ARGS | AML_HAS_TARGET | AML_HAS_RETVAL
#define AML_FLAGS_EXEC_6A_0T_1R		AML_HAS_ARGS | AML_HAS_RETVAL

/*
 * The opcode Type is used in a dispatch table, do not change
 * without updating the table.
 */
#define AML_TYPE_EXEC_0A_0T_1R      0x00
#define AML_TYPE_EXEC_1A_0T_0R      0x01	/* Monadic1  */
#define AML_TYPE_EXEC_1A_0T_1R      0x02	/* Monadic2  */
#define AML_TYPE_EXEC_1A_1T_0R      0x03
#define AML_TYPE_EXEC_1A_1T_1R      0x04	/* monadic2_r */
#define AML_TYPE_EXEC_2A_0T_0R      0x05	/* Dyadic1   */
#define AML_TYPE_EXEC_2A_0T_1R      0x06	/* Dyadic2   */
#define AML_TYPE_EXEC_2A_1T_1R      0x07	/* dyadic2_r  */
#define AML_TYPE_EXEC_2A_2T_1R      0x08
#define AML_TYPE_EXEC_3A_0T_0R      0x09
#define AML_TYPE_EXEC_3A_1T_1R      0x0A
#define AML_TYPE_EXEC_6A_0T_1R      0x0B
/* End of types used in dispatch table */

#define AML_TYPE_LITERAL            0x0B
#define AML_TYPE_CONSTANT           0x0C
#define AML_TYPE_METHOD_ARGUMENT    0x0D
#define AML_TYPE_LOCAL_VARIABLE     0x0E
#define AML_TYPE_DATA_TERM          0x0F

/* Generic for an op that returns a value */

#define AML_TYPE_METHOD_CALL        0x10

/* Misc */

#define AML_TYPE_CREATE_FIELD       0x11
#define AML_TYPE_CREATE_OBJECT      0x12
#define AML_TYPE_CONTROL            0x13
#define AML_TYPE_NAMED_NO_OBJ       0x14
#define AML_TYPE_NAMED_FIELD        0x15
#define AML_TYPE_NAMED_SIMPLE       0x16
#define AML_TYPE_NAMED_COMPLEX      0x17
#define AML_TYPE_RETURN             0x18

#define AML_TYPE_UNDEFINED          0x19
#define AML_TYPE_BOGUS              0x1A

/* AML Package Length encodings */

#define ACPI_AML_PACKAGE_TYPE1      0x40
#define ACPI_AML_PACKAGE_TYPE2      0x4000
#define ACPI_AML_PACKAGE_TYPE3      0x400000
#define ACPI_AML_PACKAGE_TYPE4      0x40000000

/*
 * Opcode classes
 */
#define AML_CLASS_EXECUTE           0x00
#define AML_CLASS_CREATE            0x01
#define AML_CLASS_ARGUMENT          0x02
#define AML_CLASS_NAMED_OBJECT      0x03
#define AML_CLASS_CONTROL           0x04
#define AML_CLASS_ASCII             0x05
#define AML_CLASS_PREFIX            0x06
#define AML_CLASS_INTERNAL          0x07
#define AML_CLASS_RETURN_VALUE      0x08
#define AML_CLASS_METHOD_CALL       0x09
#define AML_CLASS_UNKNOWN           0x0A

/* Opcode object flags */

#define ACPI_OPCODE_GENERIC		0x01
#define ACPI_OPCODE_NAMED		0x02
#define ACPI_OPCODE_DEFERRED		0x04
#define ACPI_OPCODE_BYTELIST		0x08
#define ACPI_OPCODE_IN_STACK		0x10
#define ACPI_OPCODE_TARGET		0x20
#define ACPI_OPCODE_IN_CACHE		0x80

#endif /* __ACPI_AML_H_INCLUDE__ */
