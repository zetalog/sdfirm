#include <target/scsi.h>
#include <target/mtd.h>
#include <target/bulk.h>

extern mtd_t board_flash;
#define MTD_BLOCK_SIZE	((mtd_addr_t)512)

scsi_lun_t sbc_luns[1];

static void sbc_lba_out_of_range(void)
{
	scsi_illegal_request(SCSI_ASC_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
}

static boolean sbc_invalid_lba_range(void)
{
	struct mtd_info *info = mtd_get_info(board_flash);
	return (sbc_read_cmnd.lba + scsi_current_cmnd.expect_length) >
	       ((scsi_lba_t)info->nr_pages << 1);
}

/*=========================================================================
 * READ(10) command
 *=======================================================================*/
/*=========================================================================
 * WRITE(10) command
 *=======================================================================*/
#include <target/usb.h>
#include <target/spi.h>

void sbc_read10_send(void)
{
	scsi_lbs_t j, size;
	mtd_t smtd;

	size = scsi_current_cmnd.expect_length;
	smtd = mtd_save_device(board_flash);

	j = 0;
	size >>= 6;
	while (j < size) {
		if (!(j & 7)) {
			mtd_open(OPEN_READ,
				 ((mtd_addr_t)(sbc_read_cmnd.lba+(j>>3)) << __fls16(MTD_BLOCK_SIZE)),
				 MTD_BLOCK_SIZE);
		}

		usbd_bulk_open(j << 6);
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_put(spi_hw_read_byte());
		usbd_bulk_close(64);
		j++;

		if (!(j & 7)) {
			mtd_close();
		}
	}

	BUG_ON(j & 7);
	mtd_restore_device(smtd);
}

static void sbc_write10_recv(void)
{
	scsi_lbs_t j, size;
	mtd_t smtd;

	size = scsi_current_cmnd.expect_length;
	smtd = mtd_save_device(board_flash);

	j = 0;
	size >>= 6;
	while (j < size) {
		if (!(j & 7)) {
			mtd_open(OPEN_WRITE,
				 ((mtd_addr_t)(sbc_read_cmnd.lba+(j>>3)) << __fls16(MTD_BLOCK_SIZE)),
				 MTD_BLOCK_SIZE);
		}
		usbd_bulk_open(j << 6);
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		spi_hw_write_byte(usbd_bulk_get());
		usbd_bulk_close(64);
		j++;

		if (!(j & 7)) {
			mtd_close();
		}
	}

	BUG_ON(j & 7);
	mtd_restore_device(smtd);
}

static void sbc_read10_cmpl(void)
{
	scsi_current_cmnd.data_length =
		(scsi_lbs_t)mul16u((uint16_t)scsi_current_cmnd.expect_length,
				   MTD_BLOCK_SIZE);
	scsi_command_done(SCSI_STATUS_GOOD);
}

static void sbc_read10_parse(uint8_t *cdb, uint8_t len)
{
	scsi_current_cmnd.expect_length = scsi_cdb_word(cdb, 7);
	sbc_read_cmnd.dpo = SCSI_CDB_BITS(cdb, SBC_READ_10_DPO);
	sbc_read_cmnd.fua = SCSI_CDB_BITS(cdb, SBC_READ_10_FUA);
	sbc_read_cmnd.fua_nv = SCSI_CDB_BITS(cdb, SBC_READ_10_FUA_NV);
	sbc_read_cmnd.rdprotect = SCSI_CDB_BITS(cdb, SBC_READ_10_RDPROTECT);
	sbc_read_cmnd.lba = scsi_cdb_long(cdb, 2);
	sbc_read_cmnd.group_no = SCSI_CDB_BITS(cdb, SBC_READ_10_GROUP_NUMBER);
	scsi_command_aval_in(sbc_read10_send);
}

static void sbc_read10_aval(void)
{
	if (sbc_invalid_lba_range()) {
		sbc_lba_out_of_range();
		return;
	}
	scsi_command_cmpl();
}

static void sbc_write10_parse(uint8_t *cdb, uint8_t len)
{
	scsi_current_cmnd.expect_length = scsi_cdb_word(cdb, 7);
	sbc_write_cmnd.dpo = SCSI_CDB_BITS(cdb, SBC_WRITE_10_DPO);
	sbc_write_cmnd.fua = SCSI_CDB_BITS(cdb, SBC_WRITE_10_FUA);
	sbc_write_cmnd.ebp = SCSI_CDB_BITS(cdb, SBC_WRITE_10_EBP);
	sbc_write_cmnd.lba = scsi_cdb_long(cdb, 2);
	scsi_command_aval_out(sbc_write10_recv);
}

static void sbc_write10_aval(void)
{
	if (sbc_invalid_lba_range()) {
		sbc_lba_out_of_range();
		return;
	}
	scsi_command_cmpl();
}

static void sbc_write10_cmpl(void)
{
	scsi_command_done(SCSI_STATUS_GOOD);
}

/*=========================================================================
 * READ CAPACITY(10) command
 *=======================================================================*/
static void sbc_read_capacity10_parse(uint8_t *cdb, uint8_t len)
{
	sbc_read_capacity_cmnd.lba = scsi_cdb_long(cdb, 2);
	sbc_read_capacity_cmnd.pmi = SCSI_CDB_BITS(cdb, SBC_READ_CAPACITY_10_PMI);
	scsi_command_aval(SCSI_CMND_IN);
}

static void sbc_read_capacity10_aval(void)
{
	if (sbc_read_capacity_cmnd.pmi == 0) {
		if (sbc_read_capacity_cmnd.lba != 0) {
			scsi_invalid_cdb_field();
			return;
		}
	}
	scsi_command_cmpl();
}

static void sbc_read_capacity10_cmpl(void)
{
	struct mtd_info *info = mtd_get_info(board_flash);
	scsi_lba_t lba_last = ((scsi_lba_t)(info->nr_pages) << 1) - 1;

	scsi_def_writel(lba_last);
	scsi_def_writel(MTD_BLOCK_SIZE);

	scsi_command_done(SCSI_STATUS_GOOD);
}

static void sbc_parse_cdb(uint8_t *cdb, uint8_t len)
{
	switch (scsi_current_cmnd.opcode) {
	case SBC_CMND_READ_CAPACITY_10:
		sbc_read_capacity10_parse(cdb, len);
		break;
	case SBC_CMND_READ_10:
		sbc_read10_parse(cdb, len);
		break;
	case SBC_CMND_WRITE_10:
		sbc_write10_parse(cdb, len);
		break;
	default:
		scsi_invalid_cmd_opcode();
		break;
	}
}

static void sbc_cmnd_aval(void)
{
	switch (scsi_current_cmnd.opcode) {
	case SBC_CMND_READ_CAPACITY_10:
		sbc_read_capacity10_aval();
		break;
	case SBC_CMND_READ_10:
		sbc_read10_aval();
		break;
	case SBC_CMND_WRITE_10:
		sbc_write10_aval();
		break;
	}
}

static void sbc_cmnd_cmpl(void)
{
	switch (scsi_current_cmnd.opcode) {
	case SBC_CMND_READ_CAPACITY_10:
		sbc_read_capacity10_cmpl();
		break;
	case SBC_CMND_READ_10:
		sbc_read10_cmpl();
		break;
	case SBC_CMND_WRITE_10:
		sbc_write10_cmpl();
		break;
	}
}

static uint8_t sbc_inquiry(uint8_t page_code, uint8_t offset)
{
	switch (page_code) {
	case 0x00:
		switch (offset) {
		case 5:
			return SBC_TARGET_PROTECT;
		}
		break;
	case SCSI_VPD_EXTENDED_INQUIRY_DATA:
		switch (offset) {
		case 4:
			return SBC_TARGET_RTO | SBC_TARGET_GRD_CHK |
			       SBC_TARGET_APP_CHK | SBC_TARGET_REF_CHK;
		case 5:
			return SBC_TARGET_GROUP_SUP;
		case 6:
			return SBC_TARGET_NV_SUP | SBC_TARGET_V_SUP;
		}
		break;
	}
	return 0;
}

static boolean sbc_test_ready(void)
{
	return true;
}

scsi_device_t scsi_block = {
	SCSI_DEVICE_DIRECT_ACCESS_BLOCK,
	"sbc",
	SBC_VER_DESC_SBC2,
	SCSI_DEVICE_RMB,
	sbc_inquiry,
	sbc_parse_cdb,
	sbc_cmnd_aval,
	sbc_cmnd_cmpl,
	sbc_test_ready,
};

static uint16_t sbc_block_limit_vpd_size(uint8_t page_code)
{
	return SBC_VPD_BLOCK_LIMIT_LENGTH;
}

static void sbc_block_limit_vpd_data(uint8_t page_code)
{
	uint16_t granularity = 1;
	struct mtd_info *info = mtd_get_info(board_flash);
	uint32_t transfer_length = (uint32_t)SCSI_MAX_BUFFER >> (info->pageorder-1);

	scsi_def_writeb(0);
	scsi_def_writeb(0);
	/* optimal transfer granularity */
	scsi_def_writew(granularity);
	/* maximum transfer length */
	scsi_def_writel(transfer_length);
	/* optimal transfer length */
	scsi_def_writel(transfer_length);
}

scsi_vpd_page_t sbc_block_limit_vpd = {
	sbc_block_limit_vpd_size,
	sbc_block_limit_vpd_data,
};

void scsi_sbc_init(void)
{
	sbc_luns[0] = scsi_register_device(&scsi_block);
	scsi_register_vpd_page(SBC_VPD_BLOCK_LIMIT, &sbc_block_limit_vpd);
}
