#!/bin/sh

sudo dd if=arch/arm64/boot/sdfirm.bin of=$1 bs=1024 seek=8
