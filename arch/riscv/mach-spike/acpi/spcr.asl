/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 *
 * Template for [SPCR] ACPI Table (static data table)
 * Format: [ByteLength]  FieldName : HexFieldValue
 */
[0004]                          Signature : "SPCR"    [Serial Port Console Redirection Table]
[0004]                       Table Length : 0000005A
[0001]                           Revision : 04
[0001]                           Checksum : 4E
[0006]                             Oem ID : "INTEL "
[0008]                       Oem Table ID : "TEMPLATE"
[0004]                       Oem Revision : 00000000
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20100528

[0001]                     Interface Type : 00
[0003]                           Reserved : 000000

[0012]               Serial Port Register : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 08
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 01 [Undefined/Legacy]
[0008]                            Address : 0000000010000000

[0001]                     Interrupt Type : 10
[0001]                PCAT-compatible IRQ : 00
[0004]                          Interrupt : 00000001
[0001]                          Baud Rate : 00
[0001]                             Parity : 00
[0001]                          Stop Bits : 01
[0001]                       Flow Control : 00
[0001]                      Terminal Type : 03
[0001]                           Language : 00
[0002]                      PCI Device ID : 0000
[0002]                      PCI Vendor ID : 0000
[0001]                            PCI Bus : 00
[0001]                         PCI Device : 00
[0001]                       PCI Function : 00
[0004]                          PCI Flags : 00000000
[0001]                        PCI Segment : 00
[0004]                    Uart Clock Freq : 00989680
[0004]                  Precise Baud rate : 0001C200
[0002]              NameSpaceStringLength : 0002
[0002]              NameSpaceStringOffset : 0058
[0002]                    NamespaceString : "."
