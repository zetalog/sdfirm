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

[0001]                      Subtable Type : 00 [Processor Local APIC]
[0001]                             Length : 08
[0001]                       Processor ID : 00
[0001]                      Local Apic ID : 00
[0004]              Flags (decoded below) : 00000001
                        Processor Enabled : 1
                   Runtime Online Capable : 0

[0001]                      Subtable Type : 01 [I/O APIC]
[0001]                             Length : 0C
[0001]                        I/O Apic ID : 01
[0001]                           Reserved : 00
[0004]                            Address : 00000000
[0004]                          Interrupt : 00000000

[0001]                      Subtable Type : 02 [Interrupt Source Override]
[0001]                             Length : 0A
[0001]                                Bus : 00
[0001]                             Source : 00
[0004]                          Interrupt : 00000000
[0002]              Flags (decoded below) : 0000
                                 Polarity : 0
                             Trigger Mode : 0

[0001]                      Subtable Type : 03 [NMI Source]
[0001]                             Length : 08
[0002]              Flags (decoded below) : 000D
                                 Polarity : 1
                             Trigger Mode : 3
[0004]                          Interrupt : 00000001

[0001]                      Subtable Type : 04 [Local APIC NMI]
[0001]                             Length : 06
[0001]                       Processor ID : 00
[0002]              Flags (decoded below) : 0005
                                 Polarity : 1
                             Trigger Mode : 1
[0001]               Interrupt Input LINT : 01

[0001]                      Subtable Type : 05 [Local APIC Address Override]
[0001]                             Length : 0C
[0002]                           Reserved : 0000
[0008]                       APIC Address : 0000000000000000

[0001]                      Subtable Type : 06 [I/O SAPIC]
[0001]                             Length : 10
[0001]                       I/O Sapic ID : 00
[0001]                           Reserved : 00
[0004]                     Interrupt Base : 00000000
[0008]                            Address : 0000000000000000

[0001]                      Subtable Type : 07 [Local SAPIC]
[0001]                             Length : 16
[0001]                       Processor ID : 00
[0001]                     Local Sapic ID : 00
[0001]                    Local Sapic EID : 00
[0003]                           Reserved : 000000
[0004]              Flags (decoded below) : 00000001
                        Processor Enabled : 1
[0004]                      Processor UID : 00000000
[0006]               Processor UID String : "\CPU0"

[0001]                      Subtable Type : 08 [Platform Interrupt Sources]
[0001]                             Length : 10
[0002]              Flags (decoded below) : 0005
                                 Polarity : 1
                             Trigger Mode : 1
[0001]                      InterruptType : 00
[0001]                       Processor ID : 00
[0001]                      Processor EID : 00
[0001]                   I/O Sapic Vector : 00
[0004]                          Interrupt : 00000001
[0004]              Flags (decoded below) : 00000001
                            CPEI Override : 1

[0001]                      Subtable Type : 09 [Processor Local x2APIC]
[0001]                             Length : 10
[0002]                           Reserved : 0000
[0004]                Processor x2Apic ID : 00000000
[0004]              Flags (decoded below) : 00000001
                        Processor Enabled : 1
[0004]                      Processor UID : 00000000

[0001]                      Subtable Type : 0A [Local x2APIC NMI]
[0001]                             Length : 0C
[0002]              Flags (decoded below) : 0005
                                 Polarity : 1
                             Trigger Mode : 1
[0004]                      Processor UID : 00000000
[0001]               Interrupt Input LINT : 00
[0003]                           Reserved : 000000

[0001]                      Subtable Type : 0B [Generic Interrupt Controller]
[0001]                             Length : 52
[0002]                           Reserved : 0000
[0004]               CPU Interface Number : 00000000
[0004]                      Processor UID : 00000000
[0004]              Flags (decoded below) : 00000001
                        Processor Enabled : 1
       Performance Interrupt Trigger Mode : 0
       Virtual GIC Interrupt Trigger Mode : 0
                           Online Capable : 0
                        GICR non-coherent : 0
[0004]           Parking Protocol Version : 00000000
[0004]              Performance Interrupt : 00000000
[0008]                     Parked Address : 0000000000000000
[0008]                       Base Address : 0000000000000000
[0008]           Virtual GIC Base Address : 0000000000000000
[0008]        Hypervisor GIC Base Address : 0000000000000000
[0004]              Virtual GIC Interrupt : 00000000
[0008]         Redistributor Base Address : 0000000000000000
[0008]                          ARM MPIDR : 0000000000000000
[0001]                   Efficiency Class : 00
[0001]                           Reserved : 00
[0002]             SPE Overflow Interrupt : 0000
[0002]                     TRBE Interrupt : 180C

[0001]                      Subtable Type : 0C [Generic Interrupt Distributor]
[0001]                             Length : 18
[0002]                           Reserved : 0000
[0004]              Local GIC Hardware ID : 00000000
[0008]                       Base Address : 0000000000000000
[0004]                     Interrupt Base : 00000000
[0001]                            Version : 01
[0003]                           Reserved : 000000

[0001]                      Subtable Type : 0D [Generic MSI Frame]
[0001]                             Length : 18
[0002]                           Reserved : 0000
[0004]                       MSI Frame ID : 00000000
[0008]                       Base Address : 0000000000000000
[0004]              Flags (decoded below) : 00000001
                               Select SPI : 1
[0002]                          SPI Count : 0000
[0002]                           SPI Base : 0000

[0001]                      Subtable Type : 0E [Generic Interrupt Redistributor]
[0001]                             Length : 10
[0001]              Flags (decoded below) : 00
                        GICR non-coherent : 0
[0001]                           Reserved : 00
[0008]                       Base Address : 0000000000000000
[0004]                             Length : 00000000

[0001]                      Subtable Type : 0F [Generic Interrupt Translator]
[0001]                             Length : 14
[0001]              Flags (decoded below) : 00
                     GIC ITS non-coherent : 0
[0001]                           Reserved : 00
[0004]                     Translation ID : 00000000
[0008]                       Base Address : 0000000000000000
[0004]                           Reserved : 00000000

[0001]                      Subtable Type : 10 [Mutiprocessor Wakeup]
[0001]                             Length : 10
[0002]                    Mailbox Version : 0000
[0004]                           Reserved : 00000000
[0008]                    Mailbox Address : 0000000000000000

[0001]                      Subtable Type : 11 [CPU Core Interrupt Controller]
[0001]                             Length : 0F
[0001]                            Version : 01
[0004]                        ProcessorId : 00000001
[0004]                             CoreId : 00000000
[0004]                              Flags : 00000001

[0001]                      Subtable Type : 12 [Legacy I/O Interrupt Controller]
[0001]                             Length : 17
[0001]                            Version : 01
[0008]                            Address : 000000001FE01400
[0002]                               Size : 0080
[0002]                            Cascade : 0302
[0008]                         CascadeMap : FF00000000FFFFFF

[0001]                      Subtable Type : 13 [HT Interrupt Controller]
[0001]                             Length : 15
[0001]                            Version : 01
[0008]                            Address : 00000EFDFB000080
[0002]                               Size : 0040
[0008]                            Cascade : 00000000FF000000

[0001]                      Subtable Type : 14 [Extend I/O Interrupt Controller]
[0001]                             Length : 0D
[0001]                            Version : 01
[0001]                            Cascade : 03
[0001]                               Node : 00
[0008]                            NodeMap : 0000000000000000

[0001]                      Subtable Type : 15 [MSI Interrupt Controller]
[0001]                             Length : 13
[0001]                            Version : 01
[0008]                         MsgAddress : 000000002FF00000
[0004]                              Start : 00000040
[0004]                              Count : 000000C0

[0001]                      Subtable Type : 16 [Bridge I/O Interrupt Controller]
[0001]                             Length : 11
[0001]                            Version : 01
[0008]                            Address : 0000000010000000
[0002]                               Size : 1000
[0002]                                 Id : 0000
[0002]                            GsiBase : 0040

[0001]                      Subtable Type : 17 [LPC Interrupt Controller]
[0001]                             Length : 0E
[0001]                            Version : 01
[0008]                            Address : 0000000010002000
[0002]                               Size : 1000
[0001]                            Cascade : 13

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
