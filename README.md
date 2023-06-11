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

Prerequisites
-----------------

You need RISC-V system development environment prepared before going
further. For example, you need to install RISC-V toolchains (e.x.,
riscv64-linux-gnu-gcc which is used to build linux user space applications
or riscv64-linux-gcc which is used to build system software) in your $PATH
environment, and prepare Kbuild system required shell environments:

    $ export ARCH=riscv
    $ export CROSS_COMPILE=riscv64-linux-gnu-

Note that, when you only want to build sdfirm (e.x., to try sdfirm bare
metal test benches), the following environments are equivelent to setting
ARCH=riscv and CROSS_COMPILE=riscv64|32-linux- which is the default
nolib sysetm software compiler used by kernel.org:

  $ export SUBARCH=riscv : automatically sets ARCH=riscv
  $ export RISCV64=1     : automatically sets CROSS_COMPILE=riscv64-linux-
  $ export RISCV64=      : automatically sets CROSS_COMPILE=riscv32-linux-

Note that due to the development progress of the GNU toolchain and RISC-V
community, there is a support variation between GCC versions around the C
builtin functions. Thus it is not guaranteed that the following DEMOs can
work with all GNU toolchain versions. By testing, it is working with
upstream GCC 8.3.0 and 10.1.0.

Build steps of SPIKE test benches
-------------------------------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path and spike is already installed in the PATH environment.

Running bare metal test bench in spike:

    $ apt-get install device-tree-compiler
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

Modifying sdfirm/scripts/linux/build_spike64_linux.sh to configure
linux/busybox/sdfirm features, then:

    $ ./sdfirm/scripts/linux/build_spike64_linux.sh
    $ ./sdfirm/scripts/run-spike.sh -p4 obj/sdfirm-riscv/sdfirm

You can also do this in sdfirm folder if Linux image is prepared:

    $ cp ../obj/linux-riscv/arch/riscv/boot/Image ./Image
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
    [    0.377985] devtmpfs: mounted
    [    0.380520] Freeing unused kernel image (initmem) memory: 8832K
    [    0.392810] Run /sbin/init as init process
    starting pid 37, tty '': '/etc/init.d/rcS'
    mount: mounting none on /dev failed: Device or resource busy
    ===== Factory Address =====
    192.168.10.1 255.255.255.0
    ===========================
    starting pid 65, tty '': '/sbin/getty 115200 console vt100'
    
    sdfirm login:

Using root/sdfirm as username/password to login to the console:

      _      _
     | |    (_)
     | |     _ _ __  _   ___  __
     | |    | | '_ \| | | \ \/ /
     | |____| | | | | |_| |>  <
     |______|_|_| |_|\__,_/_/\_\
    login[65]: root login on 'console'
    
    
    BusyBox v1.33.0.git (2022-12-13 15:05:43 CST) built-in shell (ash)
    
    root@sdfirm:~#

Build steps of QEMU test benches
------------------------------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path and qemu-system-riscv is already installed in the PATH
environment.

Running bare metal test bench in qemu:

    $ apt-get install device-tree-compiler
    $ make virt64_tb_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-qemu.sh -p4

Running Linux test bench in qemu:

    $ cd ..
    $ git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    $ git clone git://busybox.net/busybox.git

Modifying sdfirm/scripts/linux/build_virt64_linux.sh to configure
linux/busybox/sdfirm features, then:

    $ ./sdfirm/scripts/linux/build_virt64_linux.sh
    $ ./sdfirm/scripts/run-qemu.sh -p4 obj/sdfirm-riscv/sdfirm

You can also do this in sdfirm folder if Linux image is prepared:

    $ cp ../obj/linux-riscv/arch/riscv/boot/Image ./Image
    $ make virt64_bbl_defconfig
    $ make menuconfig
    $ make clean
    $ make
    $ ./scripts/run-qemu.sh -p4

Note that QEMU contains many bugs, e.x., fails litmus memory consistency
model tests, SSTC not supported but reported, write delegate CSR returning
wrong read value, etc., so runing Linux kernel 5.8.0 with QEMU is
suggested unless those issues are known to be fixed in another QEMU
upstream version.

Build steps of SPIKE baremetal litmus benches
-------------------------------------------------

An interesting functionality is sdfirm can be used to generate bare metal
direct cases for being used in the IC development environments (VCS
benches and etc.). RISC-V memory order tests (litmus cases) are one of
those simulation case generators.

We can try an example via the following commands:

    $ make spike64_litmus_defconfig
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

    SMP allocating PERCPU area 0000000080119000(1).
    CPU0 area: 0000000080119000
    CPU1 area: 0000000080119100
    CPU2 area: 0000000080119200
    CPU3 area: 0000000080119300
    SMP initializing CPU 0.
    SMP initializing CPU 1.
    SMP initializing CPU 2.
    SMP initializing CPU 3.
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Results for dummy.litmus %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    RISCV dummy
    "PodWW Rfe PodRR Fre"
    {0:x5=1; 0:x6=x; 0:x7=y; 1:x6=y; 1:x8=x;}
     P0          | P1          ;
     sw x5,0(x6) | lw x5,0(x6) ;
     sw x5,0(x7) | lw x7,0(x8) ;
    
    exists (1:x5=1 /\ 1:x7=0)
    Generated assembler
    Test dummy Allowed
    Histogram (2 states)
    2     :>1:x5=0; 1:x7=0;
    2     :>1:x5=1; 1:x7=1;
    No
    
    Witnesses
    Positive: 0, Negative: 4
    Condition exists (1:x5=1 /\ 1:x7=0) is NOT validated
    Hash=2939da84098a543efdbb91e30585ab71
    Cycle=Rfe PodRR Fre PodWW
    Relax dummy No
    Safe=Rfe Fre PodWW PodRR
    Generator=diy7 (version 7.51+4(dev))
    Com=Rf Fr
    Orig=PodWW Rfe PodRR Fre
    Observation dummy Never 0 4
    Time dummy 0.00
    Test success.

The dummy case is automatically generated from MP case of the RISCV litmus
cases:

    $ git clone https://github.com/litmus-tests/litmus-tests-riscv

You can build and run an arbitrary test cases using scripts provided by
sdfirm:

    $ ./scripts/litmus.sh -g riscv64-unknown-linux-gnu- -e -p ./litmus-tests-riscv ISA03_2B_SIMPLE

And you can build and run all litmus tests using the following command:

    $ ./scripts/litmus.sh -g riscv64-unknown-linux-gnu- -e -p ./litmus-tests-riscv

NOTE: litmus-tests-riscv must be generated by a proprietary version of
      herdtools which is facilitated with "-mode sdfirm" support.

    $ git clone https://github.com/zetalog/herdtools
    $ git checkout sdfirm-next

NOTE: litmus.sh accepts "-m mach" option to allow you to generate cases
      for a specific SoC whose default configuration has already been
      prepared as arch/riscv/configs/mach_litmus_defconfig. And you can
      tune the default configuration to allow the litmus cases to run in
      a complicated environment (e.x., with MMU/IRQ enabled).

Build steps of SPIKE linux litmus benches
---------------------------------------------

Another choice to run litmus tests is to run sdfirm as BBL and to boot a
Linux kernel to run the litmus tests.

Please refer to "Build steps of SPIKE test benches" chapter and run linux
section. The only thing you should do is to modify the
build_spike64_linux.sh with the following configurations enabled:

    LITMUS_CORES=4
    LITMUS_UPDATE=yes
    LITMUS_DUMP=no
    LITMUS_ROOT=<path to litmus-tests-riscv>
    TEST_EARLY=litmus

Then running the tests:

    $ ./sdfirm/scripts/run-spike.sh -p4 obj/sdfirm-riscv/sdfirm -u litmus.log

The litmus tests will be automatically executed after booting Linux kernel
to the userspace.  The litmus.log files will contain litmus test result
and can be used by mcompare7 to tell the test results.

    $ mcompare7 -nohash litmus.log ${LITMUS_ROOT}/model-results/herd.logs

Build steps of SPIKE linux SPEC CPU2006 benches
-----------------------------------------------

Sdfirm can be used to create a cross compiled SPEC CPU2006 test bench.

You must execute the following command to build CPU2006 host tools before
generating target tools/tests:

    $ ./sdfirm/scripts/linux/build_cpu2006.sh -h

Please make sure /opt/cpu2006 is writeable for the current user:

    $ mkdir -p /opt/cpu2006
    $ sudo chown $USER /opt

Please refer to "Build steps of SPIKE test benches" chapter and run linux
section. The only thing you should do is to modify the
build_spike64_linux.sh with the following configurations enabled:

    CROSS_COMPILE=riscv64-unknown-linux-gnu-
    BUILD_TINY=no
    BUILD_LIB=yes
    CPU2006_UPDATE=yes
    CPU2006_REPORT=specinvoke
    CPU2006_BENCHMARKS="401.bzip2"
    TEST_EARLY=cpu2006

NOTE: spectools (autoconf) are very strict to the cross compile tuples
format, so please check if the detection is successful.

Then running the tests:

    $ ./sdfirm/scripts/run-spike.sh -p4 obj/sdfirm-riscv/sdfirm -u litmus.log

The non-reportable tests executes bare commands generated by
"specinvoke -nn":

    SmartCore - RISC-V isa simulator (spike) Berkeley Bootloader
    5.15.90.1-microsoft-standard-WSL2 - 1.0.0.0
       _____ _____  ______ _____ _____  __  __
      / ____|  __ \|  ____|_   _|  __ \|  \/  |
     | (___ | |  | | |__    | | | |__) | \  / |
      \___ \| |  | |  __|   | | |  _  /| |\/| |
      ____) | |__| | |     _| |_| | \ \| |  | |
     |_____/|_____/|_|    |_____|_|  \_\_|  |_|


    OpenSBI v0.4 (Jun  7 2023 14:11:31)
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
    Firmware Size          : 136 KB
    Runtime SBI Version    : 0.1
    Platform Max HARTs     : 4
    Firmware Max CPUs      : 4
    Current Hart           : 0
    Current CPU            : 0
    Current Thread Pointer : 0x000000008001ee00
    Current Thread Stack   : 0x000000008001e000 - 0x000000008001f000
    PMP0: 0x0000000000000000-0xffffffffffffffff (A,R,W,X)
    PMP1: 0x0000000000000000-0xffffffffffffffff (A,R,W,X)
    Current Hart           : 1
    Current CPU            : 1
    Current Thread Pointer : 0x000000008001fe00
    Current Thread Stack   : 0x000000008001f000 - 0x0000000080020000
    Current Hart           : 2
    Current CPU            : 2
    Current Thread Pointer : 0x0000000080020e00
    Current Thread Stack   : 0x0000000080020000 - 0x0000000080021000
    Current Hart           : 3
    Cur[    0.000000] Linux version 5.13.0-sdfirm+ (zetalog@SurrfaceBook2-B) (riscv64-linux-gnu-gcc (GCC) 10.1.0, GNU ld (GNU Bineutils) 2.35) #9 SMP Wed Jun 7 14:11:19 CST 2023
    nt CPU            : 3
    Curre[    0.000000] OF: fdt: Ignoring memory range 0x8000000n0 - 0x80200000
    t Thread[    0.000000] Machine model: ucbbar,spike-bare
     Pointer : 0x0000000080021e00[    0.000000] earlycon: ns16550a0 at MMIO 0x0000000010
    000000 (options '')
    Current Thread Stack   : 0x0[    0.000000] printk: bootconsole [ns16550a0] enable0
    00000080021000 - 0x0000000080022000
    [    0.000000] Zone ranges:
    [    0.000000]   DMA32    [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000]   Normal   empty
    [    0.000000] Movable zone start for each node
    [    0.000000] Early memory node ranges
    [    0.000000]   node   0: [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000] Initmem setup node 0 [mem 0x0000000080200000-0x00000000ffffffff]
    [    0.000000] SBI specification v0.1 detected
    [    0.000000] software IO TLB: mapped [mem 0x00000000fa3f9000-0x00000000fe3f9000] (64MB)
    [    0.000000] riscv: ISA extensions acdfhimnprt
    [    0.000000] riscv: ELF capabilities acdfim
    [    0.000000] percpu: Embedded 13 pages/cpu s24224 r0 d29024 u53248
    [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 516615
    [    0.000000] Kernel command line: console=ttyS0 earlycon
    [    0.000000] Dentry cache hash table entries: 262144 (order: 9, 2097152 bytes, linear)
    [    0.000000] Inode-cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
    [    0.000000] Sorting __ex_table...
    [    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
    [    0.000000] Memory: 1953800K/2095104K available (1237K kernel code, 4234K rwdata, 2048K rodata, 32939K init, 144K bss, 141304K reserved, 0K cma-reserved)
    [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
    [    0.000000] rcu: Hierarchical RCU implementation.
    [    0.000000] rcu:     RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
    [    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
    [    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
    [    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
    [    0.000000] riscv-intc: 64 local interrupts mapped
    [    0.000000] plic: plic@c000000: mapped 31 interrupts with 4 handlers for 8 contexts.
    [    0.000000] random: get_random_bytes called from 0xffffffff80200826 with crng_init=0
    [    0.000000] riscv_timer_init_dt: Registering clocksource cpuid [0] hartid [0]
    [    0.000000] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x24e6a1710, max_idle_ns: 440795202120 ns
    [    0.000000] sched_clock: 64 bits at 10MHz, resolution 100ns, wraps every 4398046511100ns
    [    0.000235] Calibrating delay loop (skipped), value calculated using timer frequency.. 20.00 BogoMIPS (lpj=40000)
    [    0.000530] pid_max: default: 4096 minimum: 301
    [    0.000750] Mount-cache hash table entries: 4096 (order: 3, 32768 bytes, linear)
    [    0.000935] Mountpoint-cache hash table entries: 4096 (order: 3, 32768 bytes, linear)
    [    0.001410] ASID allocator using 16 bits (65536 entries)
    [    0.001550] rcu: Hierarchical SRCU implementation.
    [    0.001790] smp: Bringing up secondary CPUs ...
    [    0.002470] smp: Brought up 1 node, 4 CPUs
    [    0.002770] devtmpfs: initialized
    [    0.003205] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
    [    0.003565] futex hash table entries: 16 (order: -2, 1024 bytes, linear)
    [    0.005445] clocksource: Switched to clocksource riscv_clocksource
    [    0.009570] workingset: timestamp_bits=62 max_order=19 bucket_order=0
    [    0.041840] Serial: 8250/16550 driver, 1 ports, IRQ sharing disabled
    [    0.042300] printk: console [ttyS0] disabled
    [    0.042390] 10000000.ns16550: ttyS0 at MMIO 0x10000000 (irq = 1, base_baud = 625000) is a 16550A
    [    0.042615] printk: console [ttyS0] enabled
    [    0.042615] printk: console [ttyS0] enabled
    [    0.042730] printk: bootconsole [ns16550a0] disabled
    [    0.042730] printk: bootconsole [ns16550a0] disabled
    [    1.600915] devtmpfs: mounted
    [    1.609250] Freeing unused kernel image (initmem) memory: 32936K
    [    1.621515] Run /sbin/init as init process
    starting pid 39, tty '': '/etc/init.d/rcS'
    mount: mounting none on /dev failed: Device or resource busy
    ===== Factory Address =====
    192.168.10.1 255.255.255.0
    ===========================
    Running 401.bzip2...
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/chicken.jpg.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/chicken.jpg.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/chicken.jpg.out begin
    spec_init
    Loading Input Data
    Duplicating 652307 bytes
    Duplicating 1304614 bytes
    Duplicating 2609228 bytes
    Duplicating 5218456 bytes
    Duplicating 10436912 bytes
    Duplicating 10583456 bytes
    Input data 31457280 bytes in length
    Compressing Input Data, level 5
    Compressed data 31306985 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 30427899 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 25949409 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Tested 30MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/chicken.jpg.out end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.combined.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.combined.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.combined.out begin
    spec_init
    Loading Input Data
    Duplicating 53779568 bytes
    Duplicating 102156064 bytes
    Input data 209715200 bytes in length
    Compressing Input Data, level 5
    Compressed data 44861975 bytes in length
    Uncompressing Data
    Uncompressed data 209715200 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 43924309 bytes in length
    Uncompressing Data
    Uncompressed data 209715200 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 43210534 bytes in length
    Uncompressing Data
    Uncompressed data 209715200 bytes in length
    Uncompressed data compared correctly
    Tested 200MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.combined.out end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.program.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.program.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.program.out begin
    spec_init
    Loading Input Data
    Duplicating 8070616 bytes
    Duplicating 16141232 bytes
    Duplicating 32282464 bytes
    Duplicating 64564928 bytes
    Duplicating 129129856 bytes
    Duplicating 35341568 bytes
    Input data 293601280 bytes in length
    Compressing Input Data, level 5
    Compressed data 100396862 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 101953195 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 103010861 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Tested 280MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.program.out end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.source.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.source.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.source.out begin
    spec_init
    Loading Input Data
    Duplicating 51763200 bytes
    Duplicating 103526400 bytes
    Duplicating 86548480 bytes
    Input data 293601280 bytes in length
    Compressing Input Data, level 5
    Compressed data 54403299 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 53599136 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 52960821 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Tested 280MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/input.source.out end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/liberty.jpg.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/liberty.jpg.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/liberty.jpg.out begin
    spec_init
    Loading Input Data
    Duplicating 289631 bytes
    Duplicating 579262 bytes
    Duplicating 1158524 bytes
    Duplicating 2317048 bytes
    Duplicating 4634096 bytes
    Duplicating 9268192 bytes
    Duplicating 12920896 bytes
    Input data 31457280 bytes in length
    Compressing Input Data, level 5
    Compressed data 21917976 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 17697398 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 13755071 bytes in length
    Uncompressing Data
    Uncompressed data 31457280 bytes in length
    Uncompressed data compared correctly
    Tested 30MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/liberty.jpg.out end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/text.html.err begin
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/text.html.err end
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/text.html.out begin
    spec_init
    Loading Input Data
    Duplicating 132492 bytes
    Duplicating 264984 bytes
    Duplicating 529968 bytes
    Duplicating 1059936 bytes
    Duplicating 2119872 bytes
    Duplicating 4239744 bytes
    Duplicating 8479488 bytes
    Duplicating 16958976 bytes
    Duplicating 33917952 bytes
    Duplicating 67835904 bytes
    Duplicating 135671808 bytes
    Duplicating 22257664 bytes
    Input data 293601280 bytes in length
    Compressing Input Data, level 5
    Compressed data 22290700 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 7
    Compressed data 17811990 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Compressing Input Data, level 9
    Compressed data 14427351 bytes in length
    Uncompressing Data
    Uncompressed data 293601280 bytes in length
    Uncompressed data compared correctly
    Tested 280MB buffer: OK!
    cpu2006: /opt/cpu2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_riscv.0000/text.html.out end
    starting pid 312, tty '': '/sbin/getty 115200 console vt100'

    sdfirm login:

The cpu2006 tests will be automatically executed after booting Linux kernel
to the userspace.

To run benchmark using report generation specinvoke command, please use the
following configurations:

    CPU2006_REPORT=specinvoke

To run benchmark using report generation runspec command, please use the
following configurations:

    CPU2006_REPORT=runspec
    CPU2006_DATA=all

The following configurations generate all run base folders:

    CPU2006_BENCHMARKS=
