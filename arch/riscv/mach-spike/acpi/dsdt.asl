/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Template for [DSDT] ACPI Table (AML byte code table)
 */
DefinitionBlock ("", "DSDT", 2, "SPACET", "_DSDT_01", 0x00000001)
{
    Device (CPU0)
    {
        Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
        Name (_UID, Zero)  // _UID: Unique ID
        Name (_STA, 0x0F)  // _STA: Status
    }

    Device (CPU1)
    {
        Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
        Name (_UID, One)  // _UID: Unique ID
        Name (_STA, 0x0F)  // _STA: Status
    }

    Device (CPU2)
    {
        Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
        Name (_UID, 0x02)  // _UID: Unique ID
        Name (_STA, 0x0F)  // _STA: Status
    }

    Device (CPU3)
    {
        Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
        Name (_UID, 0x03)  // _UID: Unique ID
        Name (_STA, 0x0F)  // _STA: Status
    }
}