#ifndef __WDT_H_INCLUDE__
#define __WDT_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <driver/wdt.h>

#define wdt_ctrl_stop()		wdt_hw_ctrl_stop()

#endif
