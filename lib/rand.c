/* ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *-------------------------------------------------------------------------
 * COPYRIGHT for MT19937 (PRNG)
 *-------------------------------------------------------------------------
 * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the
 *      distribution.
 *   3. The names of its contributors may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @(#)rand.c: MT19937 pseudo random number generator (PRNG) implementation
 * $Id: rand.c,v 1.1 2019-12-23 12:23:00 zhenglv Exp $
 */

#include <target/generic.h>
#include <target/cmdline.h>
#include <target/jiffies.h>

/* Period parameters */  
#define N		624
#define M		397
#define MATRIX_A	UL(0x9908b0df)	/* constant vector a */
#define UPPER_MASK	UL(0x80000000)	/* most significant w-r bits */
#define LOWER_MASK	UL(0x7fffffff)	/* least significant r bits */
#define ODD(x)		((x) & 1)
#define M32(x)		((x) & UPPER_MASK)
#define L31(x)		((x) & LOWER_MASK)

/* The array for the state vector  */
static uint32_t mt[N];
/* mti == N + 1 means mt[N] is not initialized */
static int mti = N + 1;

/* initializes mt[N] with a seed */
void init_genrand(uint32_t s)
{
	mt[0]= s & UL(0xffffffff);
	for (mti = 1; mti < N; mti++) {
		mt[mti] = (UL(1812433253) *
			   (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. In
		 * the previous versions, MSBs of the seed affect only MSBs
		 * of the array mt[].
		 * 2002/01/09 modified by Makoto Matsumoto
		 */
		mt[mti] &= UL(0xffffffff);
		/* for >32 bit machines */
	}
}

/* initialize by an array with array-length
 * init_key   - the array for initializing keys
 * key_length - its length
 * slight change for C++, 2004/2/26
 */
void init_by_array(uint32_t init_key[], int key_length)
{
	int i, j, k;

	init_genrand(UL(19650218));
	i = 1; j = 0;
	k = (N > key_length ? N : key_length);
	for (; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) *
				  UL(1664525))) +
			init_key[j] + j; /* non linear */
		/* for WORDSIZE > 32 machines */
		mt[i] &= UL(0xffffffff);
		i++; j++;
		if (i >= N) { mt[0] = mt[N-1]; i=1; }
		if (j >= key_length) j=0;
	}
	for (k = N-1; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) *
				  UL(1566083941))) - i; /* non linear */
		/* for WORDSIZE > 32 machines */
		mt[i] &= UL(0xffffffff);
		i++;
		if (i >= N) { mt[0] = mt[N-1]; i=1; }
	}
	/* MSB is 1; assuring non-zero initial array */ 
	mt[0] = UL(0x80000000);
}

/* generates a random number on [0,0xffffffff]-interval */
uint32_t genrand_int32(void)
{
	unsigned long y;
	static uint32_t mag01[2] = { UL(0x0), MATRIX_A };
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (mti >= N) {
		/* generate N words at one time */
		int kk;

		/* if init_genrand() has not been called, a default initial
		 * seed is used.
		 */
		if (mti == N+1)
			init_genrand(UL(5489));

		for (kk = 0; kk < N-M; kk++) {
			y = M32(mt[kk]) | L31(mt[kk+1]);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[ODD(y)];
		}
		for (; kk < N-1; kk++) {
			y = M32(mt[kk]) | L31(mt[kk+1]);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[ODD(y)];
		}
		y = M32(mt[N-1]) | L31(mt[0]);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[ODD(y)];
		mti = 0;
	}
  
	y = mt[mti++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & UL(0x9d2c5680);
	y ^= (y << 15) & UL(0xefc60000);
	y ^= (y >> 18);
	return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
int32_t genrand_int31(void)
{
	return (int32_t)(genrand_int32() >> 1);
}

int rand(void)
{
	/* To output an integer compatible with rand(), we have two
	 * options:
	 * 1. Mask off the highest (32nd) bit, or
	 * 2. shift right by one bit.
	 * Masking with 0x7FFFFFFF will be compatible with 64-bit MT[], so
	 * we'll just use that here.
	 */
	return (int)L31(genrand_int32());
}

/* LCG: linear congruential generator
 * X_(n+1) = = (a*X_n + c) (mod m)
 * OPTIMAL MULTIPLIERS FOR PSEUDO-RANDOM NUMBER GENERATION BY THE LINEAR
 * CONGRUENTIAL METHOD
 * ITSHAK BOROSH AND HARALD NIEDERREITER 
 * https://link.springer.com/article/10.1007/BF01937326
 */
void borosh_lcg(uint32_t value)
{
	int i;

	mt[0] = value;
	mti = 0;

	for (i = 1; i < N; ++i)
		mt[i] = 0x6c078965*(mt[i-1] ^ mt[i-1]>>30) + i;
}

void srand(unsigned seed)
{
	borosh_lcg(seed);
}

/* These real versions are due to Isaku Wada, 2002/01/09 added */
#if 0
int main(void)
{
	int i;
	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;

	init_by_array(init, length);
	printf("1000 outputs of genrand_int32()\n");
	for (i = 0; i < 1000; i++) {
		printf("%10lu ", genrand_int32());
		if (i%5==4) printf("\n");
	}
	return 0;
}
#endif

void get_random_bytes(uint8_t *buf, size_t bytes)
{
	int i;

	srand(tick_get_counter());

	for (i = 0; i < bytes; i++)
		buf[i] = (uint8_t)rand();
}
