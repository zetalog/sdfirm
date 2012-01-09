#include <target/boot.h>
#include <target/uart.h>

#define BOOT_TABLE_SIZE		256
#define BOOT_USB_ID_SIZE	0x12
/* API Nb */
#define BOOT_HW_API_CRAM_READ	0x03
#define BOOT_HW_API_CRAM_WRITE	0x04
#define BOOT_HW_API_GET_ID	0x06

typedef uint16_t (*boot_api_t)(uint8_t, uint16_t, uint8_t);

text_word_t *crc16_table = (text_word_t *)0x9000;
text_quad_t *crc32_table = (text_quad_t *)0x9200;
text_quad_t *cast256_s1_table = (text_quad_t *)0x9600;
text_quad_t *cast256_s2_table = (text_quad_t *)0x9A00;
text_quad_t *cast256_s3_table = (text_quad_t *)0x9E00;
text_quad_t *cast256_s4_table = (text_quad_t *)0xA200;
boot_api_t boot_hw_api = (boot_api_t)0xFF00;
text_byte_t *boot_hw_usb_id = (text_byte_t *)BOOT_HW_USB_ID;

uint8_t boot_hw_cram_readb(text_byte_t *addr)
{
	return (uint8_t)boot_hw_api(BOOT_HW_API_CRAM_READ,
				    (uint16_t)addr, 0);
}

void boot_hw_cram_writeb(text_byte_t *addr, uint8_t val)
{
	boot_hw_api(BOOT_HW_API_CRAM_WRITE, (uint16_t)addr, val);
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

uint16_t boot_hw_get_id(void)
{
	return boot_hw_api(BOOT_HW_API_GET_ID, 0, 0);
}

void boot_api_test(void)
{
	uint32_t i;
	uint16_t wv;
	uint32_t lv;

	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		wv = boot_hw_cram_readw(crc16_table+i);
		uart_putchar(HIBYTE(wv));
		uart_putchar(LOBYTE(wv));
	}
	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(crc32_table+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(cast256_s1_table+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(cast256_s2_table+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(cast256_s3_table+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_TABLE_SIZE; i++) {
		lv = boot_hw_cram_readl(cast256_s4_table+i);
		uart_putchar(HIBYTE(HIWORD(lv)));
		uart_putchar(LOBYTE(HIWORD(lv)));
		uart_putchar(HIBYTE(LOWORD(lv)));
		uart_putchar(LOBYTE(LOWORD(lv)));
	}
	for (i = 0; i < BOOT_USB_ID_SIZE; i++) {
		uart_putchar(boot_hw_cram_readb(boot_hw_usb_id+i));
	}
	uart_putchar(HIBYTE(boot_hw_get_id()));
	uart_putchar(LOBYTE(boot_hw_get_id()));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_PLLDIV_CFG));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_PLLXTAL_CFG));
	uart_putchar(boot_hw_cram_readb(BOOT_HW_SOFTWARE_ID));

	lv = boot_hw_cram_readw(BOOT_HW_CODE_LENGTH);
	uart_putchar(HIBYTE(HIWORD(lv)));
	uart_putchar(LOBYTE(HIWORD(lv)));
	uart_putchar(HIBYTE(LOWORD(lv)));
	uart_putchar(LOBYTE(LOWORD(lv)));
}
