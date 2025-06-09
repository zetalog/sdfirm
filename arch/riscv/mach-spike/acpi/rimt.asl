/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Template for [RIMT] ACPI Table (static data table)
 * Format: [ByteLength]  FieldName : HexFieldValue
 */
[0004]                          Signature : "RIMT"    
[0004]                       Table Length : 00000080
[0001]                           Revision : 01
[0001]                           Checksum : E1
[0006]                             Oem ID : "BOCHS "
[0008]                       Oem Table ID : "BXPC    "
[0004]                       Oem Revision : 00000001
[0004]                    Asl Compiler ID : "BXPC"
[0004]              Asl Compiler Revision : 00000001

[0004]               Number of RIMT Nodes : 00000002
[0004]          Offset to RIMT Node Array : 00000030
[0004]                           Reserved : 00000000

[0001]                               Type : 00
[0001]                           Revision : 01
[0002]                             Length : 0028
[0002]                           Reserved : 0000
[0002]                                 ID : 0000
[0008]                        Hardware ID : "RSCV0004"
[0008]                       Base Address : 0000000003010000
[0004]                              Flags : 00000000
[0004]                   Proximity Domain : 00000000
[0002]                PCIe Segment number : 0000
[0002]                         PCIe B/D/F : 0000
[0002]          Number of interrupt wires : 0000
[0002]        Interrupt wire array offset : 0028

[0001]                               Type : 01
[0001]                           Revision : 01
[0002]                             Length : 0028
[0002]                           Reserved : 0000
[0002]                                 ID : 0001
[0004]                              Flags : 00000000
[0002]                           Reserved : 0000
[0002]                PCIe Segment number : 0000
[0002]            ID mapping array offset : 0014
[0002]              Number of ID mappings : 0001

[0004]                     Source ID Base : 00000000
[0004]                      Number of IDs : 0000FFFF
[0004]         Destination Device ID Base : 00000000
[0004]           Destination IOMMU Offset : 00000030
[0004]                              Flags : 00000000
