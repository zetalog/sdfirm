Small Device Firmware (sdfirm)
==================================

About
---------

The project sdfirm is abbrieviated for [Small Device Firmware](http://sdfirm.sourceforge.net). 

It was started to build a firmware for a MCS51 chip (AT89C5122). Compiled
with sdcc, it was named sdfirm. After successfully demonstrated its
possibility in running full stack (ISO7816 T0/T1 and USB) on such a
resource limited MCU (128 byte internal RAM as stack and 256 byte external
RAM as heap), sdfirm gained a chance to be ported to AVR chips (AT90USB1287
& AT90SCR100) with the gcc compiler supported. And then, along with the
boom of ARM Cortex-M, sdfirm was ported to ARM Cortex-M3 chips (LM3S9B92)
with EABI gcc compilation support. 

Versioning
--------------

Early usage models of sdfirm include "USB peripheral firmware", and smart
card standards.

Lately, it becomes a silicon validation platform for ARMv8 chips, it can
boot 48 QDF2400 CPUs in just miliseconds. Due to its ultra-fast/thin
design, it grows up to be an IC verification methodology, and takes part
into open source RISC-V IC revolution to be a full SMP CPU verification
program that can be run by the RTL CPUs in simulators (VCS, verilator).

Build steps of SPIKE test benches
-------------------------------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path and spike is already installed in the PATH environment.

Running bare metal test bench in spike:

    $ apt-get install device-tree-compiler
    $ export SUBARCH=riscv
    $ export RISCV64=1
    $ make spike64_tb_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-spike.sh -p4
    Listening for remote bitbang connection on port 9824.
    
    SmartCore - RISC-V ISA simulator (spike) Test Bench
    4.4.0-18362-Microsoft - 1.0.0.0
       _____ _____  ______ _____ _____  __  __
      / ____|  __ \|  ____|_   _|  __ \|  \/  |
     | (___ | |  | | |__    | | | |__) | \  / |
      \___ \| |  | |  __|   | | |  _  /| |\/| |
      ____) | |__| | |     _| |_| | \ \| |  | |
     |_____/|_____/|_|    |_____|_|  \_\_|  |_|
    
    
    OpenSBI v0.4 (Jul  6 2020 22:24:35)
       ____                    _____ ____ _____
      / __ \                  / ____|  _ \_   _|
     | |  | |_ __   ___ _ __ | (___ | |_) || |
     | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
     | |__| | |_) |  __/ | | |____) | |_) || |_
      \____/| .__/ \___|_| |_|_____/|____/_____|
            | |
            |_|
    
    Platform Name          : RISC-V ISA simulator (spike)
    Platform HART Features : RV64ACIMSU
    Firmware Base          : 0x80000000
    Firmware Size          : 2101348 KB
    Runtime SBI Version    : 0.1
    Platform Max HARTs     : 4
    Firmware Max CPUs      : 4
    Current Hart           : 0
    Current CPU            : 0
    Current Thread Pointer : 0x0000000080415e00
    Current Thread Stack   : 0x0000000080415000 - 0x0000000080416000
    PMP0: 0x0000000000000000-0x00000000ffffffff (A,R,W,X)
    PMP1: 0x0000000000000000-0x01ffffffffffffff (A,R,W,X)
    Current Hart           : 1
    Current CPU            : 1
    Current Thread Pointer : 0x0000000080416e00
    Current Thread Stack   : 0x0000000080416000 - 0x0000000080417000
    Current Hart           : 2
    Current CPU            : 2
    Current Thread Pointer : 0x0000000080417e00
    Current Thread Stack   : 0x0000000080417000 - 0x0000000080418000
    Current Hart           : 3
    Current CPU            : 3
    Current Thread Pointer : 0x0000000080418e00
    Current Thread Stack   : 0x0000000080418000 - 0x0000000080419000
    Early  MAP: 0000000080000000 - 0000000080419000
    LOWMAP: 0000000080000000 -> 0000000080000000: 0000000000401000
    LOWMAP: 0000000080401000 -> 0000000080401000: 0000000000003000
    LOWMAP: 0000000080404000 -> 0000000080404000: 000000000000d000
    LOWMAP: 0000000080411000 -> 0000000080411000: 0000000000004000
    LOWMAP: 0000000080419000 -> 0000000080419000: 000000007fbe7000
    SATP: 8000000000080408
    SATP: 800000000008040c
    reserved: 0000000080000000 - 0000000080418fff
    reserved: 00000000ffffe000 - 00000000ffffffff
    memory: 0000000080419000 - 00000000ffffe000
    SMP allocating PERCPU area 0000000080419000(1).
    CPU0 area: 0000000080419000
    CPU1 area: 0000000080419180
    CPU2 area: 0000000080419300
    CPU3 area: 0000000080419480
    SMP initializing CPU 0.
    SMP initializing CPU 1.
    SATP: 8000000000080408
    SATP: 800000000008040c
    SMP initializing CPU 2.
    SATP: 8000000000080408
    SATP: 800000000008040c
    SMP initializing CPU 3.
    SATP: 8000000000080408
    SATP: 800000000008040c
    sdfirm> Number of runs:                             5000
    User time (us):                             54850
    VAX MIPS rating:                                     518
    Number of runs:                             5000
    User time (us):                             54850
    VAX MIPS rating:                                     518
    Number of runs:                             5000
    User time (us):                             54850
    VAX MIPS rating:                                     518
    Number of runs:                             5000
    User time (us):                             54850
    VAX MIPS rating:                                     518
    
    
    sdfirm>

Running Linux test bench in spike:

    $ cd ..
    $ git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    $ git clone git://busybox.net/busybox.git
    $ git clone https://github.com/zetalog/tiny-linux
    $ cd tiny-linux
    $ LINUX_DIR=../linux BUSYBOX_DIR=../busybox SDFIRM_DIR=../sdfirm BBL=sdfirm MACH=spike64 ./script/build.sh
    $ BBL=sdfirm ./script/run-spike.sh

You can also do this in sdfirm folder:

    $ cp ../tiny-linux/obj/linux-riscv/arch/riscv/boot/Image ./Image
    $ make spike64_bbl_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-spike.sh –p4
    Listening for remote bitbang connection on port 9824.
    
    SmartCore - RISC-V isa simulator (spike) Berkeley Bootloader
    4.4.0-18362-Microsoft - 1.0.0.0
       _____ _____  ______ _____ _____  __  __
      / ____|  __ \|  ____|_   _|  __ \|  \/  |
     | (___ | |  | | |__    | | | |__) | \  / |
      \___ \| |  | |  __|   | | |  _  /| |\/| |
      ____) | |__| | |     _| |_| | \ \| |  | |
     |_____/|_____/|_|    |_____|_|  \_\_|  |_|
    
    
    OpenSBI v0.4 (Jul  6 2020 22:33:15)
       ____                    _____ ____ _____
      / __ \                  / ____|  _ \_   _|
     | |  | |_ __   ___ _ __ | (___ | |_) || |
     | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
     | |__| | |_) |  __/ | | |____) | |_) || |_
      \____/| .__/ \___|_| |_|_____/|____/_____|
            | |
            |_|
    
    Platform Name          : RISC-V ISA simulator (spike)
    Platform HART Features : RV64ACDFIMSU
    Firmware Base          : 0x80000000
    Firmware Size          : 2097280 KB
    Runtime SBI Version    : 0.1
    Platform Max HARTs     : 4
    Firmware Max CPUs      : 4
    Current Hart           : 0
    Current CPU            : 0
    Current Thread Pointer : 0x000000008001ce00
    Current Thread Stack   : 0x000000008001c000 - 0x000000008001d000
    PMP0: 0x0000000000000000-0x00000000ffffffff (A,R,W,X)
    PMP1: 0x0000000000000000-0x01ffffffffffffff (A,R,W,X)
    Current Hart           : 1
    Current CPU            : 1
    Current Thread Pointer : 0x000000008001de00
    Current Thread Stack   : 0x000000008001d000 - 0x000000008001e000
    Current Hart           : 2
    Current CPU            : 2
    Current Thread Pointer : 0x000000008001ee00
    Current Thread Stack   : 0x000000008001e000 - 0x000000008001f000
    Current Hart           : 3
    Current CPU            : 3
    Current Thread Pointer : 0x000000008001fe00
    Current Thread Stack   : 0x000000008001f000 - 0x0000000080020000
    [    0.000000] OF: fdt: Ignoring memory range 0x80000000 - 0x80200000
    [    0.000000] Linux version 5.8.0-rc1sdfirm+ (zetalog@SurfaceBook2) (riscv64-linux-gcc (GCC) 8.1.0, GNU ld (GNU Binutils) 2.30) #30 SMP Sat Jun 20 16:24:32 CST 2020
    [    0.000000] earlycon: sbi0 at I/O port 0x0 (options '')
    [    0.000000] printk: bootconsole [sbi0] enabled
    [    0.000000] initrd not found or empty - disabling initrd
    [    0.000000] Zone ranges:
    [    0.000000]   DMA32    [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000]   Normal   empty
    [    0.000000] Movable zone start for each node
    [    0.000000] Early memory node ranges
    [    0.000000]   node   0: [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000] Initmem setup node 0 [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000] software IO TLB: mapped [mem 0xfa3fb000-0xfe3fb000] (64MB)
    [    0.000000] SBI specification v0.1 detected
    [    0.000000] riscv: ISA extensions acdfim
    [    0.000000] riscv: ELF capabilities acdfim
    [    0.000000] percpu: Embedded 22 pages/cpu s60832 r0 d29280 u90112
    [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 516615
    [    0.000000] Kernel command line: console=hvc0 earlycon=sbi
    [    0.000000] Dentry cache hash table entries: 262144 (order: 9, 2097152 bytes, linear)
    [    0.000000] Inode-cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
    [    0.000000] Sorting __ex_table...
    [    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
    [    0.000000] Memory: 1988440K/2095104K available (1335K kernel code, 2969K rwdata, 2048K rodata, 567K init, 214K bss, 106664K reserved, 0K cma-reserved)
    [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
    [    0.000000] rcu: Hierarchical RCU implementation.
    [    0.000000] rcu:     RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
    [    0.000000]  Rude variant of Tasks RCU enabled.
    [    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
    [    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
    [    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
    [    0.000000] riscv-intc: 64 local interrupts mapped
    [    0.000000] riscv_timer_init_dt: Registering clocksource cpuid [0] hartid [0]
    [    0.000000] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x24e6a1710, max_idle_ns: 440795202120 ns
    [    0.000005] sched_clock: 64 bits at 10MHz, resolution 100ns, wraps every 4398046511100ns
    [    0.000505] printk: console [hvc0] enabled
    [    0.000505] printk: console [hvc0] enabled
    [    0.000990] printk: bootconsole [sbi0] disabled
    [    0.000990] printk: bootconsole [sbi0] disabled
    [    0.001535] Calibrating delay loop (skipped), value calculated using timer frequency.. 20.00 BogoMIPS (lpj=40000)
    [    0.002140] pid_max: default: 4096 minimum: 301
    [    0.002495] Mount-cache hash table entries: 4096 (order: 3, 32768 bytes, linear)
    [    0.002935] Mountpoint-cache hash table entries: 4096 (order: 3, 32768 bytes, linear)
    [    0.004030] rcu: Hierarchical SRCU implementation.
    [    0.004595] smp: Bringing up secondary CPUs ...
    [    0.006280] smp: Brought up 1 node, 4 CPUs
    [    0.006815] devtmpfs: initialized
    [    0.007510] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
    [    0.010260] clocksource: Switched to clocksource riscv_clocksource
    [    0.039775] workingset: timestamp_bits=62 max_order=19 bucket_order=0
    [    0.043275] Serial: 8250/16550 driver, 1 ports, IRQ sharing disabled
    [    0.044020] random: get_random_bytes called from init_oops_id+0x32/0x3a with crng_init=0
    [    0.044090] Warning: unable to open an initial console.
    [    0.044985] Freeing unused kernel memory: 564K
    [    0.062495] Run /init as init process
    
    Boot took 0.06 seconds
    
    
    
    BusyBox v1.32.0.git (2020-06-17 10:28:19 CST) built-in shell (ash)
    
    #

Build steps of QEMU test benches
------------------------------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path and qemu-system-riscv is already installed in the PATH
environment.

Running bare metal test bench in qemu:

    $ apt-get install device-tree-compiler
    $ export SUBARCH=riscv
    $ export RISCV64=1
    $ make virt64_tb_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-qemu.sh -p4

Running Linux test bench in qemu:

    $ cd ..
    $ git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    $ git clone git://busybox.net/busybox.git
    $ git clone https://github.com/zetalog/tiny-linux
    $ cd tiny-linux
    $ LINUX_DIR=../linux BUSYBOX_DIR=../busybox SDFIRM_DIR=../sdfirm BBL=sdfirm MACH=virt64 ./script/build.sh
    $ BBL=sdfirm ./script/run-qemu.sh

You can also do this in sdfirm folder:

    $ cp ../tiny-linux/obj/linux-riscv/arch/riscv/boot/Image ./Image
    $ make virt64_bbl_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-qemu.sh -p4

