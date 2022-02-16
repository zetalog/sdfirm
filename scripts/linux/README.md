SDFirm Linux+Busybox Image Builder
======================================

About
---------

This project is a combination of [eps-std](https://github.com/zetalog/eps-std) and [tiny-linux](https://github.com/IanJiangICT/tiny-linux).

All these projects are used to create a busybox+linux image. And this
project is specialized to only creates a RISC-V linux kernel image with
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

Then type the following command to complate all jobs:

spike image:

    $ MACH=spike64 ./sdfirm/scripts/linux/build_image.sh

qemu image:

    $ MACH=virt64 ./sdfirm/scripts/linux/build_image.sh

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
programs to sdfirm/scripts/linux/bench folder. It will be automatically
archived into the linux rootfs.

As an example, here is a way to build dhrystone/linpack/coremark:

    $ cd ./sdfirm/tests/bench
    $ make -f Makefile.target clean
    $ make -f Makefile.target
    $ cp ./dhrystone.elf ../../scripts/linux/bench/dhrystone
    $ cp ./linpack.elf ../../scripts/linux/bench/linpack
    $ cp ./coremark.elf ../../scripts/linux/bench/coremark
    $ cd -

To simplify the above process, you can also utilize the mechnisms provided
in the sdfirm/scripts/linux/build_image.sh by
1. putting your program source code into a sub-folder of sdfirm/tests,
2. generating similar Makefile.target for building the new program, and
3. modifying APPDIRS and APPELFS in sdfirm/scripts/linux/build_image.sh to
   include the new program.
