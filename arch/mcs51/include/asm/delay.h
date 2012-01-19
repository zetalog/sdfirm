#ifndef __DELAY_MCS51_H_INCLUDE__
#define __DELAY_MCS51_H_INCLUDE__

void __delay(volatile lps_t loops);

#define ARCH_HAVE_DELAY 1

#endif /* __DELAY_MCS51_H_INCLUDE__ */
