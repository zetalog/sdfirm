#include "mach.h"
#include <host/usbip.h>
#include <target/usb.h>

/* FIFO variables */
uint8_t usbd_in_xfr[SIM_USB_ENDP][SIM_USB_SIZE];
uint8_t usbd_out_xfr[SIM_USB_ENDP][SIM_USB_SIZE];
uint8_t usbd_in_idx[SIM_USB_ENDP];
uint8_t usbd_out_idx[SIM_USB_ENDP];
uint8_t usbd_in_cnt[SIM_USB_ENDP];
uint8_t usbd_out_cnt[SIM_USB_ENDP];

uint8_t usbd_in_type[SIM_USB_ENDP];
uint8_t usbd_out_type[SIM_USB_ENDP];

#define SIM_ENDP_ENABLED	0x80

uint16_t usbd_out_setup = 0x00;
uint16_t usbd_out_rxout = 0x00;
uint16_t usbd_in_txin = 0x00;
uint16_t usbd_in_irqen = 0x00;
uint16_t usbd_out_irqen = 0x00;
uint16_t usbd_in_stall = 0x00;
uint16_t usbd_out_stall = 0x00;

uint8_t usbd_dev_event = 0;
uint8_t usbd_dev_irqen = 0x00;

/* MUTEX for FIFO locking */
CRITICAL_SECTION usb_mutex;

void __sim_usbd_raise_eirq(uint8_t eid, uint8_t irq);
void __sim_usbd_unraise_eirq(uint8_t eid, uint8_t irq);
void __sim_usbd_reset_fifo(uint8_t dir, uint8_t eid);
void __sim_usbd_raise_stall(uint8_t dir, uint8_t eid);
void __sim_usbd_unraise_stall(uint8_t dir, uint8_t eid);

void sim_usb_lock_fifo(void)
{
	EnterCriticalSection(&usb_mutex);
}

void sim_usb_unlock_fifo(void)
{
	LeaveCriticalSection(&usb_mutex);
}

void sim_usb_sync_irq(void)
{
	boolean res = false;
	if (usbd_dev_event & usbd_dev_irqen)
		res = true;
	if (usbd_in_txin & usbd_in_irqen)
		res = true;
	if ((usbd_out_rxout | usbd_out_setup) & usbd_out_irqen)
		res = true;
	if (res)
		sim_irq_raise_irq(IRQ_USB);
	else
		sim_irq_unraise_irq(IRQ_USB);
}

void sim_usbd_enable_fifo(uint8_t dir, uint8_t eid, uint8_t type)
{
	if (type == USB_ENDP_CONTROL) {
		usbd_out_type[eid] = SIM_ENDP_ENABLED | type;
		usbd_in_type[eid] = SIM_ENDP_ENABLED | type;
	} else {
		if (dir == USB_DIR_OUT) {
			usbd_out_type[eid] = SIM_ENDP_ENABLED | type;
		} else {
			usbd_in_type[eid] = SIM_ENDP_ENABLED | type;
		}
	}
}

void sim_usbd_disable_fifo(uint8_t dir, uint8_t eid)
{
	if (usbd_out_type[eid] == USB_ENDP_CONTROL) {
		usbd_out_type[eid] = 0;
		usbd_in_type[eid] = 0;
	} else {
		if (dir == USB_DIR_OUT) {
			usbd_out_type[eid] = 0;
		} else {
			usbd_in_type[eid] = 0;
		}
	}
}

uint8_t sim_usbd_fifo_count(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_IN) {
		return usbd_in_cnt[eid];
	} else {
		return usbd_out_cnt[eid];
	}
}

void sim_usbd_enable_eirq(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_OUT) {
		printf("#OUT\n");
		usbd_out_irqen |= (1<<eid);
		usbip_dev_raise(dir, eid, USBIP_FIFO_EVENT_RXPOLL);
	} else {
		printf("#IN\n");
		usbd_in_irqen |= (1<<eid);
		usbip_dev_raise(dir, eid, USBIP_FIFO_EVENT_TXPOLL);
	}
	sim_usb_sync_irq();
}

void sim_usbd_disable_eirq(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_OUT) {
		printf("=OUT\n");
		usbd_out_irqen &= ~(1<<eid);
	} else {
		printf("=IN\n");
		usbd_in_irqen &= ~(1<<eid);
	}
	sim_usb_sync_irq();
}

void __sim_usbd_raise_stall(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_OUT) {
		if (!usbd_out_stall & (1<<eid)) {
			printf("+STALL\n");
			usbd_out_stall |= (1<<eid);
		}
		if (usbd_out_type[eid] == USB_ENDP_CONTROL) {
			__sim_usbd_unraise_eirq(eid, USBD_IRQ_SETUP);
			__sim_usbd_unraise_eirq(eid, USBD_IRQ_IN);
		}
		__sim_usbd_unraise_eirq(eid, USBD_IRQ_OUT);
	} else {
		if (!usbd_in_stall & (1<<eid)) {
			printf("+STALL\n");
			usbd_in_stall |= (1<<eid);
		}
		if (usbd_in_type[eid] == USB_ENDP_CONTROL) {
			__sim_usbd_unraise_eirq(eid, USBD_IRQ_SETUP);
			__sim_usbd_unraise_eirq(eid, USBD_IRQ_OUT);
		}
		__sim_usbd_unraise_eirq(eid, USBD_IRQ_IN);
	}
}

void __sim_usbd_unraise_stall(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_OUT) {
		if (usbd_out_stall & (1<<eid)) {
			printf("-STALL\n");
			usbd_out_stall &= ~(1<<eid);
		}
	} else {
		if (usbd_in_stall & (1<<eid)) {
			printf("-STALL\n");
			usbd_in_stall &= ~(1<<eid);
		}
		if (usbd_in_type[eid] != USB_ENDP_CONTROL)
			__sim_usbd_raise_eirq(eid, USBD_IRQ_IN);
	}
}

boolean sim_usbd_stall_raised(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_OUT)
		return usbd_out_stall & (1<<eid);
	else
		return usbd_in_stall & (1<<eid);
}

void sim_usbd_raise_stall(uint8_t dir, uint8_t eid)
{
	sim_usb_lock_fifo();
	__sim_usbd_raise_stall(dir, eid);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void sim_usbd_unraise_stall(uint8_t dir, uint8_t eid)
{
	sim_usb_lock_fifo();
	__sim_usbd_unraise_stall(dir, eid);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void __sim_usbd_reset_fifo(uint8_t dir, uint8_t eid)
{
	if (dir == USB_DIR_IN) {
		usbd_in_cnt[eid] = 0;
		usbd_in_idx[eid] = 0;
	} else {
		usbd_out_cnt[eid] = 0;
		usbd_out_idx[eid] = 0;
	}
	__sim_usbd_unraise_stall(dir, eid);
}

void sim_usbd_reset_fifo(uint8_t dir, uint8_t eid)
{
	sim_usb_lock_fifo();
	__sim_usbd_reset_fifo(dir, eid);
	sim_usb_unlock_fifo();
}

void __sim_usbd_raise_eirq(uint8_t eid, uint8_t irq)
{
	switch (irq) {
	case USBD_IRQ_SETUP:
		__sim_usbd_reset_fifo(USB_DIR_IN, eid);
		__sim_usbd_reset_fifo(USB_DIR_OUT, eid);
		if (!(usbd_out_setup & (1<<eid))) {
			printf("+SETUP\n");
			usbd_out_setup |= (1<<eid);
		}
		__sim_usbd_unraise_eirq(eid, USBD_IRQ_IN);
		__sim_usbd_unraise_eirq(eid, USBD_IRQ_OUT);
		break;
	case USBD_IRQ_OUT:
		if (!(usbd_out_rxout & (1<<eid))) {
			printf("+OUT\n");
			usbd_out_rxout |= (1<<eid);
		}
		break;
	case USBD_IRQ_IN:
		if (!(usbd_in_txin & (1<<eid))) {
			printf("+IN\n");
			usbd_in_txin |= (1<<eid);
		}
		break;
	case USBD_IRQ_STALL:
	default:
		BUG();
		break;
	}
}

void __sim_usbd_unraise_eirq(uint8_t eid, uint8_t irq)
{
	switch (irq) {
	case USBD_IRQ_SETUP:
		if (usbd_out_setup & (1<<eid)) {
			printf("-SETUP\n");
			usbd_out_setup &= ~(1<<eid);
			usbip_dev_raise(USB_DIR_OUT, eid,
					USBIP_FIFO_EVENT_RXCMPL);
		}
		__sim_usbd_raise_eirq(eid, USBD_IRQ_IN);
		break;
	case USBD_IRQ_OUT:
		if (usbd_out_rxout & (1<<eid)) {
			printf("-OUT\n");
			usbd_out_rxout &= ~(1<<eid);
			usbip_dev_raise(USB_DIR_OUT, eid,
					USBIP_FIFO_EVENT_RXCMPL);
		}
		break;
	case USBD_IRQ_IN:
		if (usbd_in_txin & (1<<eid)) {
			printf("-IN\n");
			usbd_in_txin &= ~(1<<eid);
			usbip_dev_raise(USB_DIR_IN, eid,
					USBIP_FIFO_EVENT_TXAVAL);
		}
		break;
	case USBD_IRQ_STALL:
	default:
		BUG();
		break;
	}
}

void sim_usbd_unraise_eirq(uint8_t eid, uint8_t irq)
{
	sim_usb_lock_fifo();
	__sim_usbd_unraise_eirq(eid, irq);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void sim_usbd_raise_eirq(uint8_t eid, uint8_t irq)
{
	sim_usb_lock_fifo();
	__sim_usbd_raise_eirq(eid, irq);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

boolean sim_usbd_eirq_raised(uint8_t eid, uint8_t irq)
{
	switch (irq) {
	case USBD_IRQ_SETUP:
		return usbd_out_setup & (1<<eid);
	case USBD_IRQ_OUT:
		return usbd_out_rxout & (1<<eid);
	case USBD_IRQ_IN:
		return usbd_in_txin & (1<<eid);
	case USBD_IRQ_STALL:
	default:
		BUG();
		return false;
	}
}

boolean sim_usbd_eirq_raised_any(uint8_t eid)
{
	uint16_t out_irq, in_irq;
	if (usbd_out_irqen & (1<<eid))
		out_irq = (usbd_out_setup | usbd_out_rxout | usbd_out_stall) & (1<<eid);
	if (usbd_in_irqen & (1<<eid))
		in_irq = (usbd_in_txin | usbd_in_stall) & (1<<eid);
	return out_irq | in_irq;
}

boolean sim_usbd_dirq_raised(uint8_t irq)
{
	return usbd_dev_event & (1<<irq);
}

uint8_t __sim_usb_read_fifo(uint8_t dir, uint8_t eid)
{
	uint8_t offset;
	uint8_t byte;

	if (dir == USB_DIR_OUT) {
		/* sdfirm */
		offset = usbd_out_idx[eid];
		BUG_ON(eid >= SIM_USB_ENDP);
		BUG_ON(offset >= SIM_USB_SIZE);
		byte = usbd_out_xfr[eid][offset];
		usbd_out_idx[eid] = (offset+1) & (SIM_USB_SIZE-1);
	} else {
		/* usbip */
		offset = usbd_in_idx[eid];
		BUG_ON(eid >= SIM_USB_ENDP);
		BUG_ON(offset >= SIM_USB_SIZE);
		byte = usbd_in_xfr[eid][offset];
		usbd_in_idx[eid] = (offset+1) & (SIM_USB_SIZE-1);
	}
	return byte;
}

uint8_t sim_usb_read_fifo(uint8_t dir, uint8_t eid)
{
	uint8_t byte;

	sim_usb_lock_fifo();
	byte = __sim_usb_read_fifo(dir, eid);
	sim_usb_unlock_fifo();
	return byte;
}

void __sim_usb_write_fifo(uint8_t dir, uint8_t eid, uint8_t byte)
{
	uint8_t offset;

	if (dir == USB_DIR_IN) {
		/* sdfirm */
		offset = usbd_in_cnt[eid];
		BUG_ON(eid >= SIM_USB_ENDP);
		BUG_ON(offset >= SIM_USB_SIZE);
		usbd_in_xfr[eid][offset] = byte;
		usbd_in_cnt[eid] = (offset+1) & (SIM_USB_SIZE-1);
	} else {
		/* usbip */
		offset = usbd_out_cnt[eid];
		BUG_ON(eid >= SIM_USB_ENDP);
		BUG_ON(offset >= SIM_USB_SIZE);
		usbd_out_xfr[eid][offset] = byte;
		usbd_out_cnt[eid] = (offset+1) & (SIM_USB_SIZE-1);
	}
}

void sim_usb_write_fifo(uint8_t dir, uint8_t eid, uint8_t byte)
{
	sim_usb_lock_fifo();
	__sim_usb_write_fifo(dir, eid, byte);
	sim_usb_unlock_fifo();
}

void sim_usbd_raise_out(uint8_t eid, uint8_t *buf, uint8_t len)
{
	uint8_t i;

	sim_usb_lock_fifo();
	__sim_usbd_raise_eirq(eid, USBD_IRQ_OUT);
	for (i = 0; i < USB_SETUP_REQUEST_SIZE; i++)
		__sim_usb_write_fifo(USB_DIR_OUT, eid, buf[i]);
	sim_usb_unlock_fifo();
}

uint8_t sim_usbd_raise_in(uint8_t eid, uint8_t *buf)
{
	uint8_t i;

	sim_usb_lock_fifo();
	for (i = 0; i < usbd_in_cnt[eid]; i++)
		buf[i] = __sim_usb_read_fifo(USB_DIR_IN, eid);
	__sim_usbd_raise_eirq(eid, USBD_IRQ_IN);
	sim_usb_unlock_fifo();
	return i;
}

void sim_usbd_raise_setup(uint8_t eid, uint8_t *buf)
{
	uint8_t i;

	sim_usb_lock_fifo();
	__sim_usbd_raise_eirq(eid, USBD_IRQ_SETUP);
	for (i = 0; i < USB_SETUP_REQUEST_SIZE; i++)
		__sim_usb_write_fifo(USB_DIR_OUT, eid, buf[i]);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void sim_usbd_enable_dirq(uint8_t irq)
{
	usbd_dev_irqen |= (1<<irq);
	sim_usb_sync_irq();
}

void sim_usbd_disable_dirq(uint8_t irq)
{
	usbd_dev_irqen &= ~(1<<irq);
	sim_usb_sync_irq();
}

void sim_usbd_unraise_dirq(uint8_t irq)
{
	sim_usb_lock_fifo();
	usbd_dev_event &= ~(1<<irq);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void sim_usbd_raise_dirq(uint8_t irq)
{
	sim_usb_lock_fifo();
	usbd_dev_event |= (1<<irq);
	sim_usb_unlock_fifo();
	sim_usb_sync_irq();
}

void sim_usb_init(void)
{
	InitializeCriticalSection(&usb_mutex);
	usbip_dev_init();
}
