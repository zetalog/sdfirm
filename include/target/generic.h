#ifndef __GENERIC_H_INCLUDE__
#define __GENERIC_H_INCLUDE__

#include <target/compiler.h>
#include <target/types.h>
#include <target/const.h>
#include <target/sizes.h>

#ifndef __ASSEMBLY__
#include <target/bitops.h>
#include <target/list.h>
#include <target/string.h>
#include <target/panic.h>
#include <target/muldiv.h>
#include <target/version.h>
#include <target/circbf.h>

#ifndef offsetof
#define offsetof(s, m)			((size_t)(&(((s *)0)->m)))
#endif

void system_init(void);
void system_suspend(void);
void system_resume(void);

#include <target/devid.h>

#define OPEN_READ		0x00
#define OPEN_WRITE		0x01
#endif /* __ASSEMBLY__ */

#endif /* __GENERIC_H_INCLUDE__ */
