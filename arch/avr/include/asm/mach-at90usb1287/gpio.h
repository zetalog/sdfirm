#ifndef __GPIO_AT90USB1287_H_INCLUDE__
#define __GPIO_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

/* 
 * Define Generic PORTn, DDn, and PINn values.
 * Port Input Pins (generic)
 * Port Data Direction Register (generic)
 * Port Data Register (generic) 
 */
#define PINA	_SFR_IO8(0X00)
#define PINA7	7
#define PINA6	6
#define PINA5	5
#define PINA4	4
#define PINA3	3
#define PINA2	2
#define PINA1	1
#define PINA0	0

#define DDRA	_SFR_IO8(0X01)
#define DDA7	7
#define DDA6	6
#define DDA5	5
#define DDA4	4
#define DDA3	3
#define DDA2	2
#define DDA1	1
#define DDA0	0

#define PORTA	_SFR_IO8(0X02)
#define PA7	7
#define PA6	6
#define PA5	5
#define PA4	4
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0

#define PINB	_SFR_IO8(0X03)
#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0

#define DDRB	_SFR_IO8(0x04)
#define DDB7	7
#define DDB6	6
#define DDB5	5
#define DDB4	4
#define DDB3	3
#define DDB2	2
#define DDB1	1
#define DDB0	0

#define PORTB	_SFR_IO8(0x05)
#define PB7	7
#define PB6	6
#define PB5	5
#define PB4	4
#define PB3	3
#define PB2	2
#define PB1	1
#define PB0	0

#define PINC	_SFR_IO8(0x06)
#define PINC7	7
#define PINC6	6
#define PINC5	5
#define PINC4	4
#define PINC3	3
#define PINC2	2
#define PINC1	1
#define PINC0	0

#define DDRC	_SFR_IO8(0x07)
#define DDC7	7
#define DDC6	6
#define DDC5	5
#define DDC4	4
#define DDC3	3
#define DDC2	2
#define DDC1	1
#define DDC0	0

#define PORTC	_SFR_IO8(0x08)
#define PC7	7
#define PC6	6
#define PC5	5
#define PC4	4
#define PC3	3
#define PC2	2
#define PC1	1
#define PC0	0

#define PIND	_SFR_IO8(0x09)
#define PIND7	7
#define PIND6	6
#define PIND5	5
#define PIND4	4
#define PIND3	3
#define PIND2	2
#define PIND1	1
#define PIND0	0

#define DDRD	_SFR_IO8(0x0A)
#define DDD7	7
#define DDD6	6
#define DDD5	5
#define DDD4	4
#define DDD3	3
#define DDD2	2
#define DDD1	1
#define DDD0	0

#define PORTD	_SFR_IO8(0x0B)
#define PD7	7
#define PD6	6
#define PD5	5
#define PD4	4
#define PD3	3
#define PD2	2
#define PD1	1
#define PD0	0

#define PINE	_SFR_IO8(0x0C)
#define PINE7	7
#define PINE6	6
#define PINE5	5
#define PINE4	4
#define PINE3	3
#define PINE2	2
#define PINE1	1
#define PINE0	0

#define DDRE	_SFR_IO8(0x0D)
#define DDE7	7
#define DDE6	6
#define DDE5	5
#define DDE4	4
#define DDE3	3
#define DDE2	2
#define DDE1	1
#define DDE0	0

#define PORTE	_SFR_IO8(0x0E)
#define PE7	7
#define PE6	6
#define PE5	5
#define PE4	4
#define PE3	3
#define PE2	2
#define PE1	1
#define PE0	0

#define PINF	_SFR_IO8(0x0F)
#define PINF7	7
#define PINF6	6
#define PINF5	5
#define PINF4	4
#define PINF3	3
#define PINF2	2
#define PINF1	1
#define PINF0	0

#define DDRF	_SFR_IO8(0x10)
#define DDF7	7
#define DDF6	6
#define DDF5	5
#define DDF4	4
#define DDF3	3
#define DDF2	2
#define DDF1	1
#define DDF0	0

#define PORTF	_SFR_IO8(0x11)
#define PF7	7
#define PF6	6
#define PF5	5
#define PF4	4
#define PF3	3
#define PF2	2
#define PF1	1
#define PF0	0

/* GPIO */
#define GPIOR0	_SFR_IO8(0x1E)
#define GPIOR1	_SFR_IO8(0x2A)
#define GPIOR2	_SFR_IO8(0x2B)

#endif /* __GPIO_AT90USB1287_H_INCLUDE__ */
