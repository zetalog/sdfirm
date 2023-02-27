#!/bin/sh

qemu-system-riscv64 \
	-D ./cpu.log -d cpu,exec,in_asm \
	-cpu sifive-u54 -machine sifive_u \
	-bios $1 \
	-m 2048 -nographic -smp cpus=5,maxcpus=5
