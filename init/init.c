#include <target/config.h>
#include <target/io.h>

/* This could be optimized on gcc by calling init functions from a link
 * stage array.  As SDCC can not support this and firmware may not include
 * so many modules, we decide to list all of the functions here after.
 */
/* normal subsystems */
#ifdef CONFIG_UART
void uart_init(void);
#else
#define uart_init()
#endif
#ifdef CONFIG_I2C
void i2c_init(void);
#else
#define i2c_init()
#endif
#ifdef CONFIG_SPI
void spi_init(void);
#else
#define spi_init()
#endif
#ifdef CONFIG_USB
void usb_init(void);
#else
#define usb_init()
#endif
#ifdef CONFIG_PCI
void pci_init(void);
#else
#define pci_init()
#endif
#ifdef CONFIG_RIO
void rio_init(void);
#else
#define rio_init()
#endif
#ifdef CONFIG_NET
void net_init(void);
#else
#define net_init()
#endif
#ifdef CONFIG_SCSI
void scsi_init(void);
#else
#define scsi_init()
#endif
#ifdef CONFIG_LPC
void lpc_init(void);
#else
#define lpc_init()
#endif
#ifdef CONFIG_ESPI
void espi_init(void);
#else
#define espi_init()
#endif
#ifdef CONFIG_MTD
void mtd_init(void);
#else
#define mtd_init()
#endif
#ifdef CONFIG_IOMMU
void iommu_init(void);
#else
#define iommu_init();
#endif

#ifdef CONFIG_LCD
void lcd_init(void);
#else
#define lcd_init()
#endif
#ifdef CONFIG_LED
void led_init(void);
#else
#define led_init()
#endif
#ifdef CONFIG_KBD
void kbd_init(void);
#else
#define kbd_init()
#endif
#ifdef CONFIG_VIDEO
void video_init(void);
#else
#define video_init()
#endif

#ifdef CONFIG_IFD
void ifd_init(void);
#else
#define ifd_init()
#endif
#ifdef CONFIG_DDR
void ddr_init(void);
#else
#define ddr_init()
#endif
#ifdef CONFIG_MMC_CORE
void mmcsd_init(void);
#else
#define mmcsd_init()
#endif
#ifdef CONFIG_COS
void cos_init(void);
#else
#define cos_init()
#endif
#ifdef CONFIG_SCS_SLOT
void scs_slot_init(void);
#else
#define scs_slot_init()
#endif

/* modules */
#ifdef CONFIG_MTD_FLASH
void flash_init(void);
#else
#define flash_init()
#endif
#ifdef CONFIG_MTD_DATAFLASH
void dataflash_init(void);
#else
#define dataflash_init()
#endif
#ifdef CONFIG_MTD_SPIFLASH
void spiflash_init(void);
#else
#define spiflash_init()
#endif

#ifdef CONFIG_USB_HID
void hid_init(void);
#else
#define hid_init()
#endif
#ifdef CONFIG_USB_DFU
void dfu_init(void);
#else
#define dfu_init()
#endif
#ifdef CONFIG_USB_MSD
void msd_init(void);
#else
#define msd_init()
#endif
#ifdef CONFIG_USB_SCD
void scd_init(void);
#else
#define scd_init()
#endif
#ifdef CONFIG_USB_PN53X
void usb_pn53x_init(void);
#else
#define usb_pn53x_init()
#endif

#ifdef CONFIG_TASK
void task_init(void);
#else
#define task_init()
#endif

#ifdef CONFIG_TERM
void term_init(void);
#else
#define term_init()
#endif

void modules_init(void)
{
	/* buses */
	lpc_init();
	espi_init();
	pci_init();
	rio_init();
	uart_init();
	i2c_init();
	spi_init();
	net_init();
	mmcsd_init();
	ddr_init();
	usb_init();
	mtd_init();
	iommu_init();

	/* subsys */
	led_init();	/* on GPIO */
	kbd_init();	/* on GPIO */
	lcd_init();	/* on mem/GPIO */
	video_init();
	scsi_init();
	ifd_init();	/* on uart */

	/* keymods */
	term_init();	/* for console */
	scs_slot_init();/* for security */

	/* busmods */
	flash_init();
	dataflash_init();
	spiflash_init();
	hid_init();
	msd_init();
	dfu_init();
	scd_init();
	cos_init();

	/* apps */
	usb_pn53x_init();
}
