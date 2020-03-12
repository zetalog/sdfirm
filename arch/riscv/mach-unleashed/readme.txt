
                  SiFive Unleashed Development Environment

1. JTAG/UART -> microUSB
   1st COM port: JTAG
   2nd COM port: UART
2. Power/Reset (power button, reset button)
3. SDFIRM:
   $ make unleashed_fsbl_defconfig
   $ make
4. Special step on Windows for openocd:
   Use USB Driver Tool to select libusb-windows on 1st COM port.

===== DEBUGGING E51 =====
5. Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-unleashed/openocd-e51.cfg
   $ openocd -f openocd.e51.cfg
6. Startup GDB session:
   $ riscv64-unknown-elf-gdb
7. In GDB shell, type the following commands:
   (gdb) target remote :3333
   (gdb) load sdfirm
   (gdb) symbol-file sdfirm (for debugging symbols)
   (gdb) continue
8. Single step debugging:
   (gdb) set disassemble-next-line on
   (gdb) si/ni (for assembly debugging)
   (gdb) s/n (for C debugging)
   (gdb) info register pc (dump register content)
   (gdb) examine /nfu <addr> (dump memory content)
         n: number of memory unit
         f: format - x hex
                     d decimal
                     u unsigned decimal
                     o octet
                     t binary
                     a address
                     i instruction
                     c character
                     f floating point
         u: length of memory unit
            b byte
            h half word
            w word
            g quad
   (gdb) set $pc = 0x8000000
   (gdb) until *0x80000a8 (run to a specific address)

===== DEBUGGING U54 =====
5. Special steps for running U54:
   Remove SD card from the slot to make sure no system is booted by U54,
   otherwise, GDB cannot reset core states.
6. Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-unleashed/openocd-u54.cfg
   $ openocd -f openocd.u54.cfg
7. Startup GDB session:
   $ riscv64-unknown-elf-gdb
8. In GDB shell, type the following commands to run on all cores:
   (gdb) target remote :3333
   (gdb) file sdfirm
   (gdb) monitor reset halt
   (gdb) flushregs
   (gdb) thread apply all load
   (gdb) set scheduler-locking off
   (gdb) continue
9. In GDB shell, type the following commands to run on a specific core:
   NOTE: CONFIG_UNLEASHED_HART_MASK should be configured to select only
         this core.
   (gdb) target remote :3333
   (gdb) info threads
   (gdb) thread 2 (switch to U54 boot core, can be 2, 3, 4, 5)
   (gdb) load sdfirm
   (gdb) symbol-file sdfirm (for debugging symbols)
   (gdb) continue

                                                                  Lv Zheng
