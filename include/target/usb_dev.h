#ifndef __USB_DEV_H_INCLUDE__
#define __USB_DEV_H_INCLUDE__

#ifdef CONFIG_USB_DEBUG
#define usbd_debug_state(state)					\
	do {							\
		if (state != usbd_state_get()) {		\
			usb_debug(USB_DEBUG_STATE, state);	\
		}						\
	} while (0)
#define usbd_debug_stage(stage)					\
	do {							\
		if (usbd_control_get_stage() != stage) {	\
			usb_debug(USB_DEBUG_STAGE, stage);	\
		}						\
	} while (0)
#define usbd_debug_eirq(irq)					\
	do {							\
		usb_debug_irq(irq);				\
		usb_debug_eid(usbd_endp);			\
	} while (0)
#define USBD_DUMP_CONTROL		true
#else
#define usbd_debug_state(state)
#define usbd_debug_stage(stage)
#define usbd_debug_eirq(irq)
#define usbd_debug_call(call)
#define USBD_DUMP_CONTROL		false
#endif

#define USBD_CALL_POLL		0
#define USBD_CALL_DATA		1
#define USBD_CALL_DONE		2
#ifdef CONFIG_USB_DEBUG_CALL
#define usbd_debug_call(call)					\
	do {							\
		usb_debug(USB_DEBUG_CALL, call);		\
	} while (0)
#else
#define usbd_debug_call(call)
#endif

/* XXX: USB Device IO Dumper
 * In order to allow other modules to dump USB IO, dump functions are not
 * associated to CONFIG_USB_DEBUG, but associated to CONFIG_DEBUG_PRINT.
 * Unless necessary, do not call usbd_dump_save/usbd_dump_restore
 * directly, use usbd_dump_on/usbd_dump_off with dump switcher (see
 * USBD_DUMP_CONTROL for reference) instead.  Calling
 * usbd_dump_save/usbd_dump_restore directly will always lead to
 * additional useless stack consumption.
 */
#ifdef CONFIG_DEBUG_PRINT
void usbd_dump_restore(boolean dbg);
boolean usbd_dump_save(boolean dbg);
#define usbd_dump_on(on)		boolean odbg = usbd_dump_save(on)
#define usbd_dump_off()			usbd_dump_restore(odbg)
#else
#define usbd_dump_restore(dbg)
#define usbd_dump_save(dbg)		false
#define usbd_dump_on(on)
#define usbd_dump_off()
#endif

#define USBD_VENDOR_ID			((uint16_t)DEV_VENDOR_ID)
#define USBD_PRODUCT_ID			((uint8_t)DEV_PRODUCT_ID)
#define USBD_SERIAL_NO			((uint16_t)DEV_SERIAL_NO)

#define NR_USBD_CONFS		1
#ifdef CONFIG_USBD_MAX_INTFS
#define NR_USBD_INTFS		CONFIG_USBD_MAX_INTFS
#else
#define NR_USBD_INTFS		0
#endif
#ifdef NR_USBD_HW_ENDPS
#define NR_USBD_ENDPS		NR_USBD_HW_ENDPS
#else
#define NR_USBD_ENDPS		0
#endif

#define INVALID_USBD_IID	NR_USBD_INTFS
#define INVALID_USBD_EID	USB_ADDR(USB_DIR_IN, NR_USBD_ENDPS)

/* 10ms precesion for endpoint polling interval scheduling */
#define USBD_POLL_INTERVAL		10

struct usb_transfer {
	/* request available bytes, this should be usb_ctrl_reqs.wLength
	 * for control transfers.
	 */
	urb_size_t req_all;
	/* request totally handled bytes, shouldn't exceed req_all */
	urb_size_t req_cur;
	/* transfer available bytes */
	utb_size_t xfr_all;
	/* transfer handled bytes, shouldn't exceed xfr_all */
	utb_size_t xfr_cur;
	/* cursor pointing to IO buffer where next IN/OUT occurs */
	urb_size_t iter;
};

struct usbd_interface {
	uint8_t string_first;
	uint8_t string_last;
	uint8_t nr_endps;
	usb_size_cb config_len;
	usb_io_cb ctrl;
#ifdef CONFIG_USB_USBIP_DEV
	uint8_t (*attr)(uint8_t type);
#endif
};
__TEXT_TYPE__(struct usbd_interface, usbd_interface_t);

struct usbd_endp_ctrl {
	uint8_t ctrls;
#define USBD_CTRL_STAGE_MASK		0x03
#define USBD_CTRL_STAGE_SETUP		0x00
#define USBD_CTRL_STAGE_DATA		0x01
#define USBD_CTRL_STAGE_STATUS		0x02

#define USBD_REQ_FLAG_MASK		0xF0
#define USBD_REQ_FLAG_SOFT		0x10	/* usbd_request_submit called */
#define USBD_REQ_FLAG_HARD		0x20	/* usbd_hw_transfer_close called */
	/* XXX: USB Device Asynchronous TX/RXAVAL Events
	 *
	 * For asynchronous USB IOs:
	 * There are two kinds of TXIN interrupts implemented by
	 * various hardwares:
	 * 1. TXAVAL: interrupt is raised when FIFO is empty
	 * 2. TXCMPL: interrupt is raised when data in FIFO is transferred
	 *            to the BUS
	 * The latter can be treat as TXAVAL except that we must emulate a
	 * TXAVAL event before we can submit transfers to the hardware.
	 *
	 * There is only one kind of RXOUT interrupt implemented by
	 * various hardwares:
	 * 1. RXAVAL: interrupt is raised when FIFO is full of transferred
	 *            data from the BUS
	 * In order to integrate RXOUT transfers handling codes to the
	 * TXIN transfers handling codes, we must emulate RXCMPL event for
	 * the out transfers,  which might also be executed asynchronously
	 * on top of the stack.  Please refer to the codes in the async
	 * version of the __usbd_request_data.
	 *
	 * For synchronous USB IOs:
	 * We still need to emulate TXAVAL for the TXCMPL type hardwares.
	 * This is done through the USBD_REQ_FLAG_ASYNC as the async
	 * version.
	 *
	 * It is required by the synchronous version of USB IOs that the
	 * IRQs of TX/RXAVAL/CMPL to be masked before the sync IOs can
	 * run.  But the first and the last TXAVAL/RXAVAL events need to
	 * occur asynchronously.
	 * 1. TX/RXAVAL: USB endpoints IRQs are not masked as the async
	 *               version soon after the usbd_request_submit is
	 *               called.  The IRQs are masked immediately after
	 *               the first TXAVAL (which might be an emulation) or
	 *               RXAVAL IRQ is raised.
	 * 2. TX/RXCMPL: For the last transfer, the IRQs need to be opened
	 *               for the new occurance of the TXCMPL events.
	 *               need to be emulated.
	 */
#define USBD_REQ_FLAG_ASYNC		0x40	/* asynchronous TX/RXAVAL events occured */
#define USBD_REQ_FLAG_SYNC		0x80	/* synchronous TX/RXAVAL events occured */

#define USBD_ENDP_FLAG_MASK		0x0C
#define USBD_ENDP_FLAG_HALT		0x04
};

struct usbd_endpoint {
	uint8_t attrs;
#define USBD_ATTR2DIR(attrs)		USB_ADDR2DIR(attrs)
#define USBD_ATTR2TYPE(attrs)		((attrs) & USB_ENDP_TYPE_MASK)
#define USBD_DIR2ATTR(dir)		USB_DIR2ADDR(dir)
#define USBD_ENDP_CTRL			(USBD_DIR2ATTR(USB_DIR_OUT) | USB_ENDP_CONTROL)
#define USBD_ENDP_BULK_IN		(USBD_DIR2ATTR(USB_DIR_IN) | USB_ENDP_BULK)
#define USBD_ENDP_BULK_OUT		(USBD_DIR2ATTR(USB_DIR_OUT) | USB_ENDP_BULK)
#define USBD_ENDP_INTR_IN		(USBD_DIR2ATTR(USB_DIR_IN) | USB_ENDP_INTERRUPT)
#define USBD_ENDP_INTR_OUT		(USBD_DIR2ATTR(USB_DIR_OUT) | USB_ENDP_INTERRUPT)
	uint16_t interval;
	usb_io_cb poll;
	usb_io_cb iocb;
	usb_io_cb done;
};
__TEXT_TYPE__(struct usbd_endpoint, usbd_endpoint_t);

#ifdef CONFIG_USB_DEV
void usbd_start(void);
void usbd_stop(void);
void usbd_restart(void);
#else
#define usbd_start()
#define usbd_stop()
#define usbd_restart()
#endif

/*=========================================================================
 * IRQ handlers
 *=======================================================================*/
void usbd_bus_reset(void);
#ifdef CONFIG_PM
void usbd_bus_suspend(void);
void usbd_bus_resume(void);
#else
#define usbd_bus_suspend()
#define usbd_bus_resume()
#endif

void usbd_frame_start(uint16_t frame);

void usbd_request_poll(void);
void usbd_request_stall(void);
void usbd_control_start(void);
void usbd_control_setup(void);
void usbd_control_reset(void);
void usbd_transfer_txin(void);
void usbd_transfer_rxout(void);
void usbd_config_reset(void);
void usbd_config_apply(void);

/*=========================================================================
 * USB device
 *=======================================================================*/
void usbd_state_set(uint8_t state);
uint8_t usbd_state_get(void);

/*=========================================================================
 * USB interface
 *=======================================================================*/
usb_iid_t usbd_declare_interface(uint8_t power,
				 usbd_interface_t *cintf);
#ifdef CONFIG_USB_USBIP_DEV
uint8_t usbd_interface_get_attr(uint8_t iid, uint8_t type);
#else
#define usbd_interface_get_attr(iid, type)	0
#endif
extern __near__ uint8_t usbd_nr_intfs;

/*=========================================================================
 * USB endpoints
 *=======================================================================*/
#define usbd_endpoint_size_addr(addr)		usbd_hw_endp_size(addr)
uint8_t usbd_endpoint_dir_addr(uint8_t addr);
uint8_t usbd_endpoint_interval_addr(uint8_t addr);
uint8_t usbd_endpoint_attrs_addr(uint8_t addr);
uint8_t usbd_endpoint_type_addr(uint8_t addr);
#define usbd_endpoint_size()			usbd_endpoint_size_addr(usbd_endp)
#define usbd_endpoint_dir()			usbd_endpoint_dir_addr(usbd_endp)
#define usbd_endpoint_type()			usbd_endpoint_type_addr(usbd_endp)
#define usbd_endpoint_attrs()			usbd_endpoint_attrs_addr(usbd_endp)
#define usbd_endpoint_interval()		usbd_endpoint_interval_addr(usbd_endp)

void usbd_endpoint_reset(void);
void usbd_endpoint_stall(void);
void usbd_endpoint_halt(void);
void usbd_endpoint_unhalt(void);
boolean usbd_endpoint_halting(void);

void usbd_endpoint_start_addr(uint8_t addr);
void usbd_endpoint_reset_addr(uint8_t addr);
void usbd_endpoint_halt_addr(uint8_t addr);
void usbd_endpoint_unhalt_addr(uint8_t addr);

uint8_t usbd_claim_endpoint(boolean explicit_zlp,
			    usbd_endpoint_t *cendp);
boolean usbd_endpoint_claimed(uint8_t addr);

uint8_t usbd_addr_save(uint8_t addr);
void usbd_addr_restore(uint8_t addr);
uint8_t usbd_saved_addr(void);

/*=========================================================================
 * USB control request
 *=======================================================================*/
uint8_t usbd_control_setup_dir(void);
uint8_t usbd_control_setup_type(void);
uint8_t usbd_control_setup_recp(void);
uint8_t usbd_control_request_type(void);
uint16_t usbd_control_request_value(void);
uint16_t usbd_control_request_index(void);
uint16_t usbd_control_request_length(void);
#if 0
uint8_t usbd_control_request_iid(void);
#endif
uint8_t usbd_control_request_addr(void);

void usbd_control_set_stage(uint8_t stage);
uint8_t usbd_control_get_stage(void);
boolean usbd_control_setup_staging(void);

/*=========================================================================
 * USB request (IRP or URB)
 *=======================================================================*/
urb_size_t usbd_request_handled(void);
urb_size_t usbd_request_unhandled(void);
utb_size_t usbd_transfer_unhandled(void);
utb_size_t usbd_transfer_handled(void);

void usbd_request_reset(void);
boolean usbd_request_submit(uint8_t addr, urb_size_t length);
void usbd_request_commit(urb_size_t length);
void usbd_request_discard_addr(uint8_t addr);
void usbd_request_discard(void);
void usbd_request_reap(void);
uint8_t usbd_request_dir(void);

/* request is submitted */
boolean usbd_request_pending(void);
/* controller interrupt is enabled */
boolean usbd_request_running(void);
/* interrupt condition checking */
boolean usbd_request_interrupting(uint8_t dir);
/* halt condition checking */
boolean usbd_request_halting(void);
/* poll condition checking */
boolean usbd_request_polling(uint8_t dir);

/*=========================================================================
 * USB transfer
 *=======================================================================*/
void usbd_transfer_submit(utb_size_t bytes);
boolean usbd_transfer_last(void);

/*=========================================================================
 * USB IO
 *=======================================================================*/
void usbd_iter_accel(void);
void usbd_iter_reset(void);

void usbd_async_iocb(void);

#ifdef CONFIG_USBD_XFR_ASYNC
void usbd_inb_async(uint8_t byte);
void usbd_inw_async(uint16_t word);
void usbd_inl_async(uint32_t dword);
uint8_t usbd_outb_async(uint8_t byte);
uint16_t usbd_outw_async(uint16_t word);
uint32_t usbd_outl_async(uint32_t dword);
#else
#define usbd_request_syncing()		true
#define usbd_inb_async(byte)
#define usbd_inw_async(word)
#define usbd_inl_async(dword)
#define usbd_outb_async(byte)		((uint8_t)0x00)
#define usbd_outw_async(word)		((uint16_t)0x0000)
#define usbd_outl_async(dword)		((uint32_t)0x00000000)
#endif

#ifdef CONFIG_USBD_XFR_SYNC
void usbd_inb_sync(uint8_t byte);
void usbd_inw_sync(uint16_t word);
void usbd_inl_sync(uint32_t dword);
uint8_t usbd_outb_sync(void);
uint16_t usbd_outw_sync(void);
uint32_t usbd_outl_sync(void);
#else
#define usbd_request_syncing()		false
#define usbd_inb_sync(byte)
#define usbd_inw_sync(word)
#define usbd_inl_sync(dword)
#define usbd_outb_sync(byte)		((uint8_t)0x00)
#define usbd_outw_sync(word)		((uint16_t)0x0000)
#define usbd_outl_sync(dword)		((uint32_t)0x00000000)
#endif

#ifdef CONFIG_USBD_BULK
void usbd_bulk_open(size_t offset);
void usbd_bulk_close(bulk_size_t size);
void usbd_bulk_put(uint8_t byte);
uint8_t usbd_bulk_get(void);
void usbd_bulk_in(uint8_t *buffer, bulk_size_t size);
void usbd_bulk_out(uint8_t *buffer, bulk_size_t size);
#else
#define usbd_bulk_open(offset)
#define usbd_bulk_close(size)
#define usbd_bulk_put(byte)
#define usbd_bulk_get()			0x00
#define usbd_bulk_in(buffer, size)
#define usbd_bulk_out(buffer, size)
#endif

void usbd_inb(uint8_t byte);
void usbd_inw(uint16_t word);
void usbd_inl(uint32_t dword);
uint8_t usbd_outb(uint8_t byte);
uint16_t usbd_outw(uint16_t word);
uint32_t usbd_outl(uint32_t dword);

#define USBD_INB(byte)			(usbd_inb(byte))
#define USBD_INW(word)			(usbd_inw(word))
#define USBD_INL(dword)			(usbd_inl(dword))
#define USBD_OUTB(byte)			(byte = usbd_outb(byte))
#define USBD_OUTW(word)			(word = usbd_outw(word))
#define USBD_OUTL(dword)		(dword = usbd_outl(dword))

#define USBD_OUT_BEGIN(byte)					\
	{							\
		urb_size_t __l = usbd_request_handled();	\
		USBD_OUTB(byte);				\
		if (usbd_request_handled() != __l)
#define USBD_OUT_END						\
	}
#define USBD_IN_BEGIN(byte)					\
	{							\
		urb_size_t __l = usbd_request_handled();	\
		USBD_INB(byte);					\
		if (usbd_request_handled() != __l)
#define USBD_IN_END						\
	}

#ifdef CONFIG_USBD_XFR_ADAPT
void usbd_request_set_sync(void);
void usbd_request_clear_sync(void);
boolean usbd_request_syncing(void);
#else
#define usbd_request_set_sync()
#define usbd_request_clear_sync()
#endif

void usbd_input_interface_desc(uint8_t cls, uint8_t subcls,
			       uint8_t proto, uint8_t istring);
void usbd_input_endpoint_desc(uint8_t addr);
void usbd_input_string(text_char_t *string);
void usbd_input_device_name(void);
void usbd_input_serial_no(void);

/*=========================================================================
 * USB globals
 *=======================================================================*/
extern __near__ usb_addr_t usbd_address;
extern __near__ uint8_t usbd_config;
extern uint8_t usbd_endp;

#endif /* __USB_DEV_H_INCLUDE__ */
