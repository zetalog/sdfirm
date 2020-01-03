
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
   (gdb) thread apply load
   (gdb) set scheduling-locking off
   (gdb) continue
9. In GDB shell, type the following commands to run on a specific core:
   (gdb) target remote :3333
   (gdb) info threads
   (gdb) thread 2 (switch to U54 boot core, can be 2, 3, 4, 5)
   (gdb) load sdfirm
   (gdb) symbol-file sdfirm (for debugging symbols)
   (gdb) continue

                                                                  Lv Zheng
