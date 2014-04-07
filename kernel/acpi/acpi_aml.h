#ifndef __ACPI_AML_H_INCLUDE__
#define __ACPI_AML_H_INCLUDE__

/* primary opcodes */
#define AML_NULL_CHAR			(uint16_t)0x00
#define AML_ZERO_OP			(uint16_t)0x00
#define AML_ONE_OP			(uint16_t)0x01
#define AML_ALIAS_OP			(uint16_t)0x06
#define AML_NAME_OP			(uint16_t)0x08
#define AML_BYTE_PFX			(uint16_t)0x0a
#define AML_WORD_PFX			(uint16_t)0x0b
#define AML_DWORD_PFX			(uint16_t)0x0c
#define AML_STRING_PFX			(uint16_t)0x0d
#define AML_SCOPE_OP			(uint16_t)0x10
#define AML_BUFFER_OP			(uint16_t)0x11
#define AML_PACKAGE_OP			(uint16_t)0x12
#define AML_METHOD_OP			(uint16_t)0x14
#define AML_LOCAL0			(uint16_t)0x60
#define AML_LOCAL1			(uint16_t)0x61
#define AML_LOCAL2			(uint16_t)0x62
#define AML_LOCAL3			(uint16_t)0x63
#define AML_LOCAL4			(uint16_t)0x64
#define AML_LOCAL5			(uint16_t)0x65
#define AML_LOCAL6			(uint16_t)0x66
#define AML_LOCAL7			(uint16_t)0x67
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
#define AML_NOTIFY_OP			(uint16_t)0x86
#define AML_SIZE_OF_OP			(uint16_t)0x87
#define AML_INDEX_OP			(uint16_t)0x88
#define AML_MATCH_OP			(uint16_t)0x89
#define AML_CREATE_DWORD_FIELD_OP	(uint16_t)0x8a
#define AML_CREATE_WORD_FIELD_OP	(uint16_t)0x8b
#define AML_CREATE_BYTE_FIELD_OP	(uint16_t)0x8c
#define AML_CREATE_BIT_FIELD_OP		(uint16_t)0x8d
#define AML_TYPE_OP			(uint16_t)0x8e
#define AML_LAND_OP			(uint16_t)0x90
#define AML_LOR_OP			(uint16_t)0x91
#define AML_LNOT_OP			(uint16_t)0x92
#define AML_LEQUAL_OP			(uint16_t)0x93
#define AML_LGREATER_OP			(uint16_t)0x94
#define AML_LLESS_OP			(uint16_t)0x95
#define AML_IF_OP			(uint16_t)0xa0
#define AML_ELSE_OP			(uint16_t)0xa1
#define AML_WHILE_OP			(uint16_t)0xa2
#define AML_NOOP_OP			(uint16_t)0xa3
#define AML_RETURN_OP			(uint16_t)0xa4
#define AML_BREAK_OP			(uint16_t)0xa5
#define AML_BREAK_POINT_OP		(uint16_t)0xcc
#define AML_ONES_OP			(uint16_t)0xff
#define AML_QWORD_PFX			(uint16_t)0x0e /* ACPI 2.0 */
#define AML_VAR_PACKAGE_OP		(uint16_t)0x13 /* ACPI 2.0 */
#define AML_CONCAT_RES_OP		(uint16_t)0x84 /* ACPI 2.0 */
#define AML_MOD_OP			(uint16_t)0x85 /* ACPI 2.0 */
#define AML_CREATE_QWORD_FIELD_OP	(uint16_t)0x8f /* ACPI 2.0 */
#define AML_TO_BUFFER_OP		(uint16_t)0x96 /* ACPI 2.0 */
#define AML_TO_DECSTRING_OP		(uint16_t)0x97 /* ACPI 2.0 */
#define AML_TO_HEXSTRING_OP		(uint16_t)0x98 /* ACPI 2.0 */
#define AML_TO_INTEGER_OP		(uint16_t)0x99 /* ACPI 2.0 */
#define AML_TO_STRING_OP		(uint16_t)0x9c /* ACPI 2.0 */
#define AML_COPY_OP			(uint16_t)0x9d /* ACPI 2.0 */
#define AML_MID_OP			(uint16_t)0x9e /* ACPI 2.0 */
#define AML_CONTINUE_OP			(uint16_t)0x9f /* ACPI 2.0 */

/* prefixed opcodes */
#define AML_MUTEX_OP			(uint16_t)0x5b01
#define AML_EVENT_OP			(uint16_t)0x5b02
#define AML_COND_REF_OF_OP		(uint16_t)0x5b12
#define AML_CREATE_FIELD_OP		(uint16_t)0x5b13
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
#define AML_REGION_OP			(uint16_t)0x5b80
#define AML_FIELD_OP			(uint16_t)0x5b81
#define AML_DEVICE_OP			(uint16_t)0x5b82
#define AML_PROCESSOR_OP		(uint16_t)0x5b83
#define AML_POWER_RES_OP		(uint16_t)0x5b84
#define AML_THERMAL_ZONE_OP		(uint16_t)0x5b85
#define AML_INDEX_FIELD_OP		(uint16_t)0x5b86
#define AML_BANK_FIELD_OP		(uint16_t)0x5b87
#define AML_LOAD_TABLE_OP		(uint16_t)0x5b1f /* ACPI 2.0 */
#define AML_DATA_REGION_OP		(uint16_t)0x5b88 /* ACPI 2.0 */
#define AML_TIMER_OP			(uint16_t)0x5b33 /* ACPI 3.0 */

#define AML_EXTENDED_OPCODE		(uint16_t)0x5b00 /* prefix for 2-byte opcodes */
#define AML_DUAL_NAME_PFX		(uint16_t)0x2e
#define AML_MULTI_NAME_PFX		(uint16_t)0x2f
#define AML_ROOT_PFX			(uint16_t)0x5c
#define AML_PARENT_PFX			(uint16_t)0x5e
#define AML_NAME_DIGIT_FIRST		(uint16_t)0x30
#define AML_NAME_DIGIT_LAST		(uint16_t)0x39
#define AML_NAME_CHAR_FIRST		(uint16_t)0x41
#define AML_NAME_CHAR_LAST		(uint16_t)0x5a
#define AML_EXTENDED_OP_PFX		(uint16_t)0x5b

/*
 * Argument types for the AML Parser
 * Each field in the args is 5 bits, allowing for a maximum of 6 arguments.
 * There can be up to 31 unique argument types
 * Zero is reserved as end-of-list indicator
 */
#define AML_NONE			0x00
#define AML_BYTEDATA			0x01
#define AML_DWORDDATA			0x02
#define AML_WORDDATA			0x03
#define AML_QWORDDATA			0x04
#define AML_ASCIICHARLIST		0x05
#define AML_NULLCHAR			0x06
#define AML_BYTELIST			0x07
#define AML_NAMESTRING			0x08
#define AML_PKGLENGTH			0x09
#define AML_PACKAGELEMENTLIST		0x0A
#define AML_SIMPLENAME			0x0B
#define AML_TERMLIST			0x0C
#define AML_DATAREFOBJECT		0x0D
#define AML_OBJECTREFERENCE		0x0E
#define AML_FIELDLIST			0x0F
#define AML_OBJLIST			0x10

#define AML_SUPERNAME(x)		(0x12 + AML_SUPERNAME_##x)
#define AML_SUPERNAME_ANY		0
#define AML_SUPERNAME_TARGET		0x1
#define AML_SUPERNAME_DEVICE		0x2
#define AML_TARGET			AML_SUPERNAME(TARGET)
#define AML_DEVICE			AML_SUPERNAME(DEVICE)

#define AML_TERMARG(x)			(0x16 + AML_TERMARG_##x)
#define AML_TERMARG_ANY			0
#define AML_TERMARG_INTEGER		1
#define AML_TERMARG_BUFFER		2
#define AML_TERMARG_PACKAGE		3
#define AML_TERMARG_DATAREFOBJECT	4
#define AML_TERMARG_COMPUTATIONALDATA	5
#define AML_TERMARG_BUFFSTR		6
#define AML_TERMARG_BUFFPKGSTR		7
#define AML_TERMARG_BYTEDATA		8
#define AML_INTEGERARG			AML_TERMARG(INTEGER)
#define AML_BUFFERARG			AML_TERMARG(BUFFER)
#define AML_PACKAGEARG			AML_TERMARG(PACKAGE)
#define AML_OBJECTARG			AML_TERMARG(DATAREFOBJECT)
#define AML_DATA			AML_TERMARG(COMPUTATIONALDATA)
#define AML_BUFFSTR			AML_TERMARG(BUFFSTR)
#define AML_BUFFPKGSTR			AML_TERMARG(BUFFPKGSTR)
#define AML_BYTEARG			AML_TERMARG(BYTEDATA)
#define AML_LENGTH			AML_INTEGERARG
#define AML_INDEX			AML_INTEGERARG
#define AML_OPERAND			AML_INTEGERARG
#define AML_PREDICATE			AML_INTEGERARG
#define AML_USECTIME			AML_BYTEARG
#define AML_MSECTIME			AML_INTEGERARG
#define AML_MUTEX			AML_TERMARG(ANY)
#define AML_EVENT			AML_TERMARG(ANY)
#define AML_DDBHANDLE			AML_TERMARG(ANY)
#define AML_BCD				AML_INTEGERARG

/*=========================================================================
 * opcode flags
 *=======================================================================*/
#define AML_NAMESPACE_MODIFIER_OBJ_OFFSET	0
#define AML_NAMED_OBJ_OFFSET			1
#define AML_TYPE1_OPCODE_OFFSET			2
#define AML_TYPE2_OPCODE_OFFSET			3
#define AML_DATA_OBJECT_OFFSET			4
#define AML_ARG_OBJ_OFFSET			5
#define AML_LOCAL_OBJ_OFFSET			6
#define AML_COMPUTATIONAL_DATA_OFFSET		7
#define AML_TYPE6_OPCODE_OFFSET			8
#define AML_DEBUG_OBJ_OFFSET			9
#define AML_ARGUMENT_OFFSET			10 /* 3 bits */
#define AML_TARGET_OFFSET			13 /* 2 bits */
#define AML_RETURN_OFFSET			15

#define AML_NAMESPACE_MODIFIER_OBJ		(1 << AML_NAMESPACE_MODIFIER_OBJ_OFFSET)
#define AML_NAMED_OBJ				(1 << AML_NAMED_OBJ_OFFSET)
#define AML_TYPE1_OPCODE			(1 << AML_TYPE1_OPCODE_OFFSET)
#define AML_TYPE2_OPCODE			(1 << AML_TYPE2_OPCODE_OFFSET)
#define AML_DATA_OBJECT				(1 << AML_DATA_OBJECT_OFFSET)
#define AML_ARG_OBJ				(1 << AML_ARG_OBJ_OFFSET)
#define AML_LOCAL_OBJ				(1 << AML_LOCAL_OBJ_OFFSET)
#define AML_COMPUTATIONAL_DATA			(1 << AML_COMPUTATIONAL_DATA_OFFSET)
#define AML_TYPE6_OPCODE			(1 << AML_TYPE6_OPCODE_OFFSET)
#define AML_DEBUG_OBJ				(1 << AML_DEBUG_OBJ_OFFSET)

#define AML_ARGUMENT_MASK			0x0007
#define AML_TARGET_MASK				0x0003
#define AML_RETURN_MASK				0x0001
#define AML_HAS_ARGUMENT(x)			((x) << AML_ARGUMENT_OFFSET)
#define AML_HAS_TARGET(x)			((x) << AML_TARGET_OFFSET)
#define AML_HAS_RETURN(x)			((x) << AML_RETURN_OFFSET)

/* Convenient flag groupings */
#define AML_TERM_OBJ				\
	(AML_NAMESPACE_MODIFIER_OBJ | AML_NAMED_OBJ | AML_TYPE1_OPCODE | AML_TYPE2_OPCODE)
#define AML_TERM_ARG				\
	(AML_TYPE2_OPCODE | AML_DATA_OBJECT | AML_ARG_OBJ | AML_LOCAL_OBJ)

#define AML_FLAGS_EXEC_0A_0T_1R			AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_1A_0T_0R			AML_HAS_ARGUMENT(1)
#define AML_FLAGS_EXEC_1A_0T_1R			AML_HAS_ARGUMENT(1) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_1A_1T_0R			AML_HAS_ARGUMENT(1) | AML_HAS_TARGET(1)
#define AML_FLAGS_EXEC_1A_1T_1R			AML_HAS_ARGUMENT(1) | AML_HAS_TARGET(1) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_2A_0T_0R			AML_HAS_ARGUMENT(2)
#define AML_FLAGS_EXEC_2A_0T_1R			AML_HAS_ARGUMENT(2) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_2A_1T_1R			AML_HAS_ARGUMENT(2) | AML_HAS_TARGET(1) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_2A_2T_1R			AML_HAS_ARGUMENT(2) | AML_HAS_TARGET(2) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_3A_0T_0R			AML_HAS_ARGUMENT(3)
#define AML_FLAGS_EXEC_3A_0T_1R			AML_HAS_ARGUMENT(3) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_3A_1T_1R			AML_HAS_ARGUMENT(3) | AML_HAS_TARGET(1) | AML_HAS_RETURN(1)
#define AML_FLAGS_EXEC_4A_0T_0R			AML_HAS_ARGUMENT(4)
#define AML_FLAGS_EXEC_5A_0T_0R			AML_HAS_ARGUMENT(5)
#define AML_FLAGS_EXEC_6A_0T_0R			AML_HAS_ARGUMENT(6)
#define AML_FLAGS_EXEC_6A_0T_1R			AML_HAS_ARGUMENT(6) | AML_HAS_RETURN(1)

#define AML_FLAGS_EXEC_MASKS				\
	((AML_ARGUMENT_MASK << AML_ARGUMENT_OFFSET) |	\
	 (AML_TARGET_MASK << AML_TARGET_OFFSET) |	\
	 (AML_RETURN_MASK << AML_RETURN_OFFSET))

boolean acpi_opcode_is_opcode(uint16_t opcode);
boolean acpi_opcode_is_namestring(uint16_t opcode);
boolean acpi_opcode_is_namespacemodifierobj(uint16_t opcode);
boolean acpi_opcode_is_namedobj(uint16_t opcode);
boolean acpi_opcode_is_computationaldata(uint16_t opcode);
boolean acpi_opcode_is_dataobject(uint16_t opcode);
boolean acpi_opcode_is_debugobj(uint16_t opcode);
boolean acpi_opcode_is_argobj(uint16_t opcode);
boolean acpi_opcode_is_localobj(uint16_t opcode);
boolean acpi_opcode_is_userterm(uint16_t opcode);
boolean acpi_opcode_is_type1opcode(uint16_t opcode);
boolean acpi_opcode_is_type2opcode(uint16_t opcode);
boolean acpi_opcode_is_type6opcode(uint16_t opcode);
boolean acpi_opcode_is_simplename(uint16_t opcode);
boolean acpi_opcode_is_termarg(uint16_t opcode);
boolean acpi_opcode_is_termobj(uint16_t opcode);
boolean acpi_opcode_is_supername(uint16_t opcode);

#endif /* __ACPI_AML_H_INCLUDE__ */
