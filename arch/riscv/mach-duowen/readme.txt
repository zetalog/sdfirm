
                  Duowen Development Environment

===== DEBUGGING IMC =====
4. Compile SDFIRM as RAM TB:
   $ make duowen_ram_imc_defconfig
   $ make
   $ cp sdfirm ram.elf
5. Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-duowen/openocd-imc.cfg
   $ openocd -f openocd-imc.cfg
6. Startup GDB session:
   $ riscv64-unknown-elf-gdb
7. In GDB shell, type the following commands:
   (gdb) set remotetimeout unlimited
   (gdb) target extended-remote :3333
   (gdb) info threads
   (gdb) load ram.elf
   (gdb) flushregs
   (gdb) symbol-file ram.elf (for debugging symbols)
   (gdb) continue
===== DEBUGGING APC =====
9. Compile SDFIRM as RAM TB:
   $ make duowen_ram_apc_defconfig
   $ make
   $ cp sdfirm ram.elf
10.Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-duowen/openocd-apc.cfg
   $ openocd -f openocd-apc.cfg
11.Startup GDB session:
   $ riscv64-unknown-elf-gdb
12.Run program in UP mode (default):
   In GDB shell, type the following commands:
   (gdb) set remotetimeout unlimited
   (gdb) target remote :3333
   (gdb) info threads
   (gdb) thread 2 (switch to another core, can be 1-16)
   (gdb) load ram.elf
   (gdb) flushregs
   (gdb) symbol-file ram.elf (for debugging symbols)
   (gdb) continue
   NOTE: Sometimes, debugger switches to hart0, and hart1 still hangs.
         You may do the followings:
   (gdb) continue (let hart0 run)
   (gdb) thread 2
   (gdb) load ram.elf (reload firmware on hart1)
   (gdb) continue (let hart1 run)
13.Run program in SMP mode:
   In GDB shell, type the following commands to run on all cores:
   (gdb) set remotetimeout unlimited
   (gdb) target remote :3333
   (gdb) file ram.elf
   (gdb) flushregs
   (gdb) thread apply all load
   (gdb) set scheduler-locking off
   (gdb) thread 2
   (gdb) continue
   NOTE: Sometimes, debugger switches to hart0, and other harts hang.
         You may do the followings:
   (gdb) continue (let hart0 run)
   (gdb) thread 2
   (gdb) thread apply all load (reload firmware on harts)
   (gdb) continue (let hart1 run)
===== DEBUGGING commands =====
14.Hardware reset:
   (gdb) monitor reset halt (reset core)
15.Single step debugging:
   (gdb) set disassemble-next-line on (auto disassemble)
   (gdb) si/ni (for assembly debugging)
   (gdb) s/n (for C debugging)
16.Information:
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
17:Run until or jump:
   (gdb) set $pc = 0x8000000
   (gdb) x *0x80000a8 (run to a specific address)


                                                                  Lv Zheng
