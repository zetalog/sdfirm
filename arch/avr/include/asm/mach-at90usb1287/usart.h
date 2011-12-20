#ifndef __USART_AT90USB1287_H_INCLUDE__
#define __USART_AT90USB1287_H_INCLUDE__

#define USART_HW_MODE_ASYNC	0x00
#define USART_HW_MODE_SYNC	0x40
#define USART_HW_MODE_MSPIM	0xC0

#define USART_HW_BITS_MASK	0x03
#define USART_HW_BITS_OFFSET	1

#define AT90USB1287_USART(n)					\
static inline void __usart##n##_hw_set_double(void)		\
{								\
	UCSR##n##A |= _BV(U2X##n);				\
}								\
static inline void __usart##n##_hw_clear_double(void)		\
{								\
	UCSR##n##A &= _BV(U2X##n);				\
}								\
static inline void __usart##n##_hw_config_mode(uint8_t mode)	\
{								\
	UCSR##n##C = mode;					\
}								\
static inline void __usart##n##_hw_set_bits(uint8_t bits)	\
{								\
	UCSR##n##C |= ((bits & USART_HW_BITS_MASK) <<		\
		       USART_HW_BITS_OFFSET);			\
	UCSR##n##B = ((bits == 4)?_BV(UCSZ##n##2):0);		\
}								\
static inline void __usart##n##_hw_set_parity(uint8_t parity)	\
{								\
	switch (parity) {					\
	case UART_PARITY_ODD:					\
		UCSR##n##C |= _BV(UPM##n##0);			\
	case UART_PARITY_EVEN:					\
		UCSR##n##C |= _BV(UPM##n##1);			\
		break;						\
	}							\
}								\
static inline void __usart##n##_hw_set_stopb(uint8_t stopb)	\
{								\
	UCSR##n##C |= stopb ? _BV(USBS##n) : 0;			\
}								\
static inline void __usart##n##_hw_write_byte(uint8_t byte)	\
{								\
	while (!((UCSR##n##A) & _BV(UDRE##n)))			\
		;						\
	UDR##n = byte;						\
}								\
static inline uint8_t __usart##n##_hw_read_byte(void)		\
{								\
	while (!(UCSR##n##A & _BV(RXC##n)))			\
		;						\
	return UDR##n;						\
}								\
static inline void __usart##n##_hw_enable_io(void)		\
{								\
	UCSR##n##B |= _BV(RXEN##n) | _BV(TXEN##n);		\
}								\
static inline void __usart##n##_hw_disable_io(void)		\
{								\
	UCSR##n##B &= ~(_BV(RXEN##n) | _BV(TXEN##n));		\
}								\
static inline void __usart##n##_hw_config_brg(uint16_t brg)	\
{								\
	UBRR##n##H = HIBYTE(brg);				\
	UBRR##n##L = LOBYTE(brg);				\
}

#endif /* __USART_AT90USB1287_H_INCLUDE__ */
