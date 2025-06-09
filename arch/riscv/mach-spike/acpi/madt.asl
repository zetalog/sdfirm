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
[0004]                          ExtIntcId : 00000000
[0008]                          ImsicAddr : 0000000558600000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000001
[0004]                                Uid : 00000001
[0004]                          ExtIntcId : 00000001
[0008]                          ImsicAddr : 0000000558608000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000002
[0004]                                Uid : 00000002
[0004]                          ExtIntcId : 00000002
[0008]                          ImsicAddr : 0000000558610000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000003
[0004]                                Uid : 00000003
[0004]                          ExtIntcId : 00000003
[0008]                          ImsicAddr : 0000000558618000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000004
[0004]                                Uid : 00000004
[0004]                          ExtIntcId : 00000004
[0008]                          ImsicAddr : 0000000558620000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000005
[0004]                                Uid : 00000005
[0004]                          ExtIntcId : 00000005
[0008]                          ImsicAddr : 0000000558628000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000006
[0004]                                Uid : 00000006
[0004]                          ExtIntcId : 00000006
[0008]                          ImsicAddr : 0000000558630000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000007
[0004]                                Uid : 00000007
[0004]                          ExtIntcId : 00000007
[0008]                          ImsicAddr : 0000000558638000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000008
[0004]                                Uid : 00000008
[0004]                          ExtIntcId : 00000008
[0008]                          ImsicAddr : 0000000558640000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000009
[0004]                                Uid : 00000009
[0004]                          ExtIntcId : 00000009
[0008]                          ImsicAddr : 0000000558648000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000A
[0004]                                Uid : 0000000A
[0004]                          ExtIntcId : 0000000A
[0008]                          ImsicAddr : 0000000558650000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000B
[0004]                                Uid : 0000000B
[0004]                          ExtIntcId : 0000000B
[0008]                          ImsicAddr : 0000000558658000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000C
[0004]                                Uid : 0000000C
[0004]                          ExtIntcId : 0000000C
[0008]                          ImsicAddr : 0000000558660000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000D
[0004]                                Uid : 0000000D
[0004]                          ExtIntcId : 0000000D
[0008]                          ImsicAddr : 0000000558668000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000E
[0004]                                Uid : 0000000E
[0004]                          ExtIntcId : 0000000E
[0008]                          ImsicAddr : 0000000558670000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000000F
[0004]                                Uid : 0000000F
[0004]                          ExtIntcId : 0000000F
[0008]                          ImsicAddr : 0000000558678000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000010
[0004]                                Uid : 00000010
[0004]                          ExtIntcId : 00000010
[0008]                          ImsicAddr : 0000000558680000
[0004]                          ImsicSize : 00000000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000011
[0004]                                Uid : 00000011
[0004]                          ExtIntcId : 00000011
[0008]                          ImsicAddr : 0000000558688000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000012
[0004]                                Uid : 00000012
[0004]                          ExtIntcId : 00000012
[0008]                          ImsicAddr : 0000000558690000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000013
[0004]                                Uid : 00000013
[0004]                          ExtIntcId : 00000013
[0008]                          ImsicAddr : 0000000558698000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000014
[0004]                                Uid : 00000014
[0004]                          ExtIntcId : 00000014
[0008]                          ImsicAddr : 00000005586A0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000015
[0004]                                Uid : 00000015
[0004]                          ExtIntcId : 00000015
[0008]                          ImsicAddr : 00000005586A8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000016
[0004]                                Uid : 00000016
[0004]                          ExtIntcId : 00000016
[0008]                          ImsicAddr : 00000005586B0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000017
[0004]                                Uid : 00000017
[0004]                          ExtIntcId : 00000017
[0008]                          ImsicAddr : 00000005586B8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000018
[0004]                                Uid : 00000018
[0004]                          ExtIntcId : 00000018
[0008]                          ImsicAddr : 00000005586C0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000019
[0004]                                Uid : 00000019
[0004]                          ExtIntcId : 00000019
[0008]                          ImsicAddr : 00000005586C8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001A
[0004]                                Uid : 0000001A
[0004]                          ExtIntcId : 0000001A
[0008]                          ImsicAddr : 00000005586D0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001B
[0004]                                Uid : 0000001B
[0004]                          ExtIntcId : 0000001B
[0008]                          ImsicAddr : 00000005586D8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001C
[0004]                                Uid : 0000001C
[0004]                          ExtIntcId : 0000001C
[0008]                          ImsicAddr : 00000005586E0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001D
[0004]                                Uid : 0000001D
[0004]                          ExtIntcId : 0000001D
[0008]                          ImsicAddr : 00000005586E8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001E
[0004]                                Uid : 0000001E
[0004]                          ExtIntcId : 0000001E
[0008]                          ImsicAddr : 00000005586F0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000001F
[0004]                                Uid : 0000001F
[0004]                          ExtIntcId : 0000001F
[0008]                          ImsicAddr : 00000005586F8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000020
[0004]                                Uid : 00000020
[0004]                          ExtIntcId : 00000020
[0008]                          ImsicAddr : 0000000558700000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000021
[0004]                                Uid : 00000021
[0004]                          ExtIntcId : 00000021
[0008]                          ImsicAddr : 0000000558708000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000022
[0004]                                Uid : 00000022
[0004]                          ExtIntcId : 00000022
[0008]                          ImsicAddr : 0000000558710000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000023
[0004]                                Uid : 00000023
[0004]                          ExtIntcId : 00000023
[0008]                          ImsicAddr : 0000000558718000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000024
[0004]                                Uid : 00000024
[0004]                          ExtIntcId : 00000024
[0008]                          ImsicAddr : 0000000558720000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000025
[0004]                                Uid : 00000025
[0004]                          ExtIntcId : 00000025
[0008]                          ImsicAddr : 0000000558728000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000026
[0004]                                Uid : 00000026
[0004]                          ExtIntcId : 00000026
[0008]                          ImsicAddr : 0000000558730000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000027
[0004]                                Uid : 00000027
[0004]                          ExtIntcId : 00000027
[0008]                          ImsicAddr : 0000000558738000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000028
[0004]                                Uid : 00000028
[0004]                          ExtIntcId : 00000028
[0008]                          ImsicAddr : 0000000558740000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000029
[0004]                                Uid : 00000029
[0004]                          ExtIntcId : 00000029
[0008]                          ImsicAddr : 0000000558748000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002A
[0004]                                Uid : 0000002A
[0004]                          ExtIntcId : 0000002A
[0008]                          ImsicAddr : 0000000558750000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002B
[0004]                                Uid : 0000002B
[0004]                          ExtIntcId : 0000002B
[0008]                          ImsicAddr : 0000000558758000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002C
[0004]                                Uid : 0000002C
[0004]                          ExtIntcId : 0000002C
[0008]                          ImsicAddr : 0000000558760000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002D
[0004]                                Uid : 0000002D
[0004]                          ExtIntcId : 0000002D
[0008]                          ImsicAddr : 0000000558768000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002E
[0004]                                Uid : 0000002E
[0004]                          ExtIntcId : 0000002E
[0008]                          ImsicAddr : 0000000558770000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000002F
[0004]                                Uid : 0000002F
[0004]                          ExtIntcId : 0000002F
[0008]                          ImsicAddr : 0000000558778000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000030
[0004]                                Uid : 00000030
[0004]                          ExtIntcId : 00000030
[0008]                          ImsicAddr : 0000000558780000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000031
[0004]                                Uid : 00000031
[0004]                          ExtIntcId : 00000031
[0008]                          ImsicAddr : 0000000558788000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000032
[0004]                                Uid : 00000032
[0004]                          ExtIntcId : 00000032
[0008]                          ImsicAddr : 0000000558790000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000033
[0004]                                Uid : 00000033
[0004]                          ExtIntcId : 00000033
[0008]                          ImsicAddr : 0000000558798000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000034
[0004]                                Uid : 00000034
[0004]                          ExtIntcId : 00000034
[0008]                          ImsicAddr : 00000005587A0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000035
[0004]                                Uid : 00000035
[0004]                          ExtIntcId : 00000035
[0008]                          ImsicAddr : 00000005587A8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000036
[0004]                                Uid : 00000036
[0004]                          ExtIntcId : 00000036
[0008]                          ImsicAddr : 00000005587B0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000037
[0004]                                Uid : 00000037
[0004]                          ExtIntcId : 00000037
[0008]                          ImsicAddr : 00000005587B8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000038
[0004]                                Uid : 00000038
[0004]                          ExtIntcId : 00000038
[0008]                          ImsicAddr : 00000005587C0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 0000000000000039
[0004]                                Uid : 00000039
[0004]                          ExtIntcId : 00000039
[0008]                          ImsicAddr : 00000005587C8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003A
[0004]                                Uid : 0000003A
[0004]                          ExtIntcId : 0000003A
[0008]                          ImsicAddr : 00000005587D0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003B
[0004]                                Uid : 0000003B
[0004]                          ExtIntcId : 0000003B
[0008]                          ImsicAddr : 00000005587D8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003C
[0004]                                Uid : 0000003C
[0004]                          ExtIntcId : 0000003C
[0008]                          ImsicAddr : 00000005587E0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003D
[0004]                                Uid : 0000003D
[0004]                          ExtIntcId : 0000003D
[0008]                          ImsicAddr : 00000005587E8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003E
[0004]                                Uid : 0000003E
[0004]                          ExtIntcId : 0000003E
[0008]                          ImsicAddr : 00000005587F0000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 18 [RISC-V Interrupt Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000001
[0008]                             HartId : 000000000000003F
[0004]                                Uid : 0000003F
[0004]                          ExtIntcId : 0000003F
[0008]                          ImsicAddr : 00000005587F8000
[0004]                          ImsicSize : 00008000

[0001]                      Subtable Type : 19 [RISC-V Incoming MSI Controller]
[0001]                             Length : 10
[0001]                            Version : 01
[0001]                           Reserved : 00
[0004]                              Flags : 00000000
[0002]                             NumIds : 00FF
[0002]                        NumGuestIds : 003F
[0001]                     GuestIndexBits : 03
[0001]                      HartIndexBits : 06
[0001]                     GroupIndexBits : 01
[0001]                    GroupIndexShift : 21

[0001]                      Subtable Type : 1A [RISC-V APLIC Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                                 Id : 00
[0004]                              Flags : 00000000
[0008]                               HwId : 0000000000000000
[0002]                            NumIdcs : 0000
[0002]                         NumSources : 01FF
[0004]                            GsiBase : 00000000
[0008]                           BaseAddr : 0000000558500000
[0004]                               Size : 00004000

[0001]                      Subtable Type : 1B [RISC-V PLIC Controller]
[0001]                             Length : 24
[0001]                            Version : 01
[0001]                                 Id : 00
[0008]                               HwId : 0000000000000000
[0002]                            NumIrqs : 0060
[0002]                            MaxPrio : 0000
[0004]                              Flags : 00000000
[0004]                               Size : 00600000
[0008]                           BaseAddr : 000000000C000000
[0004]                            GsiBase : 00000000
