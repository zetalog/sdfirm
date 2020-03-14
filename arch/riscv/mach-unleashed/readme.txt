
                  SiFive Unleashed Development Environment

1. JTAG/UART -> microUSB
   1st COM port: JTAG
   2nd COM port: UART
2. Power/Reset (power button, reset button)
3. Special step on Windows for openocd:
   Use USB Driver Tool to select libusb-windows on 1st COM port.

===== DEBUGGING E51 =====
4. Compile SDFIRM as FSBL:
   $ make unleashed_fsbl_defconfig
   $ make
   $ cp sdfirm unleashed_fsbl
5. Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-unleashed/openocd-e51.cfg
   $ openocd -f openocd.e51.cfg
6. Startup GDB session:
   $ riscv64-unknown-elf-gdb
7. In GDB shell, type the following commands:
   (gdb) target remote :3333
   (gdb) load unleashed_fsbl
   (gdb) symbol-file unleashed_fsbl (for debugging symbols)
   (gdb) continue
   NOTE: If the image is a bootstrap fsbl, you can exit openocd gdb server
         after pc reaches to the waiting loop.
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
9. Compile SDFIRM as BBL:
   $ make unleashed_bbl_defconfig
   $ make
   $ cp sdfirm unleashed_bbl
10.Special steps for running U54:
   Remove SD card from the slot to make sure no system is booted by U54,
   otherwise, GDB cannot reset core states.
11.Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-unleashed/openocd-u54.cfg
   $ openocd -f openocd.u54.cfg
12.Startup GDB session:
   $ riscv64-unknown-elf-gdb
13.Run BBL in UP mode:
   In GDB shell, type the following commands to run on a specific core:
   NOTE: CONFIG_UNLEASHED_HART_MASK should be configured to select only
         this core.
   (gdb) target remote :3333
   (gdb) info threads
   (gdb) thread 2 (switch to U54 boot core, can be 2, 3, 4, 5)
   (gdb) load unleashed_bbl
   (gdb) symbol-file unleashed_bbl (for debugging symbols)
   (gdb) continue
   NOTE: sometimes, debugger switches to hart0, and hart1 still hangs.
         you may do the followings:
   (gdb) continue (let hart0 run)
   (gdb) thread 2
   (gdb) load unleashed_bbl (reload firmware on hart1)
   (gdb) continue (let hart1 run)
14.Run BBL in SMP mode:
   In GDB shell, type the following commands to run on all cores:
   (gdb) target remote :3333
   (gdb) file unleashed_bbl
   (gdb) monitor reset halt
   (gdb) flushregs
   (gdb) thread apply all load
   (gdb) set scheduler-locking off
   (gdb) continue

                                                                  Lv Zheng
