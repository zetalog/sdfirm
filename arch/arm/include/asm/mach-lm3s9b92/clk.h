#ifndef __CLK_LM3S9B92_H_INCLUDE__
#define __CLK_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/cpu.h>

#define CLK_PIOSC	16000
#define CLK_PIOSC_DIV4	4000

#ifdef CONFIG_CPU_LM3S9B92_MOSC_16M
#define CLK_MOSC	16000
#define CLK_XTAL	0x00000540 /* External crystal is 16 MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_12M
#define CLK_MOSC	12000
#define CLK_XTAL	0x00000440 /* External crystal is 12 MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_10M
#define CLK_MOSC	10000
#define CLK_XTAL	0x00000400 /* External crystal is 10 MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_8M
#define CLK_MOSC	8000
#define CLK_XTAL	0x00000380 /* External crystal is 8MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_6M
#define CLK_MOSC	6000
#define CLK_XTAL	0x000002C0 /* External crystal is 6MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_5M
#define CLK_MOSC	5000
#define CLK_XTAL	0x00000240 /* External crystal is 5MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_4M
#define CLK_MOSC	4000
#define CLK_XTAL	0x00000180 /* External crystal is 4MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_2M
#define CLK_MOSC	2000
#define CLK_XTAL	0x00000080 /* External crystal is 2MHz */
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC_1M
#define CLK_MOSC	1000
#define CLK_XTAL	0x00000000 /* External crystal is 1MHz */
#endif

#ifndef CLK_MOSC
#define CLK_MOSC	16000
#endif

#define CLK_IOSC	30

#define CLK_PLL		400000
#define CLK_USB		60000

#ifdef CONFIG_CPU_LM3S9B92_PLL
#ifdef CONFIG_CPU_LM3S9B92_PLL_100M
#define CLK_SYS		100000
#define CLK_CONFIG	(CLK_DIV_2 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#ifdef CONFIG_CPU_LM3S9B92_PLL_80M
#define CLK_SYS		80000
#define CLK_CONFIG	(CLK_DIV_2_5 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#ifdef CONFIG_CPU_LM3S9B92_PLL_50M
#define CLK_SYS		50000
#define CLK_CONFIG	(CLK_DIV_4 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#ifdef CONFIG_CPU_LM3S9B92_PLL_40M
#define CLK_SYS		40000
#define CLK_CONFIG	(CLK_DIV_5 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#ifdef CONFIG_CPU_LM3S9B92_PLL_25M
#define CLK_SYS		25000
#define CLK_CONFIG	(CLK_DIV_8 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#ifdef CONFIG_CPU_LM3S9B92_PLL_20M
#define CLK_SYS		20000
#define CLK_CONFIG	(CLK_DIV_10 | CLK_SRC_PLL | CLK_OSC_MOSC | CLK_XTAL)
#endif
#endif
#ifdef CONFIG_CPU_LM3S9B92_MOSC
#define CLK_SYS		CLK_MOSC
#define CLK_CONFIG	(CLK_DIV_1 | CLK_SRC_OSC | CLK_OSC_MOSC | CLK_XTAL)
#endif

#define CLK_CPU		CLK_SYS

#define RIS			SYSTEM(0x050)
#define IMC			SYSTEM(0x054)
#define MISC			SYSTEM(0x058)
#define MOSCPUP			8
#define USBPLLL			7
#define PLLL			6
#define BOR			1

#define RCC			SYSTEM(0x060)
#define ACG			27
#define SYSDIV			23 /* SYSDIV offset */
#define USESYSDIV		22
#define USEPWMDIV		20
#define PWRDN			13
#define OEN			12 /* PLL output enable, not documented */
#define BYPASS			11
#define OSCSRC			4
#define IOSCDIS			2
#define MOSCDIS			1
#define SYSDIV_MASK		0x07800000
#define PWMDIV_MASK		0x000E0000
#define XTAL_MASK		0x000007C0
#define OSCSRC_MASK		0x00000030
#define OSCSRC_MOSC		0x00000000 /* MOSC */
#define OSCSRC_PIOSC		0x00000010 /* PIOSC */
#define OSCSRC_PIOSC_DIV4	0x00000020 /* PIOSC/4 */
#define OSCSRC_IOSC_30K		0x00000030 /* 30 KHz IOSC */

#define PLLCFG			SYSTEM(0x064)
#define RCC2			SYSTEM(0x070)
#define USERCC2			31
#define DIV400			30
#define SYSDIV2			23 /* SYSDIV2 offset */
#define SYSDIV2LSB		22
#define USBPWRDN		14
#define PWRDN2			13
#define BYPASS2			11
#define OSCSRC2			4 /* OSCSRC2 offset */
#define SYSDIV2_MASK		0x1F800000
#define OSCSRC2_MASK		0x00000070
#define OSCSRC2_MOSC		0x00000000 /* MOSC */
#define OSCSRC2_PIOSC		0x00000010 /* PIOSC */
#define OSCSRC2_PIOSC_DIV4	0x00000020 /* PIOSC/4 */
#define OSCSRC_IOSC_30K		0x00000030 /* 30 KHz IOSC */
#define OSCSRC_EOSC_4M		0x00000060 /* 4.194304 MHz EOSC */
#define OSCSRC_EOSC_32K		0x00000070 /* 32.768 KHz EOSC */

#define MOSCCTL			SYSTEM(0x07C)
#define DSLPCLKCFG		SYSTEM(0x144)
#define PIOSCCAL		SYSTEM(0x150)

/* clock config values (along with CLK_XTAL) */
#define CLK_SRC_PLL		0x00000000 /* system clock source is the PLL clock */
#define CLK_SRC_OSC		0x00003800 /* system clock source is the osc clock */
#define CLK_OSC_MOSC		0x00000000 /* osc clock source is MOSC */
#define CLK_OSC_PIOSC		0x00000010 /* osc clock source is PIOSC */
#define CLK_OSC_PIOSC_DIV4	0x00000020 /* osc clock source is PIOSC/4 */
#define CLK_OSC_IOSC_30K	0x00000030 /* osc clock source is 30 KHz IOSC */
#define CLK_OSC_EOSC_4M		0x80000028 /* osc clock source is 4.194304 MHz EOSC */
#define CLK_OSC_EOSC_32K	0x80000038 /* osc clock source is 32.768 KHz EOSC */
#define CLK_DIV_1		0x07800000 /* system clock is osc|PLL / 1 */
#define CLK_DIV_2		0x00C00000 /* system clock is osc|PLL / 2 */
#define CLK_DIV_3		0x01400000 /* system clock is osc|PLL / 3 */
#define CLK_DIV_4		0x01C00000 /* system clock is osc|PLL / 4 */
#define CLK_DIV_5		0x02400000 /* system clock is osc|PLL / 5 */
#define CLK_DIV_6		0x02C00000 /* system clock is osc|PLL / 6 */
#define CLK_DIV_7		0x03400000 /* system clock is osc|PLL / 7 */
#define CLK_DIV_8		0x03C00000 /* system clock is osc|PLL / 8 */
#define CLK_DIV_9		0x04400000 /* system clock is osc|PLL / 9 */
#define CLK_DIV_10		0x04C00000 /* system clock is osc|PLL / 10 */
#define CLK_DIV_11		0x05400000 /* system clock is osc|PLL / 11 */
#define CLK_DIV_12		0x05C00000 /* system clock is osc|PLL / 12 */
#define CLK_DIV_13		0x06400000 /* system clock is osc|PLL / 13 */
#define CLK_DIV_14		0x06C00000 /* system clock is osc|PLL / 14 */
#define CLK_DIV_15		0x07400000 /* system clock is osc|PLL / 15 */
#define CLK_DIV_16		0x07C00000 /* system clock is osc|PLL / 16 */
#define CLK_DIV_17		0x88400000 /* system clock is osc|PLL / 17 */
#define CLK_DIV_18		0x88C00000 /* system clock is osc|PLL / 18 */
#define CLK_DIV_19		0x89400000 /* system clock is osc|PLL / 19 */
#define CLK_DIV_20		0x89C00000 /* system clock is osc|PLL / 20 */
#define CLK_DIV_21		0x8A400000 /* system clock is osc|PLL / 21 */
#define CLK_DIV_22		0x8AC00000 /* system clock is osc|PLL / 22 */
#define CLK_DIV_23		0x8B400000 /* system clock is osc|PLL / 23 */
#define CLK_DIV_24		0x8BC00000 /* system clock is osc|PLL / 24 */
#define CLK_DIV_25		0x8C400000 /* system clock is osc|PLL / 25 */
#define CLK_DIV_26		0x8CC00000 /* system clock is osc|PLL / 26 */
#define CLK_DIV_27		0x8D400000 /* system clock is osc|PLL / 27 */
#define CLK_DIV_28		0x8DC00000 /* system clock is osc|PLL / 28 */
#define CLK_DIV_29		0x8E400000 /* system clock is osc|PLL / 29 */
#define CLK_DIV_30		0x8EC00000 /* system clock is osc|PLL / 30 */
#define CLK_DIV_31		0x8F400000 /* system clock is osc|PLL / 31 */
#define CLK_DIV_32		0x8FC00000 /* system clock is osc|PLL / 32 */
#define CLK_DIV_33		0x90400000 /* system clock is osc|PLL / 33 */
#define CLK_DIV_34		0x90C00000 /* system clock is osc|PLL / 34 */
#define CLK_DIV_35		0x91400000 /* system clock is osc|PLL / 35 */
#define CLK_DIV_36		0x91C00000 /* system clock is osc|PLL / 36 */
#define CLK_DIV_37		0x92400000 /* system clock is osc|PLL / 37 */
#define CLK_DIV_38		0x92C00000 /* system clock is osc|PLL / 38 */
#define CLK_DIV_39		0x93400000 /* system clock is osc|PLL / 39 */
#define CLK_DIV_40		0x93C00000 /* system clock is osc|PLL / 40 */
#define CLK_DIV_41		0x94400000 /* system clock is osc|PLL / 41 */
#define CLK_DIV_42		0x94C00000 /* system clock is osc|PLL / 42 */
#define CLK_DIV_43		0x95400000 /* system clock is osc|PLL / 43 */
#define CLK_DIV_44		0x95C00000 /* system clock is osc|PLL / 44 */
#define CLK_DIV_45		0x96400000 /* system clock is osc|PLL / 45 */
#define CLK_DIV_46		0x96C00000 /* system clock is osc|PLL / 46 */
#define CLK_DIV_47		0x97400000 /* system clock is osc|PLL / 47 */
#define CLK_DIV_48		0x97C00000 /* system clock is osc|PLL / 48 */
#define CLK_DIV_49		0x98400000 /* system clock is osc|PLL / 49 */
#define CLK_DIV_50		0x98C00000 /* system clock is osc|PLL / 50 */
#define CLK_DIV_51		0x99400000 /* system clock is osc|PLL / 51 */
#define CLK_DIV_52		0x99C00000 /* system clock is osc|PLL / 52 */
#define CLK_DIV_53		0x9A400000 /* system clock is osc|PLL / 53 */
#define CLK_DIV_54		0x9AC00000 /* system clock is osc|PLL / 54 */
#define CLK_DIV_55		0x9B400000 /* system clock is osc|PLL / 55 */
#define CLK_DIV_56		0x9BC00000 /* system clock is osc|PLL / 56 */
#define CLK_DIV_57		0x9C400000 /* system clock is osc|PLL / 57 */
#define CLK_DIV_58		0x9CC00000 /* system clock is osc|PLL / 58 */
#define CLK_DIV_59		0x9D400000 /* system clock is osc|PLL / 59 */
#define CLK_DIV_60		0x9DC00000 /* system clock is osc|PLL / 60 */
#define CLK_DIV_61		0x9E400000 /* system clock is osc|PLL / 61 */
#define CLK_DIV_62		0x9EC00000 /* system clock is osc|PLL / 62 */
#define CLK_DIV_63		0x9F400000 /* system clock is osc|PLL / 63 */
#define CLK_DIV_64		0x9FC00000 /* system clock is osc|PLL / 64 */
#define CLK_DIV_2_5		0xC1000000 /* system clock is PLL / 2.5 */
#define CLK_DIV_3_5		0xC1800000 /* system clock is PLL / 3.5 */
#define CLK_DIV_4_5		0xC2000000 /* system clock is PLL / 4.5 */
#define CLK_DIV_5_5		0xC2800000 /* system clock is PLL / 5.5 */
#define CLK_DIV_6_5		0xC3000000 /* system clock is PLL / 6.5 */
#define CLK_DIV_7_5		0xC3800000 /* system clock is PLL / 7.5 */
#define CLK_DIV_8_5		0xC4000000 /* system clock is PLL / 8.5 */
#define CLK_DIV_9_5		0xC4800000 /* system clock is PLL / 9.5 */
#define CLK_DIV_10_5		0xC5000000 /* system clock is PLL / 10.5 */
#define CLK_DIV_11_5		0xC5800000 /* system clock is PLL / 11.5 */
#define CLK_DIV_12_5		0xC6000000 /* system clock is PLL / 12.5 */
#define CLK_DIV_13_5		0xC6800000 /* system clock is PLL / 13.5 */
#define CLK_DIV_14_5		0xC7000000 /* system clock is PLL / 14.5 */
#define CLK_DIV_15_5		0xC7800000 /* system clock is PLL / 15.5 */
#define CLK_DIV_16_5		0xC8000000 /* system clock is PLL / 16.5 */
#define CLK_DIV_17_5		0xC8800000 /* system clock is PLL / 17.5 */
#define CLK_DIV_18_5		0xC9000000 /* system clock is PLL / 18.5 */
#define CLK_DIV_19_5		0xC9800000 /* system clock is PLL / 19.5 */
#define CLK_DIV_20_5		0xCA000000 /* system clock is PLL / 20.5 */
#define CLK_DIV_21_5		0xCA800000 /* system clock is PLL / 21.5 */
#define CLK_DIV_22_5		0xCB000000 /* system clock is PLL / 22.5 */
#define CLK_DIV_23_5		0xCB800000 /* system clock is PLL / 23.5 */
#define CLK_DIV_24_5		0xCC000000 /* system clock is PLL / 24.5 */
#define CLK_DIV_25_5		0xCC800000 /* system clock is PLL / 25.5 */
#define CLK_DIV_26_5		0xCD000000 /* system clock is PLL / 26.5 */
#define CLK_DIV_27_5		0xCD800000 /* system clock is PLL / 27.5 */
#define CLK_DIV_28_5		0xCE000000 /* system clock is PLL / 28.5 */
#define CLK_DIV_29_5		0xCE800000 /* system clock is PLL / 29.5 */
#define CLK_DIV_30_5		0xCF000000 /* system clock is PLL / 30.5 */
#define CLK_DIV_31_5		0xCF800000 /* system clock is PLL / 31.5 */
#define CLK_DIV_32_5		0xD0000000 /* system clock is PLL / 32.5 */
#define CLK_DIV_33_5		0xD0800000 /* system clock is PLL / 33.5 */
#define CLK_DIV_34_5		0xD1000000 /* system clock is PLL / 34.5 */
#define CLK_DIV_35_5		0xD1800000 /* system clock is PLL / 35.5 */
#define CLK_DIV_36_5		0xD2000000 /* system clock is PLL / 36.5 */
#define CLK_DIV_37_5		0xD2800000 /* system clock is PLL / 37.5 */
#define CLK_DIV_38_5		0xD3000000 /* system clock is PLL / 38.5 */
#define CLK_DIV_39_5		0xD3800000 /* system clock is PLL / 39.5 */
#define CLK_DIV_40_5		0xD4000000 /* system clock is PLL / 40.5 */
#define CLK_DIV_41_5		0xD4800000 /* system clock is PLL / 41.5 */
#define CLK_DIV_42_5		0xD5000000 /* system clock is PLL / 42.5 */
#define CLK_DIV_43_5		0xD5800000 /* system clock is PLL / 43.5 */
#define CLK_DIV_44_5		0xD6000000 /* system clock is PLL / 44.5 */
#define CLK_DIV_45_5		0xD6800000 /* system clock is PLL / 45.5 */
#define CLK_DIV_46_5		0xD7000000 /* system clock is PLL / 46.5 */
#define CLK_DIV_47_5		0xD7800000 /* system clock is PLL / 47.5 */
#define CLK_DIV_48_5		0xD8000000 /* system clock is PLL / 48.5 */
#define CLK_DIV_49_5		0xD8800000 /* system clock is PLL / 49.5 */
#define CLK_DIV_50_5		0xD9000000 /* system clock is PLL / 50.5 */
#define CLK_DIV_51_5		0xD9800000 /* system clock is PLL / 51.5 */
#define CLK_DIV_52_5		0xDA000000 /* system clock is PLL / 52.5 */
#define CLK_DIV_53_5		0xDA800000 /* system clock is PLL / 53.5 */
#define CLK_DIV_54_5		0xDB000000 /* system clock is PLL / 54.5 */
#define CLK_DIV_55_5		0xDB800000 /* system clock is PLL / 55.5 */
#define CLK_DIV_56_5		0xDC000000 /* system clock is PLL / 56.5 */
#define CLK_DIV_57_5		0xDC800000 /* system clock is PLL / 57.5 */
#define CLK_DIV_58_5		0xDD000000 /* system clock is PLL / 58.5 */
#define CLK_DIV_59_5		0xDD800000 /* system clock is PLL / 59.5 */
#define CLK_DIV_60_5		0xDE000000 /* system clock is PLL / 60.5 */
#define CLK_DIV_61_5		0xDE800000 /* system clock is PLL / 61.5 */
#define CLK_DIV_62_5		0xDF000000 /* system clock is PLL / 62.5 */
#define CLK_DIV_63_5		0xDF800000 /* system clock is PLL / 63.5 */

void clk_hw_set_config(uint32_t cfg);

#define clk_hw_usb_enable()	__raw_clearl_atomic(USBPWRDN, RCC2);
#define clk_hw_usb_disable()	__raw_setl_atomic(USBPWRDN, RCC2);

#endif /* __CLK_LM3S9B92_H_INCLUDE__ */
