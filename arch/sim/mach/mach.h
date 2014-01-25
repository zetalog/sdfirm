#ifndef __SIM_MACH_H_INCLUDE__
#define __SIM_MACH_H_INCLUDE__

#include <target/config.h>
#include <target/irq.h>
#include <target/gpt.h>
#include <target/jiffies.h>

/* firmware APIs */
void system_init(void);

/* IRQ APIs called by simulator */
boolean irq_hw_irq_enabled(uint8_t irq);

/* simulator APIs */
void sim_init(void);
void sim_run(void);

void sim_cpu_init(void);
void sim_irq_init(void);
void sim_uart_init(void);
#ifdef CONFIG_USB
void sim_usb_init(void);
#else
#define sim_usb_init()
#endif
void sim_mem_init(void);

void sim_irq_raise_irq(uint8_t irq);
void sim_irq_unraise_irq(uint8_t irq);
void sim_irq_schedule_all(void);
void sim_irq_register_vect(uint8_t irq, irq_vect_cb vect);
void sim_irq_execute_vect(uint8_t irq);
void sim_gpt_oneshot_timeout(timeout_t tout_ms);
void sim_uart_write_byte(uint8_t byte);
void sim_mem_post_heap(void);

#define SIM_USB_SIZE	64
#define SIM_USB_ENDP	16

void sim_usbd_enable_dirq(uint8_t irq);
void sim_usbd_disable_dirq(uint8_t irq);
void sim_usbd_raise_dirq(uint8_t irq);
void sim_usbd_unraise_dirq(uint8_t irq);
boolean sim_usbd_dirq_raised(uint8_t irq);
uint8_t sim_usb_read_fifo(uint8_t dir, uint8_t eid);
void sim_usb_write_fifo(uint8_t dir, uint8_t eid, uint8_t byte);
void sim_usbd_unraise_eirq(uint8_t eid, uint8_t irq);
void sim_usbd_raise_eirq(uint8_t eid, uint8_t irq);
boolean sim_usbd_eirq_raised_any(uint8_t eid);
boolean sim_usbd_eirq_raised(uint8_t eid, uint8_t irq);
void sim_usbd_reset_fifo(uint8_t dir, uint8_t eid);
void sim_usbd_enable_fifo(uint8_t dir, uint8_t eid, uint8_t type);
void sim_usbd_disable_fifo(uint8_t dir, uint8_t eid);
uint8_t sim_usbd_fifo_count(uint8_t dir, uint8_t eid);
void sim_usbd_enable_eirq(uint8_t dir, uint8_t eid);
void sim_usbd_disable_eirq(uint8_t dir, uint8_t eid);
void sim_usbd_raise_setup(uint8_t eid, uint8_t *xfr);
void sim_usbd_raise_out(uint8_t eid, uint8_t *buf, uint8_t len);
uint8_t sim_usbd_raise_in(uint8_t eid, uint8_t *buf);

boolean sim_usbd_stall_raised(uint8_t dir, uint8_t eid);
void sim_usbd_raise_stall(uint8_t dir, uint8_t eid);
void sim_usbd_unraise_stall(uint8_t dir, uint8_t eid);

#endif /* __SIM_MACH_H_INCLUDE__ */
