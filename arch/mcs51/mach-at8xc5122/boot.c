#include <target/boot.h>
#include <target/uart.h>

/* API Nb */
#define BOOT_HW_API_CRAM_READ	0x01
#define BOOT_HW_API_CRAM_WRITE	0x02
#define BOOT_HW_API_GET_ID	0x05

#define BOOT_HW_API_ENTRY	0xFF00
/* PLLDIV config byte */
#define BOOT_HW_PLLDIV_CFG	((text_byte_t *)0x7FFB)
/* PLL/XTAL config byte */
#define BOOT_HW_PLLXTAL_CFG	((text_byte_t *)0x7FFC)
/* software identification byte */
#define BOOT_HW_SOFTWARE_ID	((text_byte_t *)0x7FFD)
/* software code length */
#define BOOT_HW_CODE_LENGTH	((text_quad_t *)0x7FFE)

/* There are 18 bytes to specify USB IDs. Values used by bootloader is:
 * 0x12,
 * 0x01(bDescriptorType),
 * 0x1001(bscUSB),
 * 0xFF, 0xFF, 0x00,
 * 32(bMaxPacketSize0),
 * 0xEB03(idVendor),
 * 0xFE2F(idProduct),
 * 0x0000, 0, 0, 0, 1
 */
struct __boot_usb_id {
	uint8_t bLength;
	uint8_t bDescriptorType;	/* DEVICE descriptor */
	uint16_t bscUSB;		/* binary code decimal spec */
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
};

#define BOOT_HW_USB_ID		((text_byte_t *)0x7F00)
#define BOOT_HW_USB_ID_SIZE	0x12

/* tables are all 256 bytes long*/
#define BOOT_HW_TABLE_SIZE	256
#define BOOT_HW_CRC16_TABLE	((text_word_t *)0x9000)
#define BOOT_HW_CRC32_TABLE	((text_quad_t *)0x9200)
#define BOOT_HW_CAST256_S1	((text_quad_t *)0x9600)
#define BOOT_HW_CAST256_S2	((text_quad_t *)0x9A00)
#define BOOT_HW_CAST256_S3	((text_quad_t *)0x9A00)
#define BOOT_HW_CAST256_S4	((text_quad_t *)0xA200)

uint8_t boot_hw_api_a, boot_hw_api_b;

uint16_t boot_hw_get_id(void)
{
	__asm
	mov	r7, BOOT_HW_API_GET_ID
	lcall	BOOT_HW_API_ENTRY
	mov	_boot_hw_api_a, a
	mov	_boot_hw_api_b, b
	__endasm;
	return MAKEWORD(boot_hw_api_a, boot_hw_api_b);
}

uint8_t boot_hw_cram_readb(text_byte_t *addr)
{
	__asm
	mov	r7, BOOT_HW_API_CRAM_READ
	lcall	BOOT_HW_API_ENTRY
	mov	_boot_hw_api_a, a
	__endasm;
	return boot_hw_api_a;
}

void boot_hw_cram_writeb(text_byte_t *addr, uint8_t val)
{
	boot_hw_api_a = val;
	__asm
	mov	r7, BOOT_HW_API_CRAM_WRITE
	mov	a, _boot_hw_api_a
	lcall	BOOT_HW_API_ENTRY
	__endasm;
}

uint16_t boot_hw_cram_readw(text_word_t *addr)
{
	text_byte_t *caddr = (text_byte_t *)addr;
	return MAKEWORD(boot_hw_cram_readb(caddr),
			boot_hw_cram_readb(caddr+1));
}

uint32_t boot_hw_cram_readl(text_quad_t *addr)
{
	text_word_t *caddr = (text_word_t *)addr;
	return MAKELONG(boot_hw_cram_readw(caddr),
			boot_hw_cram_readw(caddr+1));
}

void boot_api_test(void)
{
	uint32_t i;
	uint16_t wv;
	uint32_t lv;

	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		wv = boot_hw_cram_readw(BOOT_HW_CRC16_TABLE+i);
		uart_putchar(HIBYTE(wv));
		uart_putchar(LOBYTE(wv));
	}
	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(BOOT_HW_CRC32_TABLE+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(BOOT_HW_CAST256_S1+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(BOOT_HW_CAST256_S2+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(BOOT_HW_CAST256_S3+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_HW_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(BOOT_HW_CAST256_S4+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_HW_USB_ID_SIZE; i++) {
		uart_putchar(boot_hw_cram_readb(BOOT_HW_USB_ID+i));
	}
	uart_putchar(HIBYTE(boot_hw_get_id()));
	uart_putchar(LOBYTE(boot_hw_get_id()));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_PLLDIV_CFG));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_PLLXTAL_CFG));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_SOFTWARE_ID));

	lv = boot_hw_cram_readl(BOOT_HW_CODE_LENGTH);
	uart_putchar(HIBYTE(HIWORD(lv)));
	uart_putchar(LOBYTE(HIWORD(lv)));
	uart_putchar(HIBYTE(LOWORD(lv)));
	uart_putchar(LOBYTE(LOWORD(lv)));
}
