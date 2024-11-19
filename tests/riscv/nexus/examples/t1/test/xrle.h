/*
 * Copyright (c) 2015, Konstantinos Agiannis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * in: buffer to compress
 * out: buffer to store compressed data
 * in_size: input buffer size in bytes
 *
 * return value: size of compressed data in bytes
 *
 * Input and output buffers must point to 8 byte aligned data.
 * Otherwise compression function ,depending on the platform,can be
 * slower or even crash.
 *
 * input buffer must be less than 32 GB
 */
size_t xrle_compress(void * out,const void * in,size_t in_size);

/*
 * in: buffer to decompress
 * out: buffer to store decompressed data
 * in_size: input buffer size in bytes
 *
 * return value: size of decompressed data in bytes
 *
 * Input and output buffers must point to 8 byte aligned data.
 * Otherwise decompression function ,depending on the platform,can be
 * slower or even crash.
 */
size_t xrle_decompress(void * out,const void * in,size_t in_size);

/* This macro returns the maximum size of compressed data in bytes */
#define xrle_max_out(A) ((A) + 8)
