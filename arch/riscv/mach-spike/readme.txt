
                  RV64 Spike Development Environment

===== 64-bit TEST BENCHES =====
1. Compile SDFIRM as SMP benchmark TB:
   $ make spike64_tb_defconfig
   $ make
   $ ./scripts/run-spike.sh -p4 sdfirm
2. Compile SDFIRM as UP multi-task TB:
   $ make spike64_task_defconfig
   $ make
   $ ./scripts/run-spike.sh -p1 sdfirm
3. Compile and run SDFIRM as SMP litmus TB:
   $ make spike64_litmus_defconfig
   $ make
   $ ./scripts/run-spike.sh -p4 sdfirm
3. Compile and run SDFIRM as SMP linux TB:
   Please refer to README.md
===== DEBUGGING (litmus as example) =====
4. Run SDFIRM with openocd enabled:
   $ ./scripts/run-spike.sh -j9824 -p4 sdfirm
5. Startup openocd session, the configuration file is:
   sdfirm/arch/riscv/mach-spike/openocd-spike64.cfg
   $ openocd -f ./arch/riscv/mach-spike/openocd-spike64.cfg
6. Startup GDB session:
   $ riscv64-unknown-elf-gdb
7. Run program in UP mode (default):
   In GDB shell, type the following commands:
   (gdb) target remote :3333
   (gdb) info threads
   (gdb) thread 2 (switch to another core, can be 1-16)
   (gdb) load sdfirm.strip
   (gdb) flushregs
   (gdb) symbol-file sdfirm (for debugging symbols)
   (gdb) continue
   NOTE: Sometimes, debugger switches to hart0, and hart1 still hangs.
         You may do the followings:
   (gdb) continue (let hart0 run)
   (gdb) thread 2
   (gdb) load sdfirm (reload firmware on hart1)
   (gdb) continue (let hart1 run)
8. Run program in SMP mode:
   In GDB shell, type the following commands to run on all cores:
   (gdb) target remote :3333
   (gdb) load sdfirm.strip
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
9. Hardware reset:
   (gdb) monitor reset halt (reset core)
10.Single step debugging:
   (gdb) set disassemble-next-line on (auto disassemble)
   (gdb) si/ni (for assembly debugging)
   (gdb) s/n (for C debugging)
11.Information:
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
12:Run until or jump:
   (gdb) set $pc = 0x8000000
   (gdb) x *0x80000a8 (run to a specific address)


                                                                  Lv Zheng
