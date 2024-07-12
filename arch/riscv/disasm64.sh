#!/bin/sh

echo -ne "$1" > data.bin ; riscv64-unknown-linux-musl-objdump -D -b binary -m riscv:rv64 -M numeric,no-aliases
rm -f data.bin
