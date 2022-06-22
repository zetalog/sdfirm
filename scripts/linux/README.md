SDFirm Linux+Busybox Image Builder
======================================

About
---------

This project is a combination of [eps-std](https://github.com/zetalog/eps-std) and [tiny-linux](https://github.com/IanJiangICT/tiny-linux).

All these projects are used to create a busybox+linux image. And this
project is specialized to only create a RISC-V linux kernel image with
busybox as its basic userspace program. And creates a bootable BBL image
using sdfirm.

Build steps of creating sdfirm BBL image
--------------------------------------------

In a working directory (say, workspace), do the following to prepare the
environments:

    $ cd workspace
    $ git clone https://github.com/zetalog/sdfirm
    $ git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    $ git clone git://busybox.net/busybox.git

Then type the following command to complete all jobs:

spike image:

    $ MACH=spike64 ./sdfirm/scripts/linux/build_image.sh

qemu image:

    $ MACH=virt64 ./sdfirm/scripts/linux/build_image.sh

Note that the current working directory is used as build top ($TOP). By
default, build_module.sh which is invoked by build_image.sh finds linux,
busybox and sdfirm right below the $TOP folder unless LINUX_DIR,
BUSYBOX_DIR and SDFIRM_DIR is specified via shell environments.

Run steps
-------------

If you've created a spike64 or virt64 image and you have spike or qemu
installed on your machine, do the following to run the image:

spike image:

    $ ./sdfirm/scripts/run-spike.sh -p4 ./obj/sdfirm-riscv/sdfirm

qemu image:

    $ ./sdfirm/scripts/run-qemu.sh -p4 ./obj/sdfirm-riscv/sdfirm

Add userspace programs
--------------------------

If you want to add userspace programs, you need to put your pre-built
programs to ${TOP}/obj/bench folder or cross compile a software by
specifying the prefix as ${TOP}/obj/bench. This folder (${TOP}/obj/bench)
will be automatically archived into the linux rootfs by the
build_module.sh which is invoked by build_image.sh.

    $ export PREFIX=`pwd`/obj/bench
    $ cd <some software>
    $ ./configure --host=riscv64-linux-gnu --prefix=${PREFIX}
    $ make
    $ make install
    $ cd -
    $ MACH=spike64 ./sdfirm/scripts/linux/build_image.sh

You can also add simple programs into the test bench rootfs
(${TOP}/obj/bench), As an example, here is a way to build sdfirm
integrated dhrystone/linpack/coremarki into the linux rootfs:

    $ export PREFIX=`pwd`/obj/bench
    $ mkdir -p ${PREFIx}/usr/local/bin
    $ cd ./sdfirm/tests/bench
    $ make -f Makefile.target clean
    $ make -f Makefile.target
    $ cp ./dhrystone.elf ${PREFIX}/usr/local/bin/dhrystone
    $ cp ./linpack.elf ${PREFIX}/usr/local/bin/linpack
    $ cp ./coremark.elf ${PREFIX}/usr/local/bin/coremark
    $ cd -
    $ MACH=spike64 ./sdfirm/scripts/linux/build_image.sh

To automate the small program integration process, you can also utilize
the mechanisms provided in the sdfirm/scripts/linux/build_image.sh by:
1. putting your program source code into a sub-folder of sdfirm/tests,
2. generating similar Makefile.target for building the new program, and
3. modifying APPDIRS and APPELFS in sdfirm/scripts/linux/build_image.sh to
   include the new program.

Then everything can be automatically generated via the following command:

    $ MACH=spike64 ./sdfirm/scripts/linux/build_image.sh -u
