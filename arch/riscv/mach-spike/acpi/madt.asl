/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Template for [APIC] ACPI Table (static data table)
 * Format: [ByteLength]  FieldName : HexFieldValue
 */
[0004]                          Signature : "APIC"    [Multiple APIC Description Table (MADT)]
[0004]                       Table Length : 00000262
[0001]                           Revision : 07
[0001]                           Checksum : 44
[0006]                             Oem ID : "INTEL "
[0008]                       Oem Table ID : "Template"
[0004]                       Oem Revision : 00000001
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20230628

[0004]                 Local Apic Address : 00000000
[0004]              Flags (decoded below) : 00000001
                      PC-AT Compatibility : 1

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000000
[0004]                                Uid : 00000000
[0004]                          ExtIntcId : 00000001
[0008]                          ImsicAddr : 0000000000000000
[0004]                          ImsicSize : 00000000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000001
[0004]                                Uid : 00000001
[0004]                          ExtIntcId : 00000003
[0008]                          ImsicAddr : 0000000000000000
[0004]                          ImsicSize : 00000000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000002
[0004]                                Uid : 00000002
[0004]                          ExtIntcId : 00000005
[0008]                          ImsicAddr : 0000000000000000
[0004]                          ImsicSize : 00000000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000003
[0004]                                Uid : 00000003
[0004]                          ExtIntcId : 00000007
[0008]                          ImsicAddr : 0000000000000000
[0004]                          ImsicSize : 00000000

[0001]                      Subtable Type : 1B [RISC-V PLIC Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                                 Id : 00
[0008]                               HwId : 0000000000000000
[0002]                            NumIrqs : 0020
[0002]                            MaxPrio : 000F
[0004]                              Flags : 00000000
[0004]                               Size : 01000000
[0008]                           BaseAddr : 000000000C000000
[0004]                            GsiBase : 00000000
