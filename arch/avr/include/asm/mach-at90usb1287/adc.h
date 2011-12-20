#ifndef __ADC_AT90USB1287_H_INCLUDE__
#define __ADC_AT90USB1287_H_INCLUDE__

/*
 * ADC Control and Status Register
 */
#define ACSR	_SFR_IO8(0x30)
#define ADCSRA	_SFR_MEM8(0x7A)
#define ADCSRB	_SFR_MEM8(0x7B)

/* 
 * ADC Data Register
 */
#define ADCW	_SFR_MEM16(0x78)
#define ADCL	_SFR_MEM8(0x78)
#define ADCH	_SFR_MEM8(0x79)

/*
 * ADC Multiplexer Selection Register
 */
#define ADMUX	_SFR_MEM8(0x7C)

/*
 * Digital Input Disable Reigster
 */
#define DIDR0	_SFR_MEM8(0x7E)
#define DIDR1	_SFR_MEM8(0x7F)

#endif /* __ADC_AT90USB1287_H_INCLUDE__ */
