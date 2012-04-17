#ifndef __GPT_LM3S3826_H_INCLUDE__
#define __GPT_LM3S3826_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>
#include <asm/mach/clk.h>
#include <asm/mach/pm.h>
#include <asm/mach/timer.h>
#include <asm/mach/tick.h>

#ifdef CONFIG_GPT_LM3S3826_16BITS
#define CLK_GPT		CLK_GPT16
#else
#define CLK_GPT		CLK_GPT32
#endif

#define GPT_MAX_TIMEOUT		(0x100000000/CLK_GPT)
#define GPT_COUNT_PER_MS	CLK_GPT

#endif /* __GPT_LM3S3826_H_INCLUDE__ */
