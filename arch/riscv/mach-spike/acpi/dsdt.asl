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
}
