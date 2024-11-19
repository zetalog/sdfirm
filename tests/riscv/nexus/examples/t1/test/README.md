# xrle - eXtreme Run Length Encoding
xrle is a compression algorithm inspired by RLE64 and lrrle.
Compared to RLE64 and lrrle 64 bit variant,xrle is faster while having
better compression ratio.

## Benchmark
police.bmp

Compressor|Compressed size|Ratio|Encoding speed|Decoding speed
----------|---------------|-----|--------------|--------------
xrle      |3827870        |x40.059|7652 MB/s|5171 MB/s
RLE64     |5007102        |x30.624|2354 MB/s|2424 MB/s
lrrle,64  |5007102        |x30.624|4894 MB/s|5151 MB/s
lrrle,256 |12097294       |x12.676|9194 MB/s|5023 MB/s
memcpy    |153339294      |x 1.000|5333 MB/s|-

## Compile
    gcc -c -O3 xrle.c -o xrle.o
For some reason clang produces unoptimized executable.See
[here](https://llvm.org/bugs/show_bug.cgi?id=23202).

## License
BSD 2-Clause
