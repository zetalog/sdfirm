all:
	mkdir -p ./output
	../../NexRv.exe -conv -objd test-OBJD.txt -pcinfo ./output/test-PCINFO.txt
	../../NexRv.exe -conv -pcinfo ./output/test-PCINFO.txt -pconly test-PCONLY.txt -pcseq ./output/test-PCSEQ.txt
	echo No Branch History Message - only Direct Branch Message
	../../NexRv.exe -enco ./output/test-PCSEQ.txt -nex ./output/test-NEX.bin -nobhm
	../../NexRv.exe -dump ./output/test-NEX.bin ./output/test-DUMP.txt
	../../NexRv.exe -deco ./output/test-NEX.bin -pcinfo ./output/test-PCINFO.txt -pcout ./output/test-PCOUT.txt
	diff -s -q ./test-PCONLY.txt ./output/test-PCOUT.txt
	echo  No Repeat Branch Message ...
	../../NexRv.exe -enco ./output/test-PCSEQ.txt -nex ./output/test-NEX.bin -norbm
	../../NexRv.exe -dump ./output/test-NEX.bin ./output/test-DUMP.txt
	../../NexRv.exe -deco ./output/test-NEX.bin -pcinfo ./output/test-PCINFO.txt -pcout ./output/test-PCOUT.txt
	diff -s -q ./test-PCONLY.txt ./output/test-PCOUT.txt
	echo  With Repeat Branch Message - default
	../../NexRv.exe -enco ./output/test-PCSEQ.txt -nex ./output/test-NEX.bin
	../../NexRv.exe -dump ./output/test-NEX.bin ./output/test-DUMP.txt
	../../NexRv.exe -deco ./output/test-NEX.bin -pcinfo ./output/test-PCINFO.txt -pcout ./output/test-PCOUT.txt
	diff -s -q ./test-PCONLY.txt ./output/test-PCOUT.txt

ELF:
	(cd test; make; cp test.elf ../)
	
OBJD:
	riscv64-unknown-elf-objdump -d test.elf >test-OBJD.txt 

clean:
	(cd test; make clean)
	rm -rf ./output
	rm -f ./test-OBJD.txt
	rm -f ./test.elf
