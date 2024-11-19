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
 *      Format:
 *           32 bits	      32 bits                    64 bits
 *      +----------------+---------------+----------+----------------+
 *      | literal length | repeat length | literals | word to repeat |
 *      +----------------+---------------+----------+----------------+
 *
 */

#include <stdint.h>
#include <string.h>

#define U64 uint64_t
#define U32 uint32_t

size_t xrle_compress(void * out,const void * in,size_t in_size)
{
	U64 *in_pos = (U64 *) in,*out_pos = (U64 *) out;
	U32 tail = in_size & 7,*descr,repeat;
	U64 *in_limit = (U64 *)((char *)in + in_size - tail),previous,next;

	if(in_size < 16){
		memcpy(out,in,in_size);
		return in_size;
	}
	for(;;){
		repeat = 1;
		descr = (U32 *)out_pos;
		out_pos++;
		next = *in_pos;

		do{
			in_pos++;
			previous = next;
			if(in_pos == in_limit)
				goto end;
			next = *in_pos;
			*out_pos++ = previous;
		}while(previous != next);

		do{
			in_pos++;
			repeat++;
			if(in_pos >= in_limit)
				goto end_all;

		}while(previous == *in_pos);

		/* encode */
		descr[0] = out_pos - ((U64 *)descr) - 2;
		descr[1] = repeat;
	}
end:
	*out_pos++ = previous;
end_all:
	descr[0]= out_pos - ((U64 *)descr) - 2;
	descr[1] = repeat;
	memcpy(out_pos,(char *)in + in_size - tail,tail);
	return (char *)out_pos - (char *)out + tail;
}

size_t xrle_decompress(void * out,const void * in,size_t in_size)
{

	U64 *in_pos = (U64 *) in,*out_pos = (U64 *) out,word,tmp;
	U32 tail = in_size & 7,lit_len,repeat,i;
	U64 *in_limit = (U64 *)((char *)in + in_size - tail);

	if(in_size < 16){
		memcpy(out,in,in_size);
		return in_size;
	}
	while(in_pos < in_limit){
		lit_len = ((U32 *) in_pos)[0];
		repeat = ((U32 *) in_pos)[1];
		in_pos++;

		for(i = 0;i < lit_len;i++)
			out_pos[i] = in_pos[i];

		in_pos += lit_len;
		out_pos += lit_len;

		word = *in_pos++;

		for(i = 0;i < repeat;i++)
			out_pos[i] = word;
		out_pos += repeat;
	}
	memcpy(out_pos,(char *)in + in_size - tail,tail);
	return (char *)out_pos - (char *)out + tail;
}
