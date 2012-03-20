#ifndef __LED_AT8XC5122_H_INCLUDE__
#define __LED_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>

Sfr(LEDCON0,	0xF1);		/* LED control 0 */
Sfr(LEDCON1,	0xE1);		/* LED control 1 */

Sbit(LED3,	0xB0, 7);	/* Led Number 3 */
Sbit(LED2,	0xB0, 6);	/* Led Number 2 */
Sbit(LED1,	0xB0, 4);	/* Led Number 1 */
Sbit(LED0,	0xB0, 2);	/* Led Number 0 */
Sbit(LED5,	0xC0, 4);	/* Led Number 5 */
Sbit(LED4,	0xC0, 3);	/* Led Number 4 */

/* LEDCON */
#define MSK_LEDCON_LED3H	0x80	/* LEDCON0 */
#define MSK_LEDCON_LED3L	0x40
#define MSK_LEDCON_LED2H	0x20
#define MSK_LEDCON_LED2L	0x10
#define MSK_LEDCON_LED1H	0x08
#define MSK_LEDCON_LED1L	0x04
#define MSK_LEDCON_LED0H	0x02
#define MSK_LEDCON_LED0L	0x01
#define MSK_LEDCON_LED6H	0x20	/* LEDCON1 */
#define MSK_LEDCON_LED6L	0x10
#define MSK_LEDCON_LED5H	0x08
#define MSK_LEDCON_LED5L	0x04
#define MSK_LEDCON_LED4H	0x02
#define MSK_LEDCON_LED4L	0x01

#define LED_0			P3_2
#define LED_1			P3_4
#define LED_2			P3_6

/* LED CONTROL */
#define light_led(led)		((led) = 0)
#define out_led(led)		((led) = 1)

#endif
