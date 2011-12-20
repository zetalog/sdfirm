#ifndef __IRQ_AT90SCR100_H_INCLUDE__
#define __IRQ_AT90SCR100_H_INCLUDE__

/* Interrupt vectors */
/* External Interrupt Request 0 */
#define IRQ_EXT0			_VECTOR(1)
/* External Interrupt Request 1 */
#define IRQ_EXT1			_VECTOR(2)
/* External Interrupt Request 2 */
#define IRQ_EXT2			_VECTOR(3)
/* External Interrupt Request 3 */
#define IRQ_EXT3			_VECTOR(4)
/* Pin Change Interrupt Request 0 */
#define IRQ_PCINT0			_VECTOR(5)
/* Pin Change Interrupt Request 1 */
#define IRQ_PCINT1			_VECTOR(6)
/* Pin Change Interrupt Request 2 */
#define IRQ_PCINT2			_VECTOR(7)
/* Watchdog Time-out Interrupt */
#define IRQ_WDT				_VECTOR(8)
/* Timer/Counter2 Compare Match A */
#define IRQ_OCF2A			_VECTOR(9)
/* Timer/Counter2 Compare Match B */
#define IRQ_OCF2B			_VECTOR(10)
/* Timer/Counter2 Overflow */
#define IRQ_OVF2			_VECTOR(11)
/* Timer/Counter1 Capture Event */
#define IRQ_ICF1			_VECTOR(12)
/* Timer/Counter1 Compare Match A */
#define IRQ_OCF1A			_VECTOR(13)
/* Timer/Counter1 Compare Match B */
#define IRQ_OCF1B			_VECTOR(14)
/* Timer/Counter1 Overflow */
#define IRQ_OVF1			_VECTOR(15)
/* Timer/Counter0 Compare Match A */
#define IRQ_OCF0A			_VECTOR(16)
/* Timer/Counter0 Compare Match B */
#define IRQ_OCF0B			_VECTOR(17)
/* Timer/Counter0 Overflow */
#define IRQ_OVF0			_VECTOR(18)
/* SPI Serial Transfer Complete */
#define IRQ_SPI_STC			_VECTOR(19)
/* USART, Rx Complete */
#define IRQ_USART1_RX			_VECTOR(20)
/* USART Data register Empty */
#define USART_UDRE_vect			_VECTOR(21)
/* USART, Tx Complete */
#define IRQ_USART1_TX			_VECTOR(22)
/* Supply Monitior Interruption */
#define IRQ_SUPPLY_MON			_VECTOR(23)
/* Reserved For Future Use */
#define IRQ_RFU				_VECTOR(24)
/* EEPROM Ready */
#define IRQ_EEPROM			_VECTOR(25)
/* 2-wire Serial Interface */
#define IRQ_TWI				_VECTOR(26)
/* Store Program Memory Read */
#define IRQ_SPM				_VECTOR(27)
/* Keyboard Input Changed */
#define IRQ_KBD				_VECTOR(28)
/* AES Block Operation Ended */
#define IRQ_AES				_VECTOR(29)
/* High-Speed SPI Interface */
#define IRQ_HSSPI			_VECTOR(30)
/* USB Endpoint linked Interrupt */
#define IRQ_USBT			_VECTOR(31)
/* USB Protocol Interrupt */
#define IRQ_USB				_VECTOR(32)
/* Smart Card Reader Interface */
#define IRQ_SCIB			_VECTOR(33)
/* USB Host Controller Interrupt */
#define IRQ_USB_HOST			_VECTOR(34)
/* USB Host Pipe Interrupt */
#define IRQ_USB_HOST_PIPE		_VECTOR(35)
/* Card Presence Detection */
#define IRQ_CPRES			_VECTOR(36)
/* Pin Change Interrupt Request 3 */
#define IRQ_PCINT3			_VECTOR(37)

#define NR_IRQS	37

/*
 * Pin Change Interrupt Control, Flag, Mask Register
 */
#define PCICR   _SFR_MEM8(0x68)
#define PCIFR   _SFR_IO8(0x1B)
#define PCMSK3  _SFR_MEM8(0x73)
#define PCMSK2  _SFR_MEM8(0x6D)
#define PCMSK1  _SFR_MEM8(0x6C)
#define PCMSK0  _SFR_MEM8(0x6B)

/*
 * External Interrupt Control, Flag, Mask, Redirection Register
 */
#define EICRA   _SFR_MEM8(0x69)
#define EIFR    _SFR_IO8(0x1C)
#define EIMSK   _SFR_IO8(0x1D)
#define EIRR	_SFR_IO8(0x1A)

#endif  /* __IRQ_AT90SCR100_H_INCLUDE__ */
