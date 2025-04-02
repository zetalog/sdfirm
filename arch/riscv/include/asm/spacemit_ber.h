#ifndef __BER_SPACEMIT_H_INCLUDE__
#define __BER_SPACEMIT_H_INCLUDE__

#if !defined(__ASSEMBLY__) && !defined(__DTS__) & !defined(LINKER_SCRIPT)
#ifdef CONFIG_SPACEMIT_BER
void spacemit_ber_init(void);
#else /* CONFIG_SPACEMIT_BER */
#define spacemit_ber_init()	do { } while (0)
#endif /* CONFIG_SPACEMIT_BER */
#endif /* !__ASSEMBLY__ && !__DTS__ && !LINKER_SCRIPT */

#endif /* __BER_SPACEMIT_H_INCLUDE__ */
