all:
	$(CROSS_COMPILE)gcc -c -g -fno-builtin -nostdlib -fsigned-char -g -ffunction-sections -fdata-sections -march=rv32imac_zicsr_zifencei -mabi=ilp32 -mcmodel=medlow test.c
	$(CROSS_COMPILE)gcc -g -march=rv32imac_zicsr_zifencei -mabi=ilp32 -mcmodel=medlow -fno-builtin -nostdlib ./entry.S ./crt0.S test.o -Wl,--gc-sections -Wl,-Map,test.map -o test.elf -T./rom-20010000_ram-80000000.lds -Wl,--start-group -lc -lgcc -lm -Wl,--end-group

clean:
	rm -f *.o
	rm -f test.elf
	rm -f test.map
