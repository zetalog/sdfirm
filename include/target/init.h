#ifndef __INIT_H_INCLUDE__
#define __INIT_H_INCLUDE__

#include <target/compiler.h>

#define __HEAD		.section	".head.text","ax"
#define __INIT		.section	".init.text","ax"

/* ARM: THUMB2 firmware specific entry vector */
#define __LOVEC		.section	".lovec.text","ax"
#define __HIVEC		.section	".hivec.text","ax"

#define __init		__section(.init.text)
#define __initdata	__section(.init.data)
#define __initconst	__section(.init.rodata)
#define __exitdata	__section(.exit.data)

#include <asm/asm-offsets.h>

#endif
