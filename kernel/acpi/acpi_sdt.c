#include <target/acpi.h>
#include <target/panic.h>
#include <target/acpi_sdt.h>
#include <target/list.h>
#include "acpi_aml.h"

struct aml_op_info aml_opcode_info[] = {
	//                                OpCode                     SubOpCode                Num  1           2           3           4           5           6             Attribute
	/* ZeroOp - 0x00 */             { AML_ZERO_OP,               0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* OneOp  - 0x01 */             { AML_ONE_OP,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* AliasOp - 0x06 */            { AML_ALIAS_OP,              0,                       2, { AML_NAME,   AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* NameOp - 0x08 */             { AML_NAME_OP,               0,                       2, { AML_NAME,   AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* BytePrefix - 0x0A */         { AML_BYTE_PFX,              0,                       1, { AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* WordPrefix - 0x0B */         { AML_WORD_PFX,              0,                       1, { AML_UINT16, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* DWordPrefix - 0x0C */        { AML_DWORD_PFX,             0,                       1, { AML_UINT32, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* StringPrefix - 0x0D */       { AML_STRING_PFX,            0,                       1, { AML_STRING, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* QWordPrefix - 0x0E */        { AML_QWORD_PFX,             0,                       1, { AML_UINT64, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ScopeOp - 0x10 */            { AML_SCOPE_OP,              0,                       1, { AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* BufferOp - 0x11 */           { AML_BUFFER_OP,             0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH                                        },
	/* PackageOp - 0x12 */          { AML_PACKAGE_OP,            0,                       1, { AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ                    },
	/* VarPackageOp - 0x13 */       { AML_VAR_PACKAGE_OP,        0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ                    },
	/* MethodOp - 0x14 */           { AML_METHOD_OP,             0,                       2, { AML_NAME,   AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* DualNamePrefix - 0x2F */     { AML_DUAL_NAME_PFX,         0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* MultiNamePrefix - 0x2F */    { AML_MULTI_NAME_PFX,        0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x41 */           { 'A',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x42 */           { 'B',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x43 */           { 'C',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x44 */           { 'D',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x45 */           { 'E',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x46 */           { 'F',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x47 */           { 'G',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x48 */           { 'H',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x49 */           { 'I',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4A */           { 'J',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4B */           { 'K',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4C */           { 'L',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4D */           { 'M',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4E */           { 'N',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x4F */           { 'O',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x50 */           { 'P',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x51 */           { 'Q',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x52 */           { 'R',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x53 */           { 'S',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x54 */           { 'T',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x55 */           { 'U',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x56 */           { 'V',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x57 */           { 'W',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x58 */           { 'X',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x59 */           { 'Y',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x5A */           { 'Z',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* MutexOp - 0x5B 0x01 */       { AML_EXT_OP,                AML_EXT_MUTEX_OP,        2, { AML_NAME,   AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* EventOp - 0x5B 0x02 */       { AML_EXT_OP,                AML_EXT_EVENT_OP,        1, { AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* CondRefOfOp - 0x5B 0x12 */   { AML_EXT_OP,                AML_EXT_COND_REF_OF_OP,  2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CreateFieldOp - 0x5B 0x13 */ { AML_EXT_OP,                AML_EXT_CREATE_FIELD_OP, 4, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LoadTableOp - 0x5B 0x1F */   { AML_EXT_OP,                AML_EXT_LOAD_TABLE_OP,   6, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_OBJECT }, 0                                                         },
	/* LoadOp - 0x5B 0x20 */        { AML_EXT_OP,                AML_EXT_LOAD_OP,         2, { AML_NAME,   AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* StallOp - 0x5B 0x21 */       { AML_EXT_OP,                AML_EXT_STALL_OP,        1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* SleepOp - 0x5B 0x22 */       { AML_EXT_OP,                AML_EXT_SLEEP_OP,        1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* AcquireOp - 0x5B 0x23 */     { AML_EXT_OP,                AML_EXT_ACQUIRE_OP,      2, { AML_OBJECT, AML_UINT16, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* SignalOp - 0x5B 0x24 */      { AML_EXT_OP,                AML_EXT_SIGNAL_OP,       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* WaitOp - 0x5B 0x25 */        { AML_EXT_OP,                AML_EXT_WAIT_OP,         2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ResetOp - 0x5B 0x26 */       { AML_EXT_OP,                AML_EXT_RESET_OP,        1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ReleaseOp - 0x5B 0x27 */     { AML_EXT_OP,                AML_EXT_RELEASE_OP,      1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* FromBCDOp - 0x5B 0x28 */     { AML_EXT_OP,                AML_EXT_FROM_BCD_OP,     2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToBCDOp - 0x5B 0x29 */       { AML_EXT_OP,                AML_EXT_TO_BCD_OP,       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* UnloadOp - 0x5B 0x2A */      { AML_EXT_OP,                AML_EXT_UNLOAD_OP,       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* RevisionOp - 0x5B 0x30 */    { AML_EXT_OP,                AML_EXT_REVISION_OP,     0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* DebugOp - 0x5B 0x31 */       { AML_EXT_OP,                AML_EXT_DEBUG_OP,        0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* FatalOp - 0x5B 0x32 */       { AML_EXT_OP,                AML_EXT_FATAL_OP,        3, { AML_UINT8,  AML_UINT32, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* TimerOp - 0x5B 0x33 */       { AML_EXT_OP,                AML_EXT_TIMER_OP,        0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* OpRegionOp - 0x5B 0x80 */    { AML_EXT_OP,                AML_EXT_REGION_OP,       4, { AML_NAME,   AML_UINT8,  AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* FieldOp - 0x5B 0x81 */       { AML_EXT_OP,                AML_EXT_FIELD_OP,        2, { AML_NAME,   AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH                                        },
	/* DeviceOp - 0x5B 0x82 */      { AML_EXT_OP,                AML_EXT_DEVICE_OP,       1, { AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* ProcessorOp - 0x5B 0x83 */   { AML_EXT_OP,                AML_EXT_PROCESSOR_OP,    4, { AML_NAME,   AML_UINT8,  AML_UINT32, AML_UINT8,  AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* PowerResOp - 0x5B 0x84 */    { AML_EXT_OP,                AML_EXT_POWER_RES_OP,    3, { AML_NAME,   AML_UINT8,  AML_UINT16, AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* ThermalZoneOp - 0x5B 0x85 */ { AML_EXT_OP,                AML_EXT_THERMAL_ZONE_OP, 1, { AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ | AML_IN_NAMESPACE },
	/* IndexFieldOp - 0x5B 0x86 */  { AML_EXT_OP,                AML_EXT_INDEX_FIELD_OP,  3, { AML_NAME,   AML_NAME,   AML_UINT8,  AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH                                        },
	/* BankFieldOp - 0x5B 0x87 */   { AML_EXT_OP,                AML_EXT_BANK_FIELD_OP,   4, { AML_NAME,   AML_NAME,   AML_OBJECT, AML_UINT8,  AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH                                        },
	/* DataRegionOp - 0x5B 0x88 */  { AML_EXT_OP,                AML_EXT_DATA_REGION_OP,  4, { AML_NAME,   AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE   }, AML_IN_NAMESPACE                                          },
	/* RootChar - 0x5C */           { AML_ROOT_PFX,              0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* ParentPrefixChar - 0x5E */   { AML_PARENT_PFX,            0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* NameChar - 0x5F */           { '_',                       0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_IS_NAME_CHAR                                          },
	/* Local0Op - 0x60 */           { AML_LOCAL0,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local1Op - 0x61 */           { AML_LOCAL1,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local2Op - 0x62 */           { AML_LOCAL2,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local3Op - 0x63 */           { AML_LOCAL3,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local4Op - 0x64 */           { AML_LOCAL4,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local5Op - 0x65 */           { AML_LOCAL5,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local6Op - 0x66 */           { AML_LOCAL6,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Local7Op - 0x67 */           { AML_LOCAL7,                0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg0Op - 0x68 */             { AML_ARG0,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg1Op - 0x69 */             { AML_ARG1,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg2Op - 0x6A */             { AML_ARG2,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg3Op - 0x6B */             { AML_ARG3,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg4Op - 0x6C */             { AML_ARG4,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg5Op - 0x6D */             { AML_ARG5,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* Arg6Op - 0x6E */             { AML_ARG6,                  0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* StoreOp - 0x70 */            { AML_STORE_OP,              0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* RefOfOp - 0x71 */            { AML_REF_OF_OP,             0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* AddOp - 0x72 */              { AML_ADD_OP,                0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ConcatOp - 0x73 */           { AML_CONCAT_OP,             0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* SubtractOp - 0x74 */         { AML_SUBTRACT_OP,           0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* IncrementOp - 0x75 */        { AML_INCREMENT_OP,          0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* DecrementOp - 0x76 */        { AML_DECREMENT_OP,          0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* MultiplyOp - 0x77 */         { AML_MULTIPLY_OP,           0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* DivideOp - 0x78 */           { AML_DIVIDE_OP,             0,                       4, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE   }, 0                                                         },
	/* ShiftLeftOp - 0x79 */        { AML_SHIFT_LEFT_OP,         0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ShiftRightOp - 0x7A */       { AML_SHIFT_RIGHT_OP,        0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* AndOp - 0x7B */              { AML_BIT_AND_OP,            0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* NAndOp - 0x7C */             { AML_BIT_NAND_OP,           0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* OrOp - 0x7D */               { AML_BIT_OR_OP,             0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* NorOp - 0x7E */              { AML_BIT_NOR_OP,            0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* XOrOp - 0x7F */              { AML_BIT_XOR_OP,            0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* NotOp - 0x80 */              { AML_BIT_NOT_OP,            0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* FindSetLeftBitOp - 0x81 */   { AML_FIND_SET_LEFT_BIT_OP,  0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* FindSetRightBitOp - 0x82 */  { AML_FIND_SET_RIGHT_BIT_OP, 0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* DerefOfOp - 0x83 */          { AML_DEREF_OF_OP,           0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ConcatResOp - 0x84 */        { AML_CONCAT_RES_OP,         0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ModOp - 0x85 */              { AML_MOD_OP,                0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* NotifyOp - 0x86 */           { AML_NOTIFY_OP,             0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* SizeOfOp - 0x87 */           { AML_SIZE_OF_OP,            0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* IndexOp - 0x88 */            { AML_INDEX_OP,              0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* MatchOp - 0x89 */            { AML_MATCH_OP,              0,                       6, { AML_OBJECT, AML_UINT8,  AML_OBJECT, AML_UINT8,  AML_OBJECT, AML_OBJECT }, 0                                                         },
	/* CreateDWordFieldOp - 0x8A */ { AML_CREATE_DWORD_FIELD_OP, 0,                       3, { AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CreateWordFieldOp - 0x8B */  { AML_CREATE_WORD_FIELD_OP,  0,                       3, { AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CreateByteFieldOp - 0x8C */  { AML_CREATE_BYTE_FIELD_OP,  0,                       3, { AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CreateBitFieldOp - 0x8D */   { AML_CREATE_BIT_FIELD_OP,   0,                       3, { AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ObjectTypeOp - 0x8E */       { AML_TYPE_OP,               0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CreateQWordFieldOp - 0x8F */ { AML_CREATE_QWORD_FIELD_OP, 0,                       3, { AML_OBJECT, AML_OBJECT, AML_NAME,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LAndOp - 0x90 */             { AML_LAND_OP,               0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LOrOp - 0x91 */              { AML_LOR_OP,                0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LNotOp - 0x92 */             { AML_LNOT_OP,               0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LEqualOp - 0x93 */           { AML_LEQUAL_OP,             0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LGreaterOp - 0x94 */         { AML_LGREATER_OP,           0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* LLessOp - 0x95 */            { AML_LLESS_OP,              0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToBufferOp - 0x96 */         { AML_TO_BUFFER_OP,          0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToDecimalStringOp - 0x97 */  { AML_TO_DECSTRING_OP,       0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToHexStringOp - 0x98 */      { AML_TO_HEXSTRING_OP,       0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToIntegerOp - 0x99 */        { AML_TO_INTEGER_OP,         0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ToStringOp - 0x9C */         { AML_TO_STRING_OP,          0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* CopyObjectOp - 0x9D */       { AML_COPY_OP,               0,                       2, { AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* MidOp - 0x9E */              { AML_MID_OP,                0,                       3, { AML_OBJECT, AML_OBJECT, AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ContinueOp - 0x9F */         { AML_CONTINUE_OP,           0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* IfOp - 0xA0 */               { AML_IF_OP,                 0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ                    },
	/* ElseOp - 0xA1 */             { AML_ELSE_OP,               0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ                    },
	/* WhileOp - 0xA2 */            { AML_WHILE_OP,              0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, AML_HAS_PKG_LENGTH | AML_HAS_CHILD_OBJ                    },
	/* NoopOp - 0xA3 */             { AML_NOOP_OP,               0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* ReturnOp - 0xA4 */           { AML_RETURN_OP,             0,                       1, { AML_OBJECT, AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* BreakOp - 0xA5 */            { AML_BREAK_OP,              0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* BreakPointOp - 0xCC */       { AML_BREAK_POINT_OP,        0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
	/* OnesOp - 0xFF */             { AML_ONES_OP,               0,                       0, { AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE,   AML_NONE   }, 0                                                         },
};

acpi_status_t aml_construct_node_list(struct aml_handle *handle,
				      struct aml_node_list *root,
				      struct aml_node_list *parent);
size_t aml_get_object_size(struct aml_op_info *info,
			   uint8_t *buffer,
			   size_t max_buffer_size);
acpi_status_t aml_parse_option_handle_common(struct aml_handle *handle,
					     AML_OP_PARSE_INDEX index,
					     AML_OP_PARSE_FORMAT *data_type,
					     uint8_t **data, size_t *data_size);

struct aml_op_info *aml_search_by_opcode(uint8_t *buffer)
{
	uint8_t opcode;
	uint8_t ext_opcode;
	int i;

	opcode = buffer[0];
	if (opcode == AML_EXT_OP)
		ext_opcode = buffer[1];
	else
		ext_opcode = 0;

	for (i = 0; i < ARRAY_SIZE(aml_opcode_info); i++) {
		if (aml_opcode_info[i].opcode == opcode &&
		    aml_opcode_info[i].ext_opcode == ext_opcode)
			return &aml_opcode_info[i];
	}

	return NULL;
}

int aml_get_pkg_length(uint8_t *buffer, size_t *pkg_length)
{
	uint8_t lead_byte;
	int count;
	int real_len;
	int offset;

	/* <bit 7-6: ByteData count that follows (0-3)>
	 * <bit 5-4: Only used if PkgLength < 63>
	 * <bit 3-0: Least significant package length nybble>
	 * Note: The high 2 bits of the first byte reveal how many follow
	 * bytes are in the package. If the PkgLength has only one byte,
	 * bit 0 through 5 are used to encode the package length (in other
	 * words, values 0-63). If the package length value is more than
	 * 63, more than one byte must be used for the encoding in which
	 * case bit 4 and 5 of the PkgLeadByte are reserved and must be
	 * zero. If the multiple bytes encoding is used, bits 0-3 of the
	 * PkgLeadByte become the least significant 4 bits of the resulting
	 * package length value. The next ByteData will become the next
	 * least significant 8 bits of the resulting value and so on, up to
	 * 3 ByteData bytes. Thus, the maximum package length is 2**28.
	 */

	lead_byte = *buffer;
	count = (uint8_t)((lead_byte >> 6) & 0x03);
	offset = count + 1;
	real_len = 0;

	switch (count) {
	case 0:
		real_len = (uint32_t)lead_byte;
		break;
	case 1:
		real_len = *(buffer + 1);
		real_len = (real_len << 4) | (lead_byte & 0xF);
		break;
	case 2:
		real_len  = *(buffer + 1);
		real_len |= (int)((*(buffer + 2)) << 8);
		real_len  = (real_len << 4) | (lead_byte & 0xF);
		break;
	case 3:
		real_len  = *(buffer + 1);
		real_len |= (int)((*(buffer + 2)) << 8);
		real_len |= (int)((*(buffer + 3)) << 16);
		real_len  = (real_len << 4) | (lead_byte & 0xF);
		break;
	default:
		BUG();
		break;
	}

	*pkg_length = real_len;
	return offset;
}

bool aml_is_root_path(uint8_t *buffer)
{
	if ((buffer[0] == AML_ROOT_PFX) && (buffer[1] == 0))
		return true;
	else
		return false;
}

bool aml_is_lead_name(uint8_t ch)
{
	if ((ch == '_') || ((ch >= 'A') && (ch <= 'Z')))
		return true;
	else
		return false;
}

bool aml_is_name(uint8_t ch)
{
	if (aml_is_lead_name(ch) || ((ch >= '0') && (ch <= '9')))
		return true;
	else
		return false;
}

bool aml_is_name_seg(uint8_t *buffer)
{
	int i;

	if (!aml_is_lead_name(buffer[0]))
		return false;

	for (i = 1; i < ACPI_NAMESEG_SIZE; i++) {
		if (!aml_is_name(buffer[i]))
			return false;
	}
	return true;
}

acpi_status_t aml_get_name_string_size(uint8_t *buffer,
				       size_t *buffer_size)
{
	int seg_count;
	size_t length;
	int i;

	length = 0;

	/* Parse root or parent prefix */
	if (*buffer == AML_ROOT_PFX) {
		buffer++;
		length++;
	} else if (*buffer == AML_PARENT_PFX) {
		do {
			buffer++;
			length++;
		} while (*buffer == AML_PARENT_PFX);
	}

	/* Parse name segment */
	if (*buffer == AML_DUAL_NAME_PFX) {
		buffer++;
		length++;
		seg_count = 2;
	} else if (*buffer == AML_MULTI_NAME_PFX) {
		buffer++;
		length++;
		seg_count = *buffer;
		buffer++;
		length++;
	} else if (*buffer == 0) {
		/* NULL Name, only for Root */
		seg_count = 0;
		buffer--;
		if ((length == 1) && (*buffer == AML_ROOT_PFX)) {
			*buffer_size = 2;
			return AE_OK;
		} else
			return AE_BAD_PARAMETER;
	} else {
		/* NameSeg */
		seg_count = 1;
	}

	i = 0;
	do {
		if (!aml_is_name_seg(buffer))
			return AE_BAD_PARAMETER;

		buffer += ACPI_NAMESEG_SIZE;
		length += ACPI_NAMESEG_SIZE;
		i++;
	} while (i < seg_count);
	*buffer_size = length;
	return AE_OK;
}

bool aml_is_asl_lead_name(uint8_t ch)
{
	if (aml_is_lead_name(ch) || ((ch >= 'a') && (ch <= 'z')))
		return true;
	else
		return false;
}

bool aml_is_asl_name(uint8_t ch)
{
	if (aml_is_asl_lead_name(ch) || ((ch >= '0') && (ch <= '9')))
		return true;
	else
		return false;
}

size_t aml_get_asl_name_seg_length(char *buffer)
{
	size_t length;
	int i;

	if (*buffer == 0)
		return 0;

	length = 0;
	/* 1st */
	if (aml_is_asl_lead_name(*buffer)) {
		length++;
		buffer++;
	}

	if ((*buffer == 0) || (*buffer == '.'))
		return length;

	/* 2, 3, 4 name char */
	for (i = 0; i < 3; i++) {
		if (aml_is_asl_name(*buffer)) {
			length++;
			buffer++;
		}
		if ((*buffer == 0) || (*buffer == '.'))
			return length;
	}
	return 0;
}

size_t aml_get_asl_name_string_size(char *buffer,
				    size_t *root,
				    size_t *parent,
				    int *seg_count)
{
	size_t name_length;
	size_t total_length;

	*root = 0;
	*parent = 0;
	*seg_count = 0;
	total_length = 0;
	name_length = 0;
	if (*buffer == AML_ROOT_PFX) {
		*root = 1;
		buffer++;
	} else if (*buffer == AML_PARENT_PFX) {
		do {
			buffer++;
			(*parent)++;
		} while (*buffer == AML_PARENT_PFX);
	}

	/* Now parse name */
	while (*buffer != 0) {
		name_length = aml_get_asl_name_seg_length(buffer);
		if ((name_length == 0) || (name_length > ACPI_NAMESEG_SIZE))
			return 0;

		(*seg_count)++;
		buffer += name_length;
		if (*buffer == 0)
			break;
		buffer++;
	}

	/* Check SegCoount */
	if (*seg_count > 0xFF)
		return 0;

	/* Calculate total length */
	total_length = *root + *parent + (*seg_count) * ACPI_NAMESEG_SIZE;
	if (*seg_count > 2)
		total_length += 2;
	else if (*seg_count == 2)
		total_length += 1;

	/* Add NULL char */
	total_length++;
	return total_length;
}

void aml_upper_case_copy_mem(char *dst_buffer,
			     char *src_buffer,
			     size_t length)
{
	int i;

	for (i = 0; i < length; i++) {
		if ((src_buffer[i] >= 'a') && (src_buffer[i] <= 'z'))
			dst_buffer[i] = (uint8_t)(src_buffer[i] - 'a' + 'A');
		else
			dst_buffer[i] = src_buffer[i];
	}
}

uint8_t *aml_name_from_asl_name(char *asl_path)
{
	size_t root;
	size_t parent;
	int seg_count;
	size_t total_length;
	size_t name_length;
	char *buffer;
	uint8_t *aml_path;
	uint8_t *aml_buffer;

	total_length = aml_get_asl_name_string_size(asl_path,
						    &root,
						    &parent,
						    &seg_count);
	if (total_length == 0)
		return NULL;

	aml_path = acpi_os_allocate(total_length);
	BUG_ON(!aml_path);

	aml_buffer = aml_path;
	buffer = asl_path;

	/* Handle Root and Parent */
	if (root == 1) {
		*aml_buffer = AML_ROOT_PFX;
		aml_buffer++;
		buffer++;
	} else if (parent > 0) {
		memset(aml_buffer, AML_PARENT_PFX, parent);
		aml_buffer += parent;
		buffer += parent;
	}

	/* Handle SegCount */
	if (seg_count > 2) {
		*aml_buffer = AML_MULTI_NAME_PFX;
		aml_buffer++;
		*aml_buffer = (uint8_t)seg_count;
		aml_buffer++;
	} else if (seg_count == 2) {
		*aml_buffer = AML_DUAL_NAME_PFX;
		aml_buffer++;
	}

	/* Now to name */
	while (*buffer != 0) {
		name_length = aml_get_asl_name_seg_length(buffer);
		BUG_ON((!name_length) || (name_length > ACPI_NAMESEG_SIZE));
		aml_upper_case_copy_mem((char *)aml_buffer, buffer, name_length);
		memset(aml_buffer + name_length, AML_NAMESTRING_PAD,
		       ACPI_NAMESEG_SIZE - name_length);
		buffer += name_length;
		aml_buffer += ACPI_NAMESEG_SIZE;
		if (*buffer == 0)
			break;
		buffer++;
	}

	/* Add NULL */
	aml_path[total_length - 1] = 0;
	return aml_path;
}

void aml_print_name_seg(uint8_t *buffer)
{
	acpi_os_debug_print("%c", buffer[0]);
	if ((buffer[1] == '_') && (buffer[2] == '_') && (buffer[3] == '_'))
		return;
	acpi_os_debug_print("%c", buffer[1]);
	if ((buffer[2] == '_') && (buffer[3] == '_'))
		return;
	acpi_os_debug_print("%c", buffer[2]);
	if (buffer[3] == '_')
		return;
	acpi_os_debug_print("%c", buffer[3]);
}

void aml_print_name_string(uint8_t *buffer)
{
	int seg_count;
	int i;

	if (*buffer == AML_ROOT_PFX) {
		/* RootChar */
		buffer++;
		acpi_os_debug_print("\\");
	} else if (*buffer == AML_PARENT_PFX) {
		/* ParentPrefixChar */
		do {
			buffer++;
			acpi_os_debug_print("^");
		} while (*buffer == AML_PARENT_PFX);
	}

	if (*buffer == AML_DUAL_NAME_PFX) {
		/* DualName */
		buffer++;
		seg_count = 2;
	} else if (*buffer == AML_MULTI_NAME_PFX) {
		/* MultiName */
		buffer++;
		seg_count = *buffer;
		buffer++;
	} else if (*buffer == 0)
		return;
	else {
		/* NameSeg */
		seg_count = 1;
	}

	aml_print_name_seg(buffer);
	buffer += ACPI_NAMESEG_SIZE;
	for (i = 0; i < seg_count - 1; i++) {
		acpi_os_debug_print(".");
		aml_print_name_seg(buffer);
		buffer += ACPI_NAMESEG_SIZE;
	}
}

#if 0
void aml_print_name_seg(uint8_t *name_string)
{
	len = acpi_path_decode(&path, asl_path, ACPI_ASL_PATH_SIZE);
}
#endif

acpi_status_t aml_parse_option_term(struct aml_op_info *info,
				    uint8_t *buffer,
				    size_t max_buffer_size,
				    AML_OP_PARSE_INDEX term_index,
				    AML_OP_PARSE_FORMAT *data_type,
				    uint8_t **data,
				    size_t *data_size)
{
	struct aml_op_info *child_info;
	acpi_status_t status;

	if (!data_type)
		*data_type = info->format[term_index - 1];
	if (!data)
		*data = buffer;

	/* Parse term according to AML type */
	switch (info->format[term_index - 1]) {
	case AML_UINT8:
		*data_size = sizeof (uint8_t);
		break;
	case AML_UINT16:
		*data_size = sizeof (uint16_t);
		break;
	case AML_UINT32:
		*data_size = sizeof (uint32_t);
		break;
	case AML_UINT64:
		*data_size = sizeof (uint64_t);
		break;
	case AML_STRING:
		*data_size = strlen((char *)buffer);
		break;
	case AML_NAME:
		status = aml_get_name_string_size(buffer, data_size);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;
		break;
	case AML_OBJECT:
		child_info = aml_search_by_opcode(buffer);
		if (child_info == NULL)
			return AE_BAD_PARAMETER;
		/* NOTE: We need override DataType here, if there is a case
		 *       the AML_OBJECT is AML_NAME. We need convert type
		 *       from EFI_ACPI_DATA_TYPE_CHILD to
		 *       EFI_ACPI_DATA_TYPE_NAME_STRING. We should not
		 *       return CHILD because there is NO OpCode for
		 *       NameString.
		 */
		if ((child_info->attr & AML_IS_NAME_CHAR) != 0) {
			if (data_type != NULL)
				*data_type = AML_NAME;
			status = aml_get_name_string_size(buffer, data_size);
			if (ACPI_FAILURE(status))
				return AE_BAD_PARAMETER;
			break;
		}
		/* It is real AML_OBJECT */
		*data_size = aml_get_object_size(child_info,
						 buffer,
						 max_buffer_size);
		if (*data_size == 0)
			return AE_BAD_PARAMETER;
		break;
	case AML_NONE:
		/* No term */
	case AML_OPCODE:
	default:
		BUG();
		return AE_BAD_PARAMETER;
	}

	if (*data_size > max_buffer_size)
		return AE_BAD_PARAMETER;

	return AE_OK;
}

acpi_status_t aml_parse_option_common(struct aml_op_info *info,
				      uint8_t *buffer,
				      size_t max_buffer_size,
				      AML_OP_PARSE_INDEX index,
				      AML_OP_PARSE_FORMAT *data_type,
				      uint8_t **data, size_t *data_size)
{
	uint8_t *curr_buffer;
	size_t pkg_length;
	size_t op_length;
	int pkg_offset;
	AML_OP_PARSE_INDEX term_index;
	acpi_status_t status;

	BUG_ON((index > info->max_index) && (index != AML_OP_PARSE_INDEX_GET_SIZE));

	/* 0. Check if this is NAME string. */
	if ((info->attr & AML_IS_NAME_CHAR) != 0) {
		/* Only allow GET_SIZE */
		if (index != AML_OP_PARSE_INDEX_GET_SIZE)
			return AE_BAD_PARAMETER;
		/* return NameString size */
		status = aml_get_name_string_size(buffer, data_size);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;
		if (*data_size > max_buffer_size)
			return AE_BAD_PARAMETER;
		return AE_OK;
	}

	/* Not NAME string, start parsing */
	curr_buffer = buffer;

	/* 1. Get OpCode */
	if (index != AML_OP_PARSE_INDEX_GET_SIZE) {
		*data_type = AML_OPCODE;
		*data = curr_buffer;
	}

	if (*curr_buffer == AML_EXT_OP)
		op_length = 2;
	else
		op_length = 1;

	*data_size = op_length;
	if (index == AML_OP_PARSE_INDEX_GET_OPCODE)
		return AE_OK;

	if (op_length > max_buffer_size)
		return AE_BAD_PARAMETER;

	curr_buffer += op_length;

	/* 2. Skip PkgLength field, if have */
	if ((info->attr & AML_HAS_PKG_LENGTH) != 0) {
		pkg_offset = aml_get_pkg_length(curr_buffer,
						&pkg_length);
		/* Override MaxBufferSize if it is valid PkgLength */
		if (op_length + pkg_length > max_buffer_size)
			return AE_BAD_PARAMETER;
		else
			max_buffer_size = op_length + pkg_length;
	} else {
		pkg_offset = 0;
		pkg_length = 0;
	}

	curr_buffer += pkg_offset;

	/* 3. Get Term one by one. */
	term_index = AML_OP_PARSE_INDEX_GET_TERM1;
	while ((index >= term_index) &&
	       (term_index <= info->max_index) &&
	       (curr_buffer < (buffer + max_buffer_size))) {
		status = aml_parse_option_term(info,
					       curr_buffer,
					       buffer + max_buffer_size - curr_buffer,
					       term_index,
					       data_type,
					       data, data_size);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;
		if (index == term_index)
			return AE_OK;
		/* Parse next one */
		curr_buffer += *data_size;
		term_index++;
	}

	/* Finish all options, but no option found. */
	if (curr_buffer > (buffer + max_buffer_size))
		return AE_BAD_PARAMETER;

	if (curr_buffer == (buffer + max_buffer_size)) {
		if (index != AML_OP_PARSE_INDEX_GET_SIZE)
			return AE_BAD_PARAMETER;
	}

	/* 4. Finish parsing all node, return size */
	BUG_ON(index != AML_OP_PARSE_INDEX_GET_SIZE);
	if ((info->attr & AML_HAS_PKG_LENGTH) != 0)
		*data_size = op_length + pkg_length;
	else
		*data_size = curr_buffer - buffer;
	return AE_OK;
}

size_t aml_get_object_size(struct aml_op_info *info,
			   uint8_t *buffer,
			   size_t max_buffer_size)
{
	acpi_status_t status;
	size_t data_size;

	status = aml_parse_option_common(info, buffer,
					 max_buffer_size,
					 AML_OP_PARSE_INDEX_GET_SIZE,
					 NULL, NULL, &data_size);
	if (ACPI_FAILURE(status))
		return 0;
	else
		return data_size;
}

uint8_t *aml_get_object_name(struct aml_handle *handle)
{
	struct aml_op_info *info;
	uint8_t *name_string;
	size_t name_size;
	AML_OP_PARSE_INDEX term_index;
	AML_OP_PARSE_FORMAT data_type;
	acpi_status_t status;

	info = handle->info;
	BUG_ON((info->attr & AML_IN_NAMESPACE) == 0);

	/* Find out Last Name index, according to OpCode table. The last
	 * name will be the node name by design.
	 */
	term_index = info->max_index;
	for (term_index = info->max_index; term_index > 0; term_index--) {
		if (info->format[term_index - 1] == AML_NAME)
			break;
	}
	BUG_ON(term_index == 0);

	/* Get Name for this node. */
	status = aml_parse_option_handle_common(handle, term_index,
						&data_type, &name_string,
						&name_size);
	if (ACPI_FAILURE(status))
		return NULL;
	BUG_ON(data_type != AML_NAME);
	return name_string;
}

acpi_status_t aml_get_offset_after_last_option(struct aml_handle *handle,
					       uint8_t **buffer)
{
	AML_OP_PARSE_FORMAT data_type;
	uint8_t *data;
	size_t data_size;
	acpi_status_t status;

	status = aml_parse_option_handle_common(handle,
						handle->info->max_index,
						&data_type,
						&data, &data_size);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;

	/* We need to parse the rest buffer after last node. */
	*buffer = data + data_size;

	/* We need skip PkgLength if no Option */
	if (data_type == AML_OPCODE)
		*buffer += aml_get_pkg_length(*buffer, &data_size);
	return AE_OK;
}

acpi_status_t aml_parse_option_handle_common(struct aml_handle *handle,
					     AML_OP_PARSE_INDEX index,
					     AML_OP_PARSE_FORMAT *data_type,
					     uint8_t **data, size_t *data_size)
{
	return aml_parse_option_common(handle->info, handle->buffer,
				       handle->size, index,
				       data_type, data, data_size);
}

struct aml_node_list *aml_create_node(uint8_t *name_seg,
				      struct aml_node_list *parent,
				      struct aml_op_info *info)
{
	struct aml_node_list *aml_node;

	aml_node = acpi_os_allocate(sizeof (*aml_node));
	BUG_ON(!aml_node);

	aml_node->signature = EFI_AML_NODE_LIST_SIGNATURE;
	aml_node->name = acpi_os_allocate(ACPI_NAMESEG_SIZE);
	BUG_ON(!aml_node->name);
	memcpy(aml_node->name, name_seg, ACPI_NAMESEG_SIZE);
	aml_node->buffer = NULL;
	aml_node->size = 0;
	INIT_LIST_HEAD(&aml_node->link);
	INIT_LIST_HEAD(&aml_node->children);
	aml_node->parent = parent;
	aml_node->info = info;
	return aml_node;
}

struct aml_node_list *aml_find_node_in_this(uint8_t *name,
					    struct aml_node_list *parent,
					    bool create)
{
	struct aml_node_list *curr;
	struct aml_node_list *node;

	list_for_each_entry(struct aml_node_list, curr, &parent->children, link) {
		if (memcmp(curr->name, name, ACPI_NAMESEG_SIZE) == 0)
			return curr;
	}

	if (!create)
		return NULL;

	node = aml_create_node(name, parent, NULL);
	list_add_tail(&node->link, &parent->children);
	return node;
}

struct aml_node_list *aml_find_node_in_tree(uint8_t *name_string,
					    struct aml_node_list *root,
					    struct aml_node_list *parent,
					    bool create)
{
	uint8_t *buffer;
	struct aml_node_list *node;
	struct aml_node_list *curr_node;
	uint8_t i;
	uint8_t seg_count;

	buffer = name_string;

	if (*buffer == AML_ROOT_PFX) {
		curr_node = root;
		buffer += 1;
	} else if (*buffer == AML_PARENT_PFX) {
		curr_node = parent;
		do {
			if (curr_node->parent != NULL)
				curr_node = curr_node->parent;
			else {
				BUG_ON(curr_node != root);
			}
			buffer += 1;
		} while (*buffer == AML_PARENT_PFX);
	} else {
		curr_node = parent;
	}

	if (*buffer == AML_DUAL_NAME_PFX) {
		buffer += 1;
		seg_count = 2;
	} else if (*buffer == AML_MULTI_NAME_PFX) {
		buffer += 1;
		seg_count = *buffer;
		buffer += 1;
	} else if (*buffer == 0) {
		BUG_ON(curr_node != root);
		return curr_node;
	} else
		seg_count = 1;

	i = 0;
	do {
		node = aml_find_node_in_this(buffer, curr_node, create);
		if (!node)
			return NULL;

		curr_node = node;
		buffer += ACPI_NAMESEG_SIZE;
		i++;
	} while (i < seg_count);
	return node;
}

struct aml_node_list *aml_insert_node_to_tree(uint8_t *name_string,
					      uint8_t *buffer,
					      size_t size,
					      struct aml_node_list *root,
					      struct aml_node_list *parent)
{
	struct aml_node_list *node;

	node = aml_find_node_in_tree(name_string, root, parent, true);
	BUG_ON(!node);
	if (!node)
		return NULL;

	if (node->buffer == NULL) {
		if (*(uint8_t *)buffer != AML_SCOPE_OP) {
			node->buffer = buffer;
			node->size = size;
			node->info = aml_search_by_opcode(buffer);
		}
		return node;
	}

	if (*buffer == AML_SCOPE_OP)
		return node;

	acpi_err("acpi_sdt: Override Happen - %s!\n", name_string);
	acpi_err("acpi_sdt: Existing Node - %x\n", node->buffer);
	acpi_err("acpi_sdt: New Buffer - %x\n", buffer);
	return NULL;
}

acpi_status_t aml_construct_node_list_for_child(struct aml_handle *handle,
						struct aml_node_list *root,
						struct aml_node_list *parent)
{
	struct aml_op_info *info;
	uint8_t *buffer;
	size_t buffer_size;
	uint8_t *curr_buffer;
	struct aml_handle *child_handle;
	acpi_status_t status;

	curr_buffer = NULL;
	child_handle = NULL;
	info = handle->info;
	buffer = handle->buffer;
	buffer_size = handle->size;

	if ((info->attr & AML_HAS_CHILD_OBJ) == 0)
		return AE_OK;

	/* Do we need add node within METHOD? Yes, just add Object is OK.
	 * But we need filter NameString for METHOD invoke.
	 */

	/* Now, we get the last node. */
	status = aml_get_offset_after_last_option(handle, &curr_buffer);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;

	/* Go through all the reset buffer. */
	while ((size_t)curr_buffer < (size_t)buffer + buffer_size) {
		status = acpi_sdt_open_ex(curr_buffer,
			(size_t)buffer + buffer_size - (size_t)curr_buffer,
			(void **)&child_handle);
		if (ACPI_FAILURE(status))
			break;

		/* Good, find the child. Construct node recursively */
		status = aml_construct_node_list(handle, root, parent);
		if (ACPI_FAILURE(status))
			break;

		/* Parse next one */
		curr_buffer += child_handle->size;
		acpi_sdt_close(child_handle);
	}
	return AE_OK;
}

acpi_status_t aml_construct_node_list(struct aml_handle *handle,
				      struct aml_node_list *root,
				      struct aml_node_list *parent)
{
	uint8_t *name_string;
	struct aml_node_list *node;

	/* 1. Check if there is need to construct node for this OpCode. */
	if ((handle->info->attr & AML_IN_NAMESPACE) == 0)
		return AE_OK;

	/* 2. Now, we need construct node for this OpCode. */
	name_string = aml_get_object_name(handle);
	if (!name_string)
		return AE_BAD_PARAMETER;

	/* Now, we need to insert node to the node list.
	 * NOTE: The name here could be AML NameString. So the callee need parse
	 * it.
	 */
	node = aml_insert_node_to_tree(name_string, handle->buffer,
				       handle->size, root, parent);
	BUG_ON(!node);

	/* 3. Ok, we need to parse the object list to see if there are more node
	 *    to be added.
	 */
	return aml_construct_node_list_for_child(handle, root, node);
}

void aml_destruct_node_list(struct aml_node_list *parent)
{
	struct aml_node_list *curr, *next;

	list_for_each_entry_safe(struct aml_node_list, curr, next,
				 &parent->children, link) {
		list_del_init(&curr->link);
		aml_destruct_node_list(curr);
	}

	if (parent->name)
		acpi_os_free(parent->name);

	acpi_os_free(parent);
}

void aml_dump_node_info(struct aml_node_list *parent,
			int level)
{
	struct aml_node_list *curr;
	int i;

	if (level == 0)
		acpi_os_debug_print("\\");
	else {
		for (i = 0; i < level; i++)
			acpi_os_debug_print("    ");
		aml_print_name_seg(parent->name);
	}
	acpi_os_debug_print("\n");

	list_for_each_entry(struct aml_node_list, curr,
			    &parent->children, link) {
		aml_dump_node_info(curr, level + 1);
	}
}

acpi_status_t aml_find_path(struct aml_handle *handle,
			    uint8_t *path, uint8_t **buffer,
			    bool from_root)
{
	struct aml_node_list *root;
	acpi_status_t status;
	struct aml_node_list *node;
	uint8_t root_name[ACPI_NAMESEG_SIZE];
	struct aml_node_list *curr_node;

	/* 1. create tree */

	/* Create root handle */
	root_name[0] = AML_ROOT_PFX;
	root_name[1] = 0;
	root_name[2] = 0;
	root_name[3] = 0;
	root = aml_create_node(root_name, NULL, handle->info);

	status = aml_construct_node_list(handle, root, root);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;

	acpi_dbg("acpi_sdt: NameSpace:\n");
	aml_dump_node_info(root, 0);

	/* 2. Search the node in the tree */
	if (from_root)
		curr_node = root;
	else {
		/* Search from this node, NOT ROOT. Since we insert node
		 * to ROOT one by one, we just get the first node and
		 * search from it.
		 */
		curr_node = list_empty(&root->children) ? NULL :
			list_entry(&root->children.next, struct aml_node_list, link);
	}

	/* Search */
	if (curr_node != NULL) {
		acpi_dbg("acpi_sdt: Search from: \\");
		aml_print_name_seg(curr_node->name);
		acpi_dbg("\n");
		node = aml_find_node_in_tree(path, root, curr_node, false);
	} else
		node = NULL;

	*buffer = NULL;
	status = AE_OK;
	if ((node != NULL) && (node->buffer != NULL))
		*buffer = node->buffer;

	/* 3. free the tree */
	aml_destruct_node_list(root);
	return status;
}

acpi_status_t acpi_sdt_open_ex(uint8_t *buffer,
			       size_t buffer_size,
			       ACPI_HANDLE *paml)
{
	struct aml_op_info *info;
	struct aml_handle *handle;

	info = aml_search_by_opcode(buffer);
	if (info == NULL)
		return AE_BAD_PARAMETER;

	/* Do not open NameString as handle */
	if ((info->attr & AML_IS_NAME_CHAR) != 0)
		return AE_BAD_PARAMETER;

	/* Good, find it */
	handle = acpi_os_allocate(sizeof (*handle));
	BUG_ON(!handle);

	handle->signature = EFI_AML_HANDLE_SIGNATURE;
	handle->buffer = buffer;
	handle->info = info;
	handle->modified = false;

	handle->size = aml_get_object_size(info, buffer, buffer_size);
	if (handle->size == 0) {
		acpi_os_free(handle);
		return AE_BAD_PARAMETER;
	}
	*paml = (ACPI_HANDLE)handle;
	return AE_OK;
}

acpi_status_t acpi_sdt_close(ACPI_HANDLE aml)
{
	struct aml_handle *handle = (struct aml_handle *)aml;

	/* Check for invalid input parameters */
	if (!handle)
		return AE_BAD_PARAMETER;

	if ((handle->signature != EFI_AML_ROOT_HANDLE_SIGNATURE) &&
	    (handle->signature != EFI_AML_HANDLE_SIGNATURE))
		return AE_BAD_PARAMETER;

	/* Update Checksum only if modified */
	if (handle->modified)
		acpi_table_calc_checksum(ACPI_CAST_PTR(struct acpi_table_header,
						       handle->buffer));
	acpi_os_free(handle);
	return AE_OK;
}

struct aml_handle *aml_duplicate_handle(struct aml_handle *handle)
{
	struct aml_handle *dsdt_handle;

	dsdt_handle = acpi_os_allocate(sizeof (*dsdt_handle));
	BUG_ON(!dsdt_handle);
	memcpy(dsdt_handle, handle, sizeof (*dsdt_handle));
	return dsdt_handle;
}

acpi_status_t aml_get_child_from_object_buffer(struct aml_handle *parent_handle,
					       uint8_t *curr_buffer,
					       uint8_t **buffer)
{
	struct aml_op_info *info;
	size_t data_size;

	/* Root is considered as SCOPE, which has TermList. We need return only
	 * Object in TermList.
	 */
	while (curr_buffer < (parent_handle->buffer + parent_handle->size)) {
		info = aml_search_by_opcode(curr_buffer);
		if (!info)
			return AE_BAD_PARAMETER;

		/* NOTE: We need return everything, because user might need parse
		 *       the returned object.
		 */
		if ((info->attr & AML_IS_NAME_CHAR) == 0) {
			*buffer = curr_buffer;
			return AE_OK;
		}

		data_size = aml_get_object_size(info, curr_buffer,
						parent_handle->buffer + parent_handle->size - curr_buffer);
		if (!data_size)
			return AE_BAD_PARAMETER;
		curr_buffer += data_size;
	}

	*buffer = NULL;
	return AE_OK;
}

acpi_status_t aml_get_child_from_root(struct aml_handle *parent_handle,
				      struct aml_handle *handle,
				      uint8_t **buffer)
{
	uint8_t *curr_buffer;

	if (!handle) {
		/* First One */
		curr_buffer = parent_handle->buffer;
	} else
		curr_buffer = handle->buffer + handle->size;
	return aml_get_child_from_object_buffer(parent_handle, curr_buffer, buffer);
}

acpi_status_t aml_get_child_from_option_list(struct aml_handle *parent_handle,
					     struct aml_handle *handle,
					     uint8_t **buffer)
{
	AML_OP_PARSE_FORMAT data_type;
	uint8_t *data;
	size_t data_size;
	AML_OP_PARSE_INDEX index;
	acpi_status_t status;
	AML_OP_PARSE_INDEX max_term;

	index = AML_OP_PARSE_INDEX_GET_TERM1;
	max_term = parent_handle->info->max_index;
	while (index <= max_term) {
		status = aml_parse_option_handle_common(parent_handle,
							index,
							&data_type,
							&data, &data_size);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;
		if (data_type == AML_NONE)
			break;

		/* Find it, and Check Data */
		if ((data_type == AML_OBJECT) &&
		    handle->buffer < data) {
			/* Buffer < Data means current node is next one */
			*buffer = data;
			return AE_OK;
		}
		/* Not Child */
		index++;
	}
	*buffer = NULL;
	return AE_OK;
}

acpi_status_t aml_get_child_from_object_child_list(struct aml_handle *parent_handle,
						   struct aml_handle *handle,
						   uint8_t **buffer)
{
	acpi_status_t status;
	uint8_t *curr_buffer;

	curr_buffer = NULL;

	if ((parent_handle->info->attr & AML_HAS_CHILD_OBJ) == 0) {
		/* No ObjectList */
		*buffer = NULL;
		return AE_OK;
	}

	/* Do we need add node within METHOD? Yes, just add Object is OK.
	 * But we need filter NameString for METHOD invoke.
	 */
	/* Now, we get the last node. */
	status = aml_get_offset_after_last_option(parent_handle, &curr_buffer);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;

	/* Go through all the reset buffer. */
	if (handle->buffer < curr_buffer) {
		/* Buffer < Data means next node is first object */
	} else if (handle->buffer + handle->size <
		   parent_handle->buffer + parent_handle->size) {
		/* There is still more node */
		curr_buffer = handle->buffer + handle->size;
	} else {
		/* No more data */
		*buffer = NULL;
		return AE_OK;
	}
	return aml_get_child_from_object_buffer(parent_handle, curr_buffer, buffer);
}

acpi_status_t aml_get_child_from_non_root(struct aml_handle *parent_handle,
					  struct aml_handle *handle,
					  uint8_t **buffer)
{
	acpi_status_t status;

	/* NULL means first one */
	if (!handle)
		handle = parent_handle;
	/* 1. Get Option */
	status = aml_get_child_from_option_list(parent_handle, handle, buffer);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;
	if (*buffer != NULL)
		return AE_OK;
	/* 2. search ObjectList */
	return aml_get_child_from_object_child_list(parent_handle, handle, buffer);
}

acpi_status_t acpi_sdt_get_child(ACPI_HANDLE aml,
				 ACPI_HANDLE *paml)
{
	struct aml_handle *parent_handle = (struct aml_handle *)aml;
	struct aml_handle *handle;
	uint8_t *buffer;
	acpi_status_t status;

	BUG_ON(!paml);

	/* Check for invalid input parameters */
	if (parent_handle == NULL)
		return AE_BAD_PARAMETER;

	handle = *paml;
	if ((handle != NULL) && (handle->signature != EFI_AML_HANDLE_SIGNATURE))
		return AE_BAD_PARAMETER;

	if (parent_handle->signature == EFI_AML_ROOT_HANDLE_SIGNATURE)
		status = aml_get_child_from_root(parent_handle, handle, &buffer);
	else if (parent_handle->signature == EFI_AML_HANDLE_SIGNATURE)
		status = aml_get_child_from_non_root(parent_handle, handle, &buffer);
	else
		return AE_BAD_PARAMETER;
	if (ACPI_FAILURE(status))
		return AE_NOT_FOUND;
	if (!buffer) {
		*paml = NULL;
		return AE_OK;
	}
	return acpi_sdt_open_ex(buffer,
				parent_handle->buffer + parent_handle->size - buffer,
				paml);
}

acpi_status_t acpi_sdt_find_path_from_non_root(ACPI_HANDLE aml,
  					       uint8_t *aml_path,
					       ACPI_HANDLE *paml)
{
	struct aml_handle *handle = (struct aml_handle *)aml;
	uint8_t *buffer;
	acpi_status_t status;

	buffer = NULL;

	/* For non-root handle, we need search from THIS node instead of
	 * ROOT.
	 */
	status = aml_find_path(handle, aml_path, &buffer, false);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;

	if (buffer == NULL) {
		*paml = NULL;
		return AE_OK;
	}
	return acpi_sdt_open_ex(buffer,
				handle->buffer + handle->size - buffer,
				paml);
}

acpi_status_t acpi_sdt_find_path_from_root(ACPI_HANDLE aml,
					   uint8_t *aml_path,
					   ACPI_HANDLE *paml)
{
	struct aml_handle *handle = (struct aml_handle *)aml;
	ACPI_HANDLE child_handle;
	acpi_status_t status;
	uint8_t *buffer;

	buffer = NULL;

	/* Handle case that AcpiPath is Root */
	if (aml_is_root_path(aml_path)) {
		/* Duplicate RootHandle */
		*(struct aml_handle **)paml = aml_duplicate_handle(handle);
		return AE_OK;
	}

	/* Let children find it. */
	child_handle = NULL;
	while (true) {
		status = acpi_sdt_get_child(handle, &child_handle);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;

		if (!child_handle) {
			/* No more children to search, path not found */
			*paml = NULL;
			return AE_OK;
		}

		/* More child */
		handle = (struct aml_handle *)child_handle;
		status = aml_find_path(handle, aml_path, &buffer, true);
		if (ACPI_FAILURE(status))
			return AE_BAD_PARAMETER;

		if (buffer) {
			/* Great! Find it, open */
			status = acpi_sdt_open_ex(buffer,
						  handle->buffer + handle->size - buffer,
						  paml);
			if (!ACPI_FAILURE(status))
				return AE_OK;
		}
	}
	return AE_NOT_FOUND;
}

acpi_status_t acpi_sdt_find_path(ACPI_HANDLE aml,
				 char *asl_path,
				 ACPI_HANDLE *paml)
{
	struct aml_handle *handle = (struct aml_handle *)aml;
	acpi_status_t status;
	uint8_t *path;

	/* Check for invalid input parameters */
	if (!handle)
		return AE_BAD_PARAMETER;

	/* Convert ASL path to AML path */
	path = aml_name_from_asl_name(asl_path);
	if (!path)
		return AE_BAD_PARAMETER;

	acpi_os_debug_print("acpi_sdt: find_path - ");
	aml_print_name_seg(path);
	acpi_os_debug_print("\n");

	if (handle->signature == EFI_AML_ROOT_HANDLE_SIGNATURE)
		status = acpi_sdt_find_path_from_root(handle, path, paml);
	else if (handle->signature == EFI_AML_HANDLE_SIGNATURE)
		status = acpi_sdt_find_path_from_non_root(handle, path, paml);
	else
		status = AE_BAD_PARAMETER;

	acpi_os_free(path);
	return status;
}

acpi_status_t acpi_sdt_set_option(ACPI_HANDLE aml,
				  int index,
				  uint8_t *data,
				  size_t data_size)
{
	struct aml_handle *handle = (struct aml_handle *)aml;
	struct aml_op_info *info;
	acpi_status_t status;
	AML_OP_PARSE_FORMAT data_type;
	uint8_t *org_data;
	size_t org_data_size;

	BUG_ON(!data);

	/* Check for invalid input parameters */
	if (!handle)
		return AE_BAD_PARAMETER;

	/* Do not check EFI_AML_ROOT_HANDLE_SIGNATURE because there is no
	 * option for Root handle
	 */
	if (handle->signature != EFI_AML_HANDLE_SIGNATURE)
		return AE_BAD_PARAMETER;
	info = handle->info;
	if (index > info->max_index)
		return AE_BAD_PARAMETER;

	/* Parse option */
	status = aml_parse_option_handle_common(handle,
		(AML_OP_PARSE_INDEX)index,
		&data_type, &org_data, &org_data_size);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;
	if (data_type == AML_NONE)
		return AE_BAD_PARAMETER;
	if (data_size > org_data_size)
		return AE_BAD_DATA;

	/* Update */
	memcpy(org_data, data, data_size);
	handle->modified = true;
	return AE_OK;
}

acpi_status_t acpi_sdt_get_option(ACPI_HANDLE aml,
				  int index,
				  AML_OP_PARSE_FORMAT *data_type,
				  uint8_t **data,
				  size_t *data_size)
{
	struct aml_handle *handle = (struct aml_handle *)aml;
	struct aml_op_info *info;
	acpi_status_t status;

	BUG_ON(!data_type);
	BUG_ON(!data);
	BUG_ON(!data_size);

	/* Check for invalid input parameters */
	if (!handle)
		return AE_BAD_PARAMETER;

	/* Do not check EFI_AML_ROOT_HANDLE_SIGNATURE because there is no
	 * option for Root handle
	 */
	if (handle->signature != EFI_AML_HANDLE_SIGNATURE)
		return AE_BAD_PARAMETER;

	info = handle->info;
	if (index > info->max_index) {
		*data_type = AML_NONE;
		return AE_OK;
	}

	/* Parse option */
	status = aml_parse_option_handle_common(handle,
		(AML_OP_PARSE_INDEX)index, data_type, data, data_size);
	if (ACPI_FAILURE(status))
		return AE_BAD_PARAMETER;
	return AE_OK;
}

acpi_status_t acpi_sdt_patch_STA(struct acpi_table_header *table,
				 char *path,
				 uint8_t sta)
{
	acpi_status_t status;
	ACPI_HANDLE obj;
	struct aml_handle *root_handle;
	AML_OP_PARSE_FORMAT type;
	size_t size;
	uint8_t *buffer;

	if (!table || !path) {
		acpi_err("acpi_sdt_patch_STA: invalid parameters\n");
		return AE_BAD_PARAMETER;
	}

	/* Create a root handle for the DSDT table */
	root_handle = acpi_os_allocate(sizeof(struct aml_handle));
	if (!root_handle) {
		acpi_err("acpi_sdt_patch_STA: failed to allocate root handle\n");
		return AE_NO_MEMORY;
	}

	root_handle->signature = EFI_AML_ROOT_HANDLE_SIGNATURE;
	root_handle->buffer = (uint8_t *)(table + sizeof(struct acpi_table_header));
	root_handle->size = table->length - sizeof(struct acpi_table_header);
	/* Set info to SCOPE_OP for root handle since DSDT is essentially a scope */
	{
		uint8_t scope_op = AML_SCOPE_OP;
		root_handle->info = aml_search_by_opcode(&scope_op);
	}
	root_handle->modified = false;


	status = acpi_sdt_find_path(root_handle, path, &obj);
	if (ACPI_FAILURE(status) || !obj) {
		acpi_err("acpi_sdt_patch_STA: failed to find path '%s', status: %d\n", path, status);
		goto cleanup;
	}

	status = acpi_sdt_get_option(obj, 2, &type, &buffer, &size);
	if (ACPI_FAILURE(status)) {
		acpi_err("acpi_sdt_patch_STA: failed to get option 2, status: %d\n", status);
		goto cleanup;
	}

	if (size < 4) {
		acpi_err("acpi_sdt_patch_STA: buffer size is too small, expected at least 4 bytes, got %lu\n", size);
		status = AE_BUFFER_OVERFLOW;
		goto cleanup;
	}

	buffer[3] = sta;

	status = AE_OK;

cleanup:
	if (obj)
		acpi_sdt_close(obj);

	if (root_handle)
		acpi_os_free(root_handle);

	return status;
}
