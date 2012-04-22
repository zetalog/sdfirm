#include <target/config.h>
#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_LCD_EZIO
extern void appl_ezio_init(void);
#else
#define appl_ezio_init()
#endif

void appl_init(void)
{
	appl_ezio_init();
}

