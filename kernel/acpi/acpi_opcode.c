/*
 * ZETALOG's Personal COPYRIGHT v2
 *
 * Copyright (c) 2014
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 2. Permission of reuse of souce code only granted to ZETALOG and the
 *    developer(s) in the companies ZETALOG worked and has redistributed this
 *    software to.  Permission of redistribution of source code only granted
 *    to ZETALOG.
 * 3. Permission of redistribution and/or reuse of binary fully granted
 *    to ZETALOG and the companies ZETALOG worked and has redistributed this
 *    software to.
 * 4. Any modification of this software in the redistributed companies need
 *    not be published to ZETALOG.
 * 5. All source code modifications linked/included this software and modified
 *    by ZETALOG are of ZETALOG's personal COPYRIGHT unless the above COPYRIGHT
 *    is no long disclaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)acpi_opcode.c: ACPI opcode information implementation
 * $Id: acpi_opcode.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "acpi_int.h"

/*
 * Numbers of opcodes are extracted from LastIndex+1 of
 * acpi_gbl_opcode_info.
 */
#define NUM_PRIMARY_OPCODES		0x53
#define NUM_EXTENDED_OPCODES		0x1D

#define MAX_PRIMARY_OPCODE		0xFF
#define MAX_EXTENDED_OPCODE		0x88
#define NUM_PRIMARY_OPCODE		(MAX_PRIMARY_OPCODE + 1)
#define NUM_EXTENDED_OPCODE		(MAX_EXTENDED_OPCODE + 1)

/* Used for non-assigned opcodes */
#define _UNK				(NUM_PRIMARY_OPCODES + NUM_EXTENDED_OPCODES)

/*
 * Reserved ASCII characters. Do not use any of these for
 * internal opcodes, since they are used to differentiate
 * name strings from AML opcodes
 */
#define _ASC				(_UNK+1)
#define _PFX				(_UNK+2)

#define NUM_RESERVED_OPCODES		(0x03)

/* Total number of aml opcodes defined */
#define AML_NUM_OPCODES			\
	(NUM_PRIMARY_OPCODES + NUM_EXTENDED_OPCODES + NUM_RESERVED_OPCODES)

#define ACPI_OP(name, args, flags)		\
	{ (name), (uint32_t)(args), (uint32_t)(flags) }

#define AML_TYPE_WIDTH			5
#define AML_ARG_1(x)			((uint32_t)(x))
#define AML_ARG_2(x)			((uint32_t)(x) << (1 * AML_TYPE_WIDTH))
#define AML_ARG_3(x)			((uint32_t)(x) << (2 * AML_TYPE_WIDTH))
#define AML_ARG_4(x)			((uint32_t)(x) << (3 * AML_TYPE_WIDTH))
#define AML_ARG_5(x)			((uint32_t)(x) << (4 * AML_TYPE_WIDTH))
#define AML_ARG_6(x)			((uint32_t)(x) << (5 * AML_TYPE_WIDTH))

#define AML_ARGS1(a)			(AML_ARG_1(a))
#define AML_ARGS2(a, b)			(AML_ARG_1(a)|AML_ARG_2(b))
#define AML_ARGS3(a, b, c)		(AML_ARG_1(a)|AML_ARG_2(b)|AML_ARG_3(c))
#define AML_ARGS4(a, b, c, d)		(AML_ARG_1(a)|AML_ARG_2(b)|AML_ARG_3(c)|AML_ARG_4(d))
#define AML_ARGS5(a, b, c, d, e)	(AML_ARG_1(a)|AML_ARG_2(b)|AML_ARG_3(c)|AML_ARG_4(d)|AML_ARG_5(e))
#define AML_ARGS6(a, b, c, d, e, f)	(AML_ARG_1(a)|AML_ARG_2(b)|AML_ARG_3(c)|AML_ARG_4(d)|AML_ARG_5(e)|AML_ARG_6(f))

#define AML_GET_ARG_TYPE(args, index)	((uint8_t)(((args) >> ((uint32_t)AML_TYPE_WIDTH * (index))) & ((uint32_t) 0x1F)))

/*
 * All AML opcodes and the parse-time arguments for each. Used by the AML
 * parser  Each list is compressed into a 32-bit number and stored in the
 * master opcode table (in psopcode.c).
 */
#define AML_ZERO_ARGS			AML_NONE
#define AML_ONE_ARGS			AML_NONE
#define AML_ALIAS_ARGS			AML_ARGS2(AML_NAMESTRING, AML_NAMESTRING)
#define AML_NAME_ARGS			AML_ARGS2(AML_NAMESTRING, AML_DATAREFOBJECT)
#define AML_BYTE_ARGS			AML_ARGS1(AML_BYTEDATA)
#define AML_WORD_ARGS			AML_ARGS1(AML_WORDDATA)
#define AML_DWORD_ARGS			AML_ARGS1(AML_DWORDDATA)
#define AML_STRING_ARGS			AML_ARGS2(AML_ASCIICHARLIST, AML_NULLCHAR)
#define AML_SCOPE_ARGS			AML_ARGS3(AML_PKGLENGTH, AML_NAMESTRING, AML_TERMLIST)
#define AML_BUFFER_ARGS			AML_ARGS3(AML_PKGLENGTH, AML_LENGTH, AML_BYTELIST)
#define AML_PACKAGE_ARGS		AML_ARGS3(AML_PKGLENGTH, AML_BYTEDATA, AML_PACKAGELEMENTLIST)
#define AML_METHOD_ARGS			AML_ARGS4(AML_PKGLENGTH, AML_NAMESTRING, AML_BYTEDATA, AML_TERMLIST)
#define AML_LOCAL0_ARGS			AML_NONE
#define AML_LOCAL1_ARGS			AML_NONE
#define AML_LOCAL2_ARGS			AML_NONE
#define AML_LOCAL3_ARGS			AML_NONE
#define AML_LOCAL4_ARGS			AML_NONE
#define AML_LOCAL5_ARGS			AML_NONE
#define AML_LOCAL6_ARGS			AML_NONE
#define AML_LOCAL7_ARGS			AML_NONE
#define AML_ARG0_ARGS			AML_NONE
#define AML_ARG1_ARGS			AML_NONE
#define AML_ARG2_ARGS			AML_NONE
#define AML_ARG3_ARGS			AML_NONE
#define AML_ARG4_ARGS			AML_NONE
#define AML_ARG5_ARGS			AML_NONE
#define AML_ARG6_ARGS			AML_NONE
#define AML_STORE_ARGS			AML_ARGS2(AML_TERMARG(ANY), AML_SUPERNAME(ANY))
#define AML_REF_OF_ARGS			AML_ARGS1(AML_SUPERNAME(ANY))
#define AML_ADD_ARGS			AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_CONCAT_ARGS			AML_ARGS3(AML_DATA, AML_DATA, AML_TARGET)
#define AML_SUBTRACT_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_INCREMENT_ARGS		AML_ARGS1(AML_SUPERNAME(ANY))
#define AML_DECREMENT_ARGS		AML_ARGS1(AML_SUPERNAME(ANY))
#define AML_MULTIPLY_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_DIVIDE_ARGS			AML_ARGS4(AML_OPERAND, AML_OPERAND, AML_TARGET, AML_TARGET)
#define AML_SHIFT_LEFT_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_SHIFT_RIGHT_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_AND_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_NAND_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_OR_ARGS			AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_NOR_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_XOR_ARGS		AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_BIT_NOT_ARGS		AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_FIND_SET_LEFT_BIT_ARGS	AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_FIND_SET_RIGHT_BIT_ARGS	AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_DEREF_OF_ARGS		AML_ARGS1(AML_OBJECTREFERENCE)
#define AML_NOTIFY_ARGS			AML_ARGS2(AML_DEVICE, AML_INDEX)
#define AML_SIZE_OF_ARGS		AML_ARGS1(AML_SUPERNAME(ANY))
#define AML_INDEX_ARGS			AML_ARGS3(AML_BUFFPKGSTR, AML_INDEX, AML_TARGET)
#define AML_MATCH_ARGS			AML_ARGS6(AML_PACKAGEARG, AML_BYTEDATA, AML_OPERAND, AML_BYTEDATA, AML_OPERAND, AML_INDEX)
#define AML_CREATE_DWORD_FIELD_ARGS	AML_ARGS3(AML_BUFFERARG, AML_INDEX, AML_NAMESTRING)
#define AML_CREATE_WORD_FIELD_ARGS	AML_ARGS3(AML_BUFFERARG, AML_INDEX, AML_NAMESTRING)
#define AML_CREATE_BYTE_FIELD_ARGS	AML_ARGS3(AML_BUFFERARG, AML_INDEX, AML_NAMESTRING)
#define AML_CREATE_BIT_FIELD_ARGS	AML_ARGS3(AML_BUFFERARG, AML_INDEX, AML_NAMESTRING)
#define AML_TYPE_ARGS			AML_ARGS1(AML_SUPERNAME(ANY))
#define AML_LAND_ARGS			AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LOR_ARGS			AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LNOT_ARGS			AML_ARGS1(AML_OPERAND)
#define AML_LEQUAL_ARGS			AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LGREATER_ARGS		AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LLESS_ARGS			AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_IF_ARGS			AML_ARGS4(AML_PKGLENGTH, AML_PREDICATE, AML_TERMLIST, AML_TERMLIST)
#define AML_ELSE_ARGS			AML_ARGS2(AML_PKGLENGTH, AML_TERMLIST)
#define AML_WHILE_ARGS			AML_ARGS3(AML_PKGLENGTH, AML_PREDICATE, AML_TERMLIST)
#define AML_NOOP_ARGS			AML_NONE
#define AML_RETURN_ARGS			AML_ARGS1(AML_OBJECTARG)
#define AML_BREAK_ARGS			AML_NONE
#define AML_BREAK_POINT_ARGS		AML_NONE
#define AML_ONES_ARGS			AML_NONE
#define AML_QWORD_ARGS			AML_ARGS1(AML_QWORDDATA)
#define AML_VAR_PACKAGE_ARGS		AML_ARGS3(AML_PKGLENGTH, AML_LENGTH, AML_PACKAGELEMENTLIST)
#define AML_CONCAT_RES_ARGS		AML_ARGS3(AML_BUFFERARG, AML_BUFFERARG, AML_TARGET)
#define AML_MOD_ARGS			AML_ARGS3(AML_OPERAND, AML_OPERAND, AML_TARGET)
#define AML_CREATE_QWORD_FIELD_ARGS	AML_ARGS3(AML_BUFFERARG, AML_INDEX, AML_NAMESTRING)
#define AML_TO_BUFFER_ARGS		AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_TO_DEC_STR_ARGS		AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_TO_HEX_STR_ARGS		AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_TO_INTEGER_ARGS		AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_TO_STRING_ARGS		AML_ARGS3(AML_TERMARG(ANY), AML_LENGTH, AML_TARGET)
#define AML_COPY_ARGS			AML_ARGS2(AML_TERMARG(ANY), AML_SIMPLENAME)
#define AML_MID_ARGS			AML_ARGS4(AML_BUFFSTR, AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TARGET)
#define AML_CONTINUE_ARGS		AML_NONE

#if 0
#define AML_LGREATEREQUAL_ARGS		AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LLESSEQUAL_ARGS		AML_ARGS2(AML_OPERAND, AML_OPERAND)
#define AML_LNOTEQUAL_ARGS		AML_ARGS2(AML_OPERAND, AML_OPERAND)
#endif

#define AML_MUTEX_ARGS			AML_ARGS2(AML_NAMESTRING, AML_BYTEDATA)
#define AML_EVENT_ARGS			AML_ARGS1(AML_NAMESTRING)
#define AML_COND_REF_OF_ARGS		AML_ARGS2(AML_SUPERNAME(ANY), AML_TARGET)
#define AML_CREATE_FIELD_ARGS		AML_ARGS4(AML_BUFFERARG, AML_INDEX, AML_LENGTH, AML_NAMESTRING)
#define AML_LOAD_ARGS			AML_ARGS2(AML_NAMESTRING, AML_DDBHANDLE)
#define AML_STALL_ARGS			AML_ARGS1(AML_USECTIME)
#define AML_SLEEP_ARGS			AML_ARGS1(AML_MSECTIME)
#define AML_ACQUIRE_ARGS		AML_ARGS2(AML_MUTEX, AML_WORDDATA)
#define AML_SIGNAL_ARGS			AML_ARGS1(AML_EVENT)
#define AML_WAIT_ARGS			AML_ARGS2(AML_EVENT, AML_OPERAND)
#define AML_RESET_ARGS			AML_ARGS1(AML_EVENT)
#define AML_RELEASE_ARGS		AML_ARGS1(AML_MUTEX)
#define AML_FROM_BCD_ARGS		AML_ARGS2(AML_BCD, AML_TARGET)
#define AML_TO_BCD_ARGS			AML_ARGS2(AML_OPERAND, AML_TARGET)
#define AML_UNLOAD_ARGS			AML_ARGS1(AML_DDBHANDLE)
#define AML_REVISION_ARGS		AML_NONE
#define AML_DEBUG_ARGS			AML_NONE
#define AML_FATAL_ARGS			AML_ARGS3(AML_BYTEDATA, AML_DWORDDATA, AML_INDEX)
#define AML_REGION_ARGS			AML_ARGS4(AML_NAMESTRING, AML_BYTEDATA, AML_INDEX, AML_LENGTH)
#define AML_FIELD_ARGS			AML_ARGS4(AML_PKGLENGTH, AML_NAMESTRING, AML_BYTEDATA, AML_FIELDLIST)
#define AML_DEVICE_ARGS			AML_ARGS3(AML_PKGLENGTH, AML_NAMESTRING, AML_OBJLIST)
#define AML_PROCESSOR_ARGS		AML_ARGS6(AML_PKGLENGTH, AML_NAMESTRING, AML_BYTEDATA, AML_DWORDDATA, AML_BYTEDATA, AML_OBJLIST)
#define AML_POWER_RES_ARGS		AML_ARGS5(AML_PKGLENGTH, AML_NAMESTRING, AML_BYTEDATA, AML_WORDDATA, AML_OBJLIST)
#define AML_THERMAL_ZONE_ARGS		AML_ARGS3(AML_PKGLENGTH, AML_NAMESTRING, AML_OBJLIST)
#define AML_INDEX_FIELD_ARGS		AML_ARGS5(AML_PKGLENGTH, AML_NAMESTRING, AML_NAMESTRING, AML_BYTEDATA, AML_FIELDLIST)
#define AML_BANK_FIELD_ARGS		AML_ARGS6(AML_PKGLENGTH, AML_NAMESTRING, AML_NAMESTRING, AML_INDEX, AML_BYTEDATA, AML_FIELDLIST)
#define AML_LOAD_TABLE_ARGS		AML_ARGS6(AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TERMARG(ANY))
#define AML_DATA_REGION_ARGS		AML_ARGS4(AML_NAMESTRING, AML_TERMARG(ANY), AML_TERMARG(ANY), AML_TERMARG(ANY))
#define AML_TIMER_ARGS			AML_NONE

static boolean acpi_opcode_is_type(uint16_t opcode, uint16_t type);
static boolean acpi_opcode_info_index(uint16_t opcode);

/*
 * Master Opcode information table. A summary of everything we know about each
 * opcode, all in one place.
 */
const struct acpi_opcode_info acpi_gbl_opcode_info[AML_NUM_OPCODES] =
{
/* Index          Name                     Interpreter Args             Flags */
/* ACPI 1.0 opcodes */
/* 00 */ ACPI_OP("Zero",                   AML_ZERO_ARGS,               AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_0A_0T_1R),
/* 01 */ ACPI_OP("One",                    AML_ONE_ARGS,                AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_0A_0T_1R),
/* 02 */ ACPI_OP("Alias",                  AML_ALIAS_ARGS,              AML_NAMESPACE_MODIFIER_OBJ | AML_FLAGS_EXEC_2A_0T_0R),
/* 03 */ ACPI_OP("Name",                   AML_NAME_ARGS,               AML_NAMESPACE_MODIFIER_OBJ | AML_FLAGS_EXEC_2A_0T_0R),
/* 04 */ ACPI_OP("ByteConst",              AML_BYTE_ARGS,               AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_1A_0T_1R),
/* 05 */ ACPI_OP("WordConst",              AML_WORD_ARGS,               AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_1A_0T_1R),
/* 06 */ ACPI_OP("DwordConst",             AML_DWORD_ARGS,              AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_1A_0T_1R),
/* 07 */ ACPI_OP("String",                 AML_STRING_ARGS,             AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_2A_0T_1R),
/* 08 */ ACPI_OP("Scope",                  AML_SCOPE_ARGS,              AML_NAMESPACE_MODIFIER_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 09 */ ACPI_OP("Buffer",                 AML_BUFFER_ARGS,             AML_TYPE2_OPCODE | AML_COMPUTATIONAL_DATA | AML_DATA_OBJECT | AML_FLAGS_EXEC_3A_0T_1R),
/* 0A */ ACPI_OP("Package",                AML_PACKAGE_ARGS,            AML_TYPE2_OPCODE | AML_DATA_OBJECT | AML_FLAGS_EXEC_3A_0T_1R),
/* 0B */ ACPI_OP("Method",                 AML_METHOD_ARGS,             AML_NAMED_OBJ | AML_FLAGS_EXEC_4A_0T_0R),
/* 0C */ ACPI_OP("Local0",                 AML_LOCAL0_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 0D */ ACPI_OP("Local1",                 AML_LOCAL1_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 0E */ ACPI_OP("Local2",                 AML_LOCAL2_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 0F */ ACPI_OP("Local3",                 AML_LOCAL3_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 10 */ ACPI_OP("Local4",                 AML_LOCAL4_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 11 */ ACPI_OP("Local5",                 AML_LOCAL5_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 12 */ ACPI_OP("Local6",                 AML_LOCAL6_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 13 */ ACPI_OP("Local7",                 AML_LOCAL7_ARGS,             AML_LOCAL_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 14 */ ACPI_OP("Arg0",                   AML_ARG0_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 15 */ ACPI_OP("Arg1",                   AML_ARG1_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 16 */ ACPI_OP("Arg2",                   AML_ARG2_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 17 */ ACPI_OP("Arg3",                   AML_ARG3_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 18 */ ACPI_OP("Arg4",                   AML_ARG4_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 19 */ ACPI_OP("Arg5",                   AML_ARG5_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 1A */ ACPI_OP("Arg6",                   AML_ARG6_ARGS,               AML_ARG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 1B */ ACPI_OP("Store",                  AML_STORE_ARGS,              AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 1C */ ACPI_OP("RefOf",                  AML_REF_OF_ARGS,             AML_TYPE2_OPCODE | AML_TYPE6_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 1D */ ACPI_OP("Add",                    AML_ADD_ARGS,                AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 1E */ ACPI_OP("Concatenate",            AML_CONCAT_ARGS,             AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 1F */ ACPI_OP("Subtract",               AML_SUBTRACT_ARGS,           AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 20 */ ACPI_OP("Increment",              AML_INCREMENT_ARGS,          AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 21 */ ACPI_OP("Decrement",              AML_DECREMENT_ARGS,          AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 22 */ ACPI_OP("Multiply",               AML_MULTIPLY_ARGS,           AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 23 */ ACPI_OP("Divide",                 AML_DIVIDE_ARGS,             AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_2T_1R),
/* 24 */ ACPI_OP("ShiftLeft",              AML_SHIFT_LEFT_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 25 */ ACPI_OP("ShiftRight",             AML_SHIFT_RIGHT_ARGS,        AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 26 */ ACPI_OP("And",                    AML_BIT_AND_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 27 */ ACPI_OP("NAnd",                   AML_BIT_NAND_ARGS,           AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 28 */ ACPI_OP("Or",                     AML_BIT_OR_ARGS,             AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 29 */ ACPI_OP("NOr",                    AML_BIT_NOR_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 2A */ ACPI_OP("XOr",                    AML_BIT_XOR_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 2B */ ACPI_OP("Not",                    AML_BIT_NOT_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 2C */ ACPI_OP("FindSetLeftBit",         AML_FIND_SET_LEFT_BIT_ARGS,  AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 2D */ ACPI_OP("FindSetRightBit",        AML_FIND_SET_RIGHT_BIT_ARGS, AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 2E */ ACPI_OP("DerefOf",                AML_DEREF_OF_ARGS,           AML_TYPE2_OPCODE | AML_TYPE6_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 2F */ ACPI_OP("Notify",                 AML_NOTIFY_ARGS,             AML_TYPE1_OPCODE | AML_FLAGS_EXEC_2A_0T_0R),
/* 30 */ ACPI_OP("SizeOf",                 AML_SIZE_OF_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 31 */ ACPI_OP("Index",                  AML_INDEX_ARGS,              AML_TYPE2_OPCODE | AML_TYPE6_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 32 */ ACPI_OP("Match",                  AML_MATCH_ARGS,              AML_TYPE2_OPCODE | AML_FLAGS_EXEC_6A_0T_1R),
/* 33 */ ACPI_OP("CreateDWordField",       AML_CREATE_DWORD_FIELD_ARGS, AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 34 */ ACPI_OP("CreateWordField",        AML_CREATE_WORD_FIELD_ARGS,  AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 35 */ ACPI_OP("CreateByteField",        AML_CREATE_BYTE_FIELD_ARGS,  AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 36 */ ACPI_OP("CreateBitField",         AML_CREATE_BIT_FIELD_ARGS,   AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 37 */ ACPI_OP("ObjectType",             AML_TYPE_ARGS,               AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 38 */ ACPI_OP("LAnd",                   AML_LAND_ARGS,               AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 39 */ ACPI_OP("LOr",                    AML_LOR_ARGS,                AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 3A */ ACPI_OP("LNot",                   AML_LNOT_ARGS,               AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_0T_1R),
/* 3B */ ACPI_OP("LEqual",                 AML_LEQUAL_ARGS,             AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 3C */ ACPI_OP("LGreater",               AML_LGREATER_ARGS,           AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 3D */ ACPI_OP("LLess",                  AML_LLESS_ARGS,              AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 3E */ ACPI_OP("If",                     AML_IF_ARGS,                 AML_TYPE1_OPCODE | AML_FLAGS_EXEC_4A_0T_0R),
/* 3F */ ACPI_OP("Else",                   AML_ELSE_ARGS,               AML_TYPE1_OPCODE | AML_FLAGS_EXEC_2A_0T_0R),
/* 40 */ ACPI_OP("While",                  AML_WHILE_ARGS,              AML_TYPE1_OPCODE | AML_FLAGS_EXEC_3A_0T_0R),
/* 41 */ ACPI_OP("Noop",                   AML_NOOP_ARGS,               AML_TYPE1_OPCODE),
/* 42 */ ACPI_OP("Return",                 AML_RETURN_ARGS,             AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 43 */ ACPI_OP("Break",                  AML_BREAK_ARGS,              AML_TYPE1_OPCODE),
/* 44 */ ACPI_OP("BreakPoint",             AML_BREAK_POINT_ARGS,        AML_TYPE1_OPCODE),
/* 45 */ ACPI_OP("Ones",                   AML_ONES_ARGS,               AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_0A_0T_1R),
/* ACPI 2.0 opcodes */
/* 46 */ ACPI_OP("QwordConst",             AML_QWORD_ARGS,              AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_1A_0T_1R),
/* 47 */ ACPI_OP("Package", /* Var */      AML_VAR_PACKAGE_ARGS,        AML_TYPE2_OPCODE | AML_DATA_OBJECT | AML_FLAGS_EXEC_3A_0T_1R),
/* 48 */ ACPI_OP("ConcatenateResTemplate", AML_CONCAT_RES_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 49 */ ACPI_OP("Mod",                    AML_MOD_ARGS,                AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 4A */ ACPI_OP("CreateQWordField",       AML_CREATE_QWORD_FIELD_ARGS, AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 4B */ ACPI_OP("ToBuffer",               AML_TO_BUFFER_ARGS,          AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 4C */ ACPI_OP("ToDecimalString",        AML_TO_DEC_STR_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 4D */ ACPI_OP("ToHexString",            AML_TO_HEX_STR_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 4E */ ACPI_OP("ToInteger",              AML_TO_INTEGER_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 4F */ ACPI_OP("ToString",               AML_TO_STRING_ARGS,          AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_1T_1R),
/* 50 */ ACPI_OP("CopyObject",             AML_COPY_ARGS,               AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 51 */ ACPI_OP("Mid",                    AML_MID_ARGS,                AML_TYPE2_OPCODE | AML_FLAGS_EXEC_3A_1T_1R),
/* 52 */ ACPI_OP("Continue",               AML_CONTINUE_ARGS,           AML_TYPE1_OPCODE),

/* Prefixed opcodes (Two-byte opcodes with a prefix op) */
/* ACPI 1.0 opcodes */
/* 00 */ ACPI_OP("Mutex",                  AML_MUTEX_ARGS,              AML_NAMED_OBJ | AML_FLAGS_EXEC_2A_0T_0R),
/* 01 */ ACPI_OP("Event",                  AML_EVENT_ARGS,              AML_NAMED_OBJ | AML_FLAGS_EXEC_1A_0T_0R),
/* 02 */ ACPI_OP("CondRefOf",              AML_COND_REF_OF_ARGS,        AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 03 */ ACPI_OP("CreateField",            AML_CREATE_FIELD_ARGS,       AML_NAMED_OBJ | AML_FLAGS_EXEC_4A_0T_0R),
/* 04 */ ACPI_OP("Load",                   AML_LOAD_ARGS,               AML_TYPE1_OPCODE | AML_FLAGS_EXEC_2A_0T_0R),
/* 05 */ ACPI_OP("Stall",                  AML_STALL_ARGS,              AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 06 */ ACPI_OP("Sleep",                  AML_SLEEP_ARGS,              AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 07 */ ACPI_OP("Acquire",                AML_ACQUIRE_ARGS,            AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 08 */ ACPI_OP("Signal",                 AML_SIGNAL_ARGS,             AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 09 */ ACPI_OP("Wait",                   AML_WAIT_ARGS,               AML_TYPE2_OPCODE | AML_FLAGS_EXEC_2A_0T_1R),
/* 0A */ ACPI_OP("Reset",                  AML_RESET_ARGS,              AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 0B */ ACPI_OP("Release",                AML_RELEASE_ARGS,            AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 0C */ ACPI_OP("FromBCD",                AML_FROM_BCD_ARGS,           AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 0D */ ACPI_OP("ToBCD",                  AML_TO_BCD_ARGS,             AML_TYPE2_OPCODE | AML_FLAGS_EXEC_1A_1T_1R),
/* 0E */ ACPI_OP("Unload",                 AML_UNLOAD_ARGS,             AML_TYPE1_OPCODE | AML_FLAGS_EXEC_1A_0T_0R),
/* 0F */ ACPI_OP("Revision",               AML_REVISION_ARGS,           AML_DATA_OBJECT | AML_COMPUTATIONAL_DATA | AML_FLAGS_EXEC_0A_0T_1R),
/* 10 */ ACPI_OP("Debug",                  AML_DEBUG_ARGS,              AML_DEBUG_OBJ | AML_FLAGS_EXEC_0A_0T_1R),
/* 11 */ ACPI_OP("Fatal",                  AML_FATAL_ARGS,              AML_TYPE1_OPCODE | AML_FLAGS_EXEC_3A_0T_0R),
/* 12 */ ACPI_OP("OperationRegion",        AML_REGION_ARGS,             AML_NAMED_OBJ | AML_FLAGS_EXEC_4A_0T_0R),
/* 13 */ ACPI_OP("Field",                  AML_FIELD_ARGS,              AML_NAMED_OBJ | AML_FLAGS_EXEC_4A_0T_0R),
/* 14 */ ACPI_OP("Device",                 AML_DEVICE_ARGS,             AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 15 */ ACPI_OP("Processor",              AML_PROCESSOR_ARGS,          AML_NAMED_OBJ | AML_FLAGS_EXEC_5A_0T_0R),
/* 16 */ ACPI_OP("PowerResource",          AML_POWER_RES_ARGS,          AML_NAMED_OBJ | AML_FLAGS_EXEC_5A_0T_0R),
/* 17 */ ACPI_OP("ThermalZone",            AML_THERMAL_ZONE_ARGS,       AML_NAMED_OBJ | AML_FLAGS_EXEC_3A_0T_0R),
/* 18 */ ACPI_OP("IndexField",             AML_INDEX_FIELD_ARGS,        AML_NAMED_OBJ | AML_FLAGS_EXEC_5A_0T_0R),
/* 19 */ ACPI_OP("BankField",              AML_BANK_FIELD_ARGS,         AML_NAMED_OBJ | AML_FLAGS_EXEC_6A_0T_0R),
/* ACPI 2.0 opcodes */
/* 1A */ ACPI_OP("LoadTable",              AML_LOAD_TABLE_ARGS,         AML_TYPE2_OPCODE | AML_FLAGS_EXEC_6A_0T_1R),
/* 1B */ ACPI_OP("DataTableRegion",        AML_DATA_REGION_ARGS,        AML_NAMED_OBJ | AML_FLAGS_EXEC_4A_0T_0R),
/* ACPI 3.0 opcodes */
/* 1C */ ACPI_OP("Timer",                  AML_TIMER_ARGS,              AML_TYPE2_OPCODE | AML_FLAGS_EXEC_0A_0T_1R),

/* Internal indexed information */
/* 01 */ ACPI_OP("--Unknown--",            0,                           0),
/* 02 */ ACPI_OP("--ASCII--",              0,                           0),
/* 03 */ ACPI_OP("--Prefix--",             0,                           0),
};

/*
 * This table is directly indexed by the opcodes It returns
 * an index into the opcode table (acpi_gbl_opcode_info)
 */
const uint8_t acpi_gbl_short_opcode_indexes[256] =
{
/*              0     1     2     3     4     5     6     7  */
/*              8     9     A     B     C     D     E     F  */
/* 0x00 */    0x00, 0x01, _UNK, _UNK, _UNK, _UNK, 0x02, _UNK,
/* 0x08 */    0x03, _UNK, 0x04, 0x05, 0x06, 0x07, 0x46, _UNK,
/* 0x10 */    0x08, 0x09, 0x0a, 0x47, 0x0b, _UNK, _UNK, _UNK,
/* 0x18 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x20 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x28 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _PFX, _PFX,
/* 0x30 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, 0x7D,
/* 0x38 */    0x7F, 0x80, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x40 */    _UNK, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x48 */    _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x50 */    _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC, _ASC,
/* 0x58 */    _ASC, _ASC, _ASC, _UNK, _PFX, _UNK, _PFX, _ASC,
/* 0x60 */    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
/* 0x68 */    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, _UNK,
/* 0x70 */    0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22,
/* 0x78 */    0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
/* 0x80 */    0x2b, 0x2c, 0x2d, 0x2e, 0x48, 0x49, 0x2f, 0x30,
/* 0x88 */    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x4A,
/* 0x90 */    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x4B, 0x4C,
/* 0x98 */    0x4D, 0x4E, _UNK, _UNK, 0x4F, 0x50, 0x51, 0x52,
/* 0xA0 */    0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, _UNK, _UNK,
/* 0xA8 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
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
 * pair. It returns an index into the opcode table (acpi_gbl_opcode_info)
 */
const uint8_t acpi_gbl_long_opcode_indexes[NUM_EXTENDED_OPCODE] =
{
/*              0     1     2     3     4     5     6     7  */
/*              8     9     A     B     C     D     E     F  */
/* 0x00 */    _UNK, 0x00, 0x01, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x08 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x10 */    _UNK, _UNK, 0x02, 0x03, _UNK, _UNK, _UNK, _UNK,
/* 0x18 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, 0x1A,
/* 0x20 */    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
/* 0x28 */    0x0C, 0x0D, 0x0E, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x30 */    0x0F, 0x10, 0x11, 0x1C, _UNK, _UNK, _UNK, _UNK,
/* 0x38 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x40 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x48 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x50 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x58 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x60 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x68 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x70 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x78 */    _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK, _UNK,
/* 0x80 */    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
/* 0x88 */    0x1B,
};

static boolean acpi_opcode_info_index(uint16_t opcode)
{
	uint8_t op_index = _UNK;

	if (!(opcode & 0xFF00))
		op_index = acpi_gbl_short_opcode_indexes[(uint8_t)opcode];
	if ((opcode & 0xFF00) == AML_EXTENDED_OPCODE &&
	    (opcode & 0x00FF) <= MAX_EXTENDED_OPCODE)
		op_index = acpi_gbl_long_opcode_indexes[(uint8_t)(opcode & 0x00FF)] +
			   NUM_PRIMARY_OPCODES;

	return op_index;
}

const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode)
{
	uint8_t op_index = acpi_opcode_info_index(opcode);

	return (&acpi_gbl_opcode_info[op_index]);
}

union acpi_term *acpi_term_alloc(uint16_t opcode)
{
	union acpi_term *op;
	const struct acpi_opcode_info *op_info;

	op_info = acpi_opcode_get_info(opcode);

	/* Allocate the minimum required size object */
	if (op_info->flags & AML_NAMED_OBJ)
		op = acpi_os_allocate_zeroed(sizeof (struct acpi_named_obj));
	else
		op = acpi_os_allocate_zeroed(sizeof (struct acpi_term_obj));
	if (op) {
		op->common.descriptor_type = ACPI_DESC_TYPE_TERM;
		op->common.aml_opcode = opcode;
	}
	
	return op;
}

void acpi_term_free(union acpi_term *op)
{
	acpi_os_free(op);
}

static boolean acpi_opcode_is_type(uint16_t opcode, uint16_t type)
{
	const struct acpi_opcode_info *op_info;

	op_info = acpi_opcode_get_info(opcode);

	return (op_info && op_info->flags & type) ? true : false;
}

boolean acpi_opcode_is_opcode(uint16_t opcode)
{
	uint8_t op_index = acpi_opcode_info_index(opcode);

	return (op_index == _UNK || op_index == _ASC || op_index == _PFX) ?
	       false : true;
}

boolean acpi_opcode_is_namestring(uint16_t opcode)
{
	uint8_t op_index = acpi_opcode_info_index(opcode);

	return (op_index == _ASC || op_index == _PFX) ? true : false;
}

boolean acpi_opcode_is_namedobj(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_NAMED_OBJ))
		return true;

	return false;
}

boolean acpi_opcode_is_namespacemodifierobj(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_NAMESPACE_MODIFIER_OBJ))
		return true;

	return false;
}

boolean acpi_opcode_is_computationaldata(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_COMPUTATIONAL_DATA))
		return true;

	return false;
}

boolean acpi_opcode_is_dataobject(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_DATA_OBJECT))
		return true;

	return false;
}

boolean acpi_opcode_is_debugobj(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_DEBUG_OBJ))
		return true;

	return false;
}

boolean acpi_opcode_is_argobj(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_ARG_OBJ))
		return true;

	return false;
}

boolean acpi_opcode_is_localobj(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_LOCAL_OBJ))
		return true;

	return false;
}

boolean acpi_opcode_is_type1opcode(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_TYPE1_OPCODE))
		return true;

	return false;
}

boolean acpi_opcode_is_type2opcode(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_TYPE2_OPCODE) ||
	    acpi_opcode_is_userterm(opcode))
		return true;

	return false;
}

boolean acpi_opcode_is_type6opcode(uint16_t opcode)
{
	if (acpi_opcode_is_type(opcode, AML_TYPE6_OPCODE) ||
	    acpi_opcode_is_userterm(opcode))
		return true;

	return false;
}

boolean acpi_opcode_is_simplename(uint16_t opcode)
{
	if (acpi_opcode_is_namestring(opcode) ||
	    acpi_opcode_is_argobj(opcode) ||
	    acpi_opcode_is_localobj(opcode))
		return true;

	return false;
}

boolean acpi_opcode_is_userterm(uint16_t opcode)
{
	return acpi_opcode_is_namestring(opcode);
}

boolean acpi_opcode_is_supername(uint16_t opcode)
{
	if (acpi_opcode_is_type6opcode(opcode) ||
	    acpi_opcode_is_simplename(opcode) ||
	    acpi_opcode_is_debugobj(opcode))
		return true;

	return false;
}

boolean acpi_opcode_is_termarg(uint16_t opcode)
{
	if (acpi_opcode_is_type2opcode(opcode) ||
	    acpi_opcode_is_dataobject(opcode) ||
	    acpi_opcode_is_argobj(opcode) ||
	    acpi_opcode_is_localobj(opcode))
		return true;

	return false;
}

boolean acpi_opcode_is_termobj(uint16_t opcode)
{
	if (acpi_opcode_is_namespacemodifierobj(opcode) ||
	    acpi_opcode_is_namedobj(opcode) ||
	    acpi_opcode_is_type1opcode(opcode) ||
	    acpi_opcode_is_type2opcode(opcode))
		return true;

	return false;
}

acpi_status_t acpi_opcode_get_args(uint16_t opcode, int16_t *nr_arguments,
				   int16_t *nr_targets, int16_t *nr_returns)
{
	const struct acpi_opcode_info *info;

	info = acpi_opcode_get_info(opcode);

	if (!info)
		return AE_AML_BAD_OPCODE;

	if (nr_arguments)
		*nr_arguments = (info->flags >> AML_ARGUMENT_OFFSET) &
				AML_ARGUMENT_MASK;
	if (nr_targets)
		*nr_targets = (info->flags >> AML_TARGET_OFFSET) &
			      AML_TARGET_MASK;
	if (nr_returns)
		*nr_returns = (info->flags >> AML_RETURN_OFFSET) &
			      AML_RETURN_MASK;

	return AE_OK;
}

#ifdef CONFIG_ACPI_TESTS
static const char *acpi_argument_type_name(uint8_t arg_type)
{
	switch (arg_type) {
	default:
	case AML_NONE:
		return "";
	case AML_BYTEDATA:
		return "ByteData";
	case AML_WORDDATA:
		return "WordData";
	case AML_DWORDDATA:
		return "DWordData";
	case AML_QWORDDATA:
		return "QWordData";
	case AML_ASCIICHARLIST:
		return "AsciiCharList";
	case AML_NULLCHAR:
		return "NullChar";
	case AML_BYTELIST:
		return "ByteList";
	case AML_NAMESTRING:
		return "NameString";
	case AML_PKGLENGTH:
		return "PkgLength";
	case AML_PACKAGELEMENTLIST:
		return "PackageElementList";
	case AML_SIMPLENAME:
		return "SimpleName";
	case AML_TERMLIST:
		return "TermList";
	case AML_DATAREFOBJECT:
		return "DataRefObject";
	case AML_OBJECTREFERENCE:
		return "ObjectReference";
	case AML_FIELDLIST:
		return "FieldList";
	case AML_OBJLIST:
		return "ObjList";
	case AML_SUPERNAME(ANY):
		return "SuperName";
	case AML_TARGET:
		return "SuperName => Target";
	case AML_DEVICE:
		return "SuperName => Device,Processor,ThermalZone";
	case AML_TERMARG(ANY):
		return "TermArg";
	case AML_INTEGERARG:
		return "TermArg => Integer";
	case AML_BUFFERARG:
		return "TermArg => Buffer";
	case AML_PACKAGEARG:
		return "TermArg => Package";
	case AML_OBJECTARG:
		return "TermArg => DataRefObject";
	case AML_DATA:
		return "TermArg => ComputationalData";
	case AML_BUFFSTR:
		return "TermArg => Buffer,String";
	case AML_BUFFPKGSTR:
		return "TermArg => Buffer,Package,String";
	case AML_BYTEARG:
		return "TermArg => ByteData";
	}
}

static const char *acpi_opcode_argument_name(uint32_t args, uint8_t index)
{
	return acpi_argument_type_name(AML_GET_ARG_TYPE(args, index));
}

void acpi_opcode_test(char *str, uint16_t opcode)
{
	const struct acpi_opcode_info *info;
	int16_t nr_arguments = -1, nr_targets = -1, nr_returns = -1;

	info = acpi_opcode_get_info(opcode);
	(void)acpi_opcode_get_args(opcode,
				   &nr_arguments,
				   &nr_targets,
				   &nr_returns);

	acpi_dbg("%s: %s(%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s) %d %d %d", str,
		 info->name,
		 (nr_arguments > 0) ? acpi_opcode_argument_name(info->args, 0) : "",
		 (nr_arguments > 1) ? ", " : "",
		 (nr_arguments > 1) ? acpi_opcode_argument_name(info->args, 1) : "",
		 (nr_arguments > 2) ? ", " : "",
		 (nr_arguments > 2) ? acpi_opcode_argument_name(info->args, 2) : "",
		 (nr_arguments > 3) ? ", " : "",
		 (nr_arguments > 3) ? acpi_opcode_argument_name(info->args, 3) : "",
		 (nr_arguments > 4) ? ", " : "",
		 (nr_arguments > 4) ? acpi_opcode_argument_name(info->args, 4) : "",
		 (nr_arguments > 5) ? ", " : "",
		 (nr_arguments > 5) ? acpi_opcode_argument_name(info->args, 5) : "",
		 (nr_targets > 0) ? ", " : "",
		 (nr_targets > 0) ? acpi_opcode_argument_name(info->args, (uint8_t)(nr_arguments+0)) : "",
		 (nr_targets > 1) ? ", " : "",
		 (nr_targets > 1) ? acpi_opcode_argument_name(info->args, (uint8_t)(nr_arguments+1)) : "",
		 nr_arguments, nr_targets, nr_returns);
}

#define ACPI_OPCODE_TEST(op)	acpi_opcode_test(#op, op)

void acpi_opcode_tests(void)
{
	ACPI_OPCODE_TEST(AML_ZERO_OP);
	ACPI_OPCODE_TEST(AML_ONE_OP);
	ACPI_OPCODE_TEST(AML_ALIAS_OP);
	ACPI_OPCODE_TEST(AML_NAME_OP);
	ACPI_OPCODE_TEST(AML_BYTE_PFX);
	ACPI_OPCODE_TEST(AML_WORD_PFX);
	ACPI_OPCODE_TEST(AML_DWORD_PFX);
	ACPI_OPCODE_TEST(AML_STRING_PFX);
	ACPI_OPCODE_TEST(AML_SCOPE_OP);
	ACPI_OPCODE_TEST(AML_BUFFER_OP);
	ACPI_OPCODE_TEST(AML_PACKAGE_OP);
	ACPI_OPCODE_TEST(AML_METHOD_OP);
	ACPI_OPCODE_TEST(AML_LOCAL0);
	ACPI_OPCODE_TEST(AML_LOCAL1);
	ACPI_OPCODE_TEST(AML_LOCAL2);
	ACPI_OPCODE_TEST(AML_LOCAL3);
	ACPI_OPCODE_TEST(AML_LOCAL4);
	ACPI_OPCODE_TEST(AML_LOCAL5);
	ACPI_OPCODE_TEST(AML_LOCAL6);
	ACPI_OPCODE_TEST(AML_LOCAL7);
	ACPI_OPCODE_TEST(AML_ARG0);
	ACPI_OPCODE_TEST(AML_ARG1);
	ACPI_OPCODE_TEST(AML_ARG2);
	ACPI_OPCODE_TEST(AML_ARG3);
	ACPI_OPCODE_TEST(AML_ARG4);
	ACPI_OPCODE_TEST(AML_ARG5);
	ACPI_OPCODE_TEST(AML_ARG6);
	ACPI_OPCODE_TEST(AML_STORE_OP);
	ACPI_OPCODE_TEST(AML_REF_OF_OP);
	ACPI_OPCODE_TEST(AML_ADD_OP);
	ACPI_OPCODE_TEST(AML_CONCAT_OP);
	ACPI_OPCODE_TEST(AML_SUBTRACT_OP);
	ACPI_OPCODE_TEST(AML_INCREMENT_OP);
	ACPI_OPCODE_TEST(AML_DECREMENT_OP);
	ACPI_OPCODE_TEST(AML_MULTIPLY_OP);
	ACPI_OPCODE_TEST(AML_DIVIDE_OP);
	ACPI_OPCODE_TEST(AML_SHIFT_LEFT_OP);
	ACPI_OPCODE_TEST(AML_SHIFT_RIGHT_OP);
	ACPI_OPCODE_TEST(AML_BIT_AND_OP);
	ACPI_OPCODE_TEST(AML_BIT_NAND_OP);
	ACPI_OPCODE_TEST(AML_BIT_OR_OP);
	ACPI_OPCODE_TEST(AML_BIT_NOR_OP);
	ACPI_OPCODE_TEST(AML_BIT_XOR_OP);
	ACPI_OPCODE_TEST(AML_BIT_NOT_OP);
	ACPI_OPCODE_TEST(AML_FIND_SET_LEFT_BIT_OP);
	ACPI_OPCODE_TEST(AML_FIND_SET_RIGHT_BIT_OP);
	ACPI_OPCODE_TEST(AML_DEREF_OF_OP);
	ACPI_OPCODE_TEST(AML_NOTIFY_OP);
	ACPI_OPCODE_TEST(AML_SIZE_OF_OP);
	ACPI_OPCODE_TEST(AML_INDEX_OP);
	ACPI_OPCODE_TEST(AML_MATCH_OP);
	ACPI_OPCODE_TEST(AML_CREATE_DWORD_FIELD_OP);
	ACPI_OPCODE_TEST(AML_CREATE_WORD_FIELD_OP);
	ACPI_OPCODE_TEST(AML_CREATE_BYTE_FIELD_OP);
	ACPI_OPCODE_TEST(AML_CREATE_BIT_FIELD_OP);
	ACPI_OPCODE_TEST(AML_TYPE_OP);
	ACPI_OPCODE_TEST(AML_LAND_OP);
	ACPI_OPCODE_TEST(AML_LOR_OP);
	ACPI_OPCODE_TEST(AML_LNOT_OP);
	ACPI_OPCODE_TEST(AML_LEQUAL_OP);
	ACPI_OPCODE_TEST(AML_LGREATER_OP);
	ACPI_OPCODE_TEST(AML_LLESS_OP);
	ACPI_OPCODE_TEST(AML_IF_OP);
	ACPI_OPCODE_TEST(AML_ELSE_OP);
	ACPI_OPCODE_TEST(AML_WHILE_OP);
	ACPI_OPCODE_TEST(AML_NOOP_OP);
	ACPI_OPCODE_TEST(AML_RETURN_OP);
	ACPI_OPCODE_TEST(AML_BREAK_OP);
	ACPI_OPCODE_TEST(AML_BREAK_POINT_OP);
	ACPI_OPCODE_TEST(AML_ONES_OP);
	ACPI_OPCODE_TEST(AML_QWORD_PFX);
	ACPI_OPCODE_TEST(AML_VAR_PACKAGE_OP);
	ACPI_OPCODE_TEST(AML_CONCAT_RES_OP);
	ACPI_OPCODE_TEST(AML_MOD_OP);
	ACPI_OPCODE_TEST(AML_CREATE_QWORD_FIELD_OP);
	ACPI_OPCODE_TEST(AML_TO_BUFFER_OP);
	ACPI_OPCODE_TEST(AML_TO_DECSTRING_OP);
	ACPI_OPCODE_TEST(AML_TO_HEXSTRING_OP);
	ACPI_OPCODE_TEST(AML_TO_INTEGER_OP);
	ACPI_OPCODE_TEST(AML_TO_STRING_OP);
	ACPI_OPCODE_TEST(AML_COPY_OP);
	ACPI_OPCODE_TEST(AML_MID_OP);
	ACPI_OPCODE_TEST(AML_CONTINUE_OP);

	ACPI_OPCODE_TEST(AML_MUTEX_OP);
	ACPI_OPCODE_TEST(AML_EVENT_OP);
	ACPI_OPCODE_TEST(AML_COND_REF_OF_OP);
	ACPI_OPCODE_TEST(AML_CREATE_FIELD_OP);
	ACPI_OPCODE_TEST(AML_LOAD_OP);
	ACPI_OPCODE_TEST(AML_STALL_OP);
	ACPI_OPCODE_TEST(AML_SLEEP_OP);
	ACPI_OPCODE_TEST(AML_ACQUIRE_OP);
	ACPI_OPCODE_TEST(AML_SIGNAL_OP);
	ACPI_OPCODE_TEST(AML_WAIT_OP);
	ACPI_OPCODE_TEST(AML_RESET_OP);
	ACPI_OPCODE_TEST(AML_RELEASE_OP);
	ACPI_OPCODE_TEST(AML_FROM_BCD_OP);
	ACPI_OPCODE_TEST(AML_TO_BCD_OP);
	ACPI_OPCODE_TEST(AML_UNLOAD_OP);
	ACPI_OPCODE_TEST(AML_REVISION_OP);
	ACPI_OPCODE_TEST(AML_DEBUG_OP);
	ACPI_OPCODE_TEST(AML_FATAL_OP);
	ACPI_OPCODE_TEST(AML_REGION_OP);
	ACPI_OPCODE_TEST(AML_FIELD_OP);
	ACPI_OPCODE_TEST(AML_DEVICE_OP);
	ACPI_OPCODE_TEST(AML_PROCESSOR_OP);
	ACPI_OPCODE_TEST(AML_POWER_RES_OP);
	ACPI_OPCODE_TEST(AML_THERMAL_ZONE_OP);
	ACPI_OPCODE_TEST(AML_INDEX_FIELD_OP);
	ACPI_OPCODE_TEST(AML_BANK_FIELD_OP);
	ACPI_OPCODE_TEST(AML_LOAD_TABLE_OP);
	ACPI_OPCODE_TEST(AML_DATA_REGION_OP);
	ACPI_OPCODE_TEST(AML_TIMER_OP);
}
#endif