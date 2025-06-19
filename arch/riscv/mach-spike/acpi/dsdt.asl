/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 *
 * Template for [DSDT] ACPI Table (AML byte code table)
 */
DefinitionBlock ("", "DSDT", 2, "SPACET", "_DSDT_01", 0x00000001)
{
    Scope (_SB)
    {
        Device (CPU0)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, Zero)  // _UID: Unique ID
            Name (_STA, 0x0F)  // _STA: Status
            Name (_MAT, Buffer (0x24)  // _MAT: Multiple APIC Table Entry
            {
                /* 0000 */  0x18, 0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  // .$......
                /* 0008 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ........
                /* 0010 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // ........
                /* 0018 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ........
                /* 0020 */  0x00, 0x00, 0x00, 0x00                           // ....
            })
        }

        Device (CPU1)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, One)  // _UID: Unique ID
            Name (_STA, 0x0F)  // _STA: Status
            Name (_MAT, Buffer (0x24)  // _MAT: Multiple APIC Table Entry
            {
                /* 0000 */  0x18, 0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  // .$......
                /* 0008 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // ........
                /* 0010 */  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03,  // ........
                /* 0018 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ........
                /* 0020 */  0x00, 0x00, 0x00, 0x00                           // ....
            })
        }

        Device (CPU2)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x02)  // _UID: Unique ID
            Name (_STA, 0x0F)  // _STA: Status
            Name (_MAT, Buffer (0x24)  // _MAT: Multiple APIC Table Entry
            {
                /* 0000 */  0x18, 0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  // .$......
                /* 0008 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,  // ........
                /* 0010 */  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05,  // ........
                /* 0018 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ........
                /* 0020 */  0x00, 0x00, 0x00, 0x00                           // ....
            })
        }

        Device (CPU3)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x03)  // _UID: Unique ID
            Name (_STA, 0x0F)  // _STA: Status
            Name (_MAT, Buffer (0x24)  // _MAT: Multiple APIC Table Entry
            {
                /* 0000 */  0x18, 0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  // .$......
                /* 0008 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,  // ........
                /* 0010 */  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07,  // ........
                /* 0018 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ........
                /* 0020 */  0x00, 0x00, 0x00, 0x00                           // ....
            })
        }
    }

    Scope (_SB)
    {
        Device (PLIC)
        {
            Name (_HID, "RSCV0001")  // _HID: Hardware ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                    0x0000000000000000, // Granularity
                    0x000000000C000000, // Range Minimum
                    0x000000000CFFFFFF, // Range Maximum
                    0x0000000000000000, // Translation Offset
                    0x0000000001000000, // Length
                    ,, , AddressRangeMemory, TypeStatic)
            })
            Method (_GSB, 0, NotSerialized)  // _GSB: Global System Interrupt Base
            {
                Return (Zero)
            }
        }
    }

    Scope (_SB)
    {
        Device (URT)
        {
            Name (_HID, "RSCV0003")  // _HID: Hardware ID
            Name (_UID, Zero)  // _UID: Unique ID
            Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
            {
                ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301") /* Device Properties for _DSD */,
                Package (0x03)
                {
                    Package (0x02)
                    {
                        "clock-frequency",
                        0x00989680
                    },

                    Package (0x02)
                    {
                        "reg-shift",
                        Zero
                    },

                    Package (0x02)
                    {
                        "reg-io-width",
                        One
                    }
                }
            })
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                QWordMemory (ResourceConsumer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                    0x0000000000000000, // Granularity
                    0x0000000010000000, // Range Minimum
                    0x00000000100000FF, // Range Maximum
                    0x0000000000000000, // Translation Offset
                    0x0000000000000100, // Length
                    ,, , AddressRangeMemory, TypeStatic)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                    0x00000001,
                }
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }
        }
    }
}
