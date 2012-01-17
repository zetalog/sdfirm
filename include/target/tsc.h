#ifndef __TSC_H_INCLUDE__
#define __TSC_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <driver/tsc.h>

#define tsc_read_counter()	tsc_hw_read_counter()

#endif /* __TSC_H_INCLUDE__ */
