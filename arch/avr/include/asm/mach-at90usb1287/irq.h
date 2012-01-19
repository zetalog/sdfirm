#ifndef __IRQ_AT90USB1287_H_INCLUDE__
#define __IRQ_AT90USB1287_H_INCLUDE__

/* Interrupt vectors */
/* External Interrupt Request 0 */
#define IRQ_EXT0			_VECTOR(1)
/* External Interrupt Request 1 */
#define IRQ_EXT1			_VECTOR(2)
/* External Interrupt Request 2 */
#define IRQ_EXT2			_VECTOR(3)
/* External Interrupt Request 3 */
#define IRQ_EXT3			_VECTOR(4)
/* External Interrupt Request 4 */
#define IRQ_EXT4			_VECTOR(5)
/* External Interrupt Request 5 */
#define IRQ_EXT5			_VECTOR(6)
/* External Interrupt Request 6 */
#define IRQ_EXT6			_VECTOR(7)
/* External Interrupt Request 7 */
#define IRQ_EXT7			_VECTOR(8)
/* Pin Change Interrupt Request 0 */
#define IRQ_PCINT0			_VECTOR(9)
/* USB General Interrupt Request */
#define IRQ_USB				_VECTOR(10)
/* USB Endpoint/Pipe Transfer Interrupt Request */
#define IRQ_USBT			_VECTOR(11)
/* Watchdog Time-out Interrupt */
#define IRQ_WDT				_VECTOR(12)

/* Timer/Counter2 Compare Match A */
#define IRQ_OCF2A			_VECTOR(13)
/* Timer/Counter2 Compare Match B */
#define IRQ_OCF2B			_VECTOR(14)
/* Timer/Counter2 Overflow */
#define IRQ_OVF2			_VECTOR(15)

/* Timer/Counter1 Capture Event */
#define IRQ_ICF1			_VECTOR(16)
/* Timer/Counter1 Compare Match A */
#define IRQ_OCF1A			_VECTOR(17)
/* Timer/Counter1 Compare Match B */
#define IRQ_OCF1B			_VECTOR(18)
/* Timer/Counter1 Compare Match C */
#define IRQ_OCF1C			_VECTOR(19)
/* Timer/Counter1 Overflow */
#define IRQ_OVF1			_VECTOR(20)

/* Timer/Counter0 Compare Match A */
#define IRQ_OCF0A			_VECTOR(21)
/* Timer/Counter0 Compare Match B */
#define IRQ_OCF0B			_VECTOR(22)
/* Timer/Counter0 Overflow */
#define IRQ_OVF0			_VECTOR(23)

/* SPI Serial Transfer Complete */
#define IRQ_SPI_STC			_VECTOR(24)

/* USART1, Rx Complete */
#define IRQ_USART1_RX			_VECTOR(25)
/* USART1 Data register Empty */
#define USART1_UDRE_vect		_VECTOR(26)
/* USART1, Tx Complete */
#define IRQ_USART1_TX			_VECTOR(27)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(28)

/* ADC Conversion Complete */
#define IRQ_ADC				_VECTOR(29)

/* EEPROM Ready */
#define IRQ_EEPROM			_VECTOR(30)

/* Timer/Counter3 Capture Event */
#define IRQ_ICF3			_VECTOR(31)
/* Timer/Counter3 Compare Match A */
#define IRQ_OCF3A			_VECTOR(32)
/* Timer/Counter3 Compare Match B */
#define IRQ_OCF3B			_VECTOR(33)
/* Timer/Counter3 Compare Match C */
#define IRQ_OCF3C			_VECTOR(34)
/* Timer/Counter3 Overflow */
#define IRQ_OVF3			_VECTOR(35)

/* 2-wire Serial Interface */
#define IRQ_TWI				_VECTOR(36)

/* Store Program Memory Read */
#define IRQ_SPM				_VECTOR(37)

#define NR_IRQS	37

/*
 * Pin Change Interrupt Control, Flag, Mask Register
 */
#define PCICR   _SFR_MEM8(0x68)
#define PCIFR   _SFR_IO8(0x1B)
#define PCMSK0  _SFR_MEM8(0x6B)

/*
 * External Interrupt Control, Flag, Mask Register
 */
#define EICRA   _SFR_MEM8(0x69)
#define EICRB   _SFR_MEM8(0x6A)
#define EIFR    _SFR_IO8(0x1C)
#define EIMSK   _SFR_IO8(0x1D)

#endif  /* __IRQ_AT90USB1287_H_INCLUDE__ */
