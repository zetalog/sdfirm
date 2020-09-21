#!/bin/sh

./scripts/sync-litmus.sh -c 4 -t ../memory-model/litmus-tests-riscv/
./scripts/gen-litmus.sh -m spike64 -o ../litmus-spike64/
./scripts/sync-litmus.sh clean
