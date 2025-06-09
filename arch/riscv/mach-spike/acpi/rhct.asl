/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Template for [RHCT] ACPI Table (static data table)
 * Format: [ByteLength]  FieldName : HexFieldValue
 */
[0004]                          Signature : "RHCT"    [RISC-V Hart Capabilities Table]
[0004]                       Table Length : 00000096
[0001]                           Revision : 01
[0001]                           Checksum : 24
[0006]                             Oem ID : "OEMCA"
[0008]                       Oem Table ID : "TEMPLATE"
[0004]                       Oem Revision : 00000001
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20220928

[0004]                              Flags : 00000000
[0008]               Timer Base Frequency : 0000000000989680
[0004]                    Number of nodes : 00000002
[0004]           Offset to the node array : 00000038

[0002]                      Subtable Type : 0000 [RISC-V ISA string structure]
[0002]                             Length : 0034
[0002]                           Revision : 0001
[0002]                  ISA string length : 002B
[0043]                         ISA string : "rv64imafdch_zicsr_zifencei_zba_zbb_zbc_zbs"
[0001]                   Optional Padding : 00                                              /* . */\

[0002]                      Subtable Type : FFFF [RISC-V Hart Info structure]
[0002]                             Length : 0018
[0002]                           Revision : 0001
[0002]                  Number of offsets : 0003
[0004]                      Processor UID : 00000000
[0004]                              Nodes : 00000038
[0004]                              Nodes : 0000007C
[0004]                              Nodes : 0000008E

[0002]                      Subtable Type : 0001 [RISC-V CMO node structure]
[0002]                             Length : 000A
[0002]                           Revision : 0001
[0001]                           Reserved : 00
[0001]                    CBOM Block Size : 06
[0001]                    CBOP Block Size : 06
[0001]                    CBOZ Block Size : 06

[0002]                      Subtable Type : 0002 [RISC-V MMU node structure]
[0002]                             Length : 0008
[0002]                           Revision : 0001
[0001]                           Reserved : 00
[0001]                           MMU Type : 02
