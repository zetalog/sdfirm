#include <target/irq.h>

#ifdef CONFIG_GPT_AT8XC5122
void DECLARE_ISR(timer_isr(void), IRQ_GPT);
#endif
#ifdef CONFIG_KBD_AT8XC5122
void DECLARE_ISR(kbd_isr(void), IRQ_KBD);
#endif
#if defined(CONFIG_USB_AT8XC5122) && !defined(SYS_REALTIME)
void DECLARE_ISR(usb_isr(void), IRQ_USB);
#endif
#if defined(CONFIG_UART_AT8XC5122_ASYNC) && !defined(SYS_REALTIME)
void DECLARE_ISR(uart_isr(void), IRQ_UART);
#endif
#if defined(CONFIG_IFD_AT8XC5122) && defined(CONFIG_IFD_PRES_IRQ)
void DECLARE_ISR(scib_pres_isr(void), IRQ_INT1);
#endif
