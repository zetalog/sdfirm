#include <target/scsi.h>

struct sbc_bulkio {
	uint8_t open_type;
	uint16_t iter;
	bulk_size_t optimal_size;
};

sbc_lun_t sbc_nr_luns = 0;
scsi_lun_t sbc_scsi_luns[NR_SBC_UNITS];
struct sbc_device *sbc_devices[NR_SBC_UNITS];
sbc_lun_t sbc_lun = INVALID_SBC_LUN;
struct sbc_bulkio sbc_bulkios[NR_SBC_UNITS];

void sbc_restore_lun(sbc_lun_t lun)
{
	sbc_lun = lun;
}

sbc_lun_t sbc_save_lun(sbc_lun_t lun)
{
	sbc_lun_t olun = sbc_lun;
	sbc_restore_lun(lun);
	return olun;
}

static bulk_size_t sbc_bulkio_init(uint8_t open_type, uint8_t bulk_type)
{
	sbc_bulkios[sbc_lun].iter = 0;
	sbc_bulkios[sbc_lun].open_type = open_type;
	bulk_reset(scsi_current_cmnd.bulk, bulk_type);
	return sbc_bulkios[sbc_lun].optimal_size;
}

static void sbc_bulkio_exit(bulk_size_t optimal_size)
{
	sbc_bulkios[sbc_lun].optimal_size = optimal_size;
}

static void sbc_bulkio_iter(void)
{
	sbc_bulkios[sbc_lun].iter++;
}

static void sbc_lba_out_of_range(void)
{
	scsi_illegal_request(SCSI_ASC_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
}

static boolean sbc_invalid_lba_range(void)
{
	sbc_lun_t lun = sbc_current_unit();
	return (sbc_read_cmnd.lba > sbc_devices[lun]->nr_blocks) ||
	       ((sbc_read_cmnd.lba + scsi_current_cmnd.expect_length) >
	        sbc_devices[lun]->nr_blocks);
}

static void sbc_data_aval(void)
{
	if (sbc_invalid_lba_range()) {
		sbc_lba_out_of_range();
		return;
	}
	scsi_command_cmpl();
}

static void sbc_data_cmpl(void)
{
	scsi_command_done(SCSI_STATUS_GOOD);
}

static void sbc_bulk_open(size_t size)
{
	sbc_devices[sbc_lun]->open_block(sbc_devices[sbc_lun],
					 sbc_bulkios[sbc_lun].open_type,
					 sbc_read_cmnd.lba+sbc_bulkios[sbc_lun].iter,
					 1);
}

static void sbc_bulk_close(bulk_size_t size)
{
	sbc_devices[sbc_lun]->close_block(sbc_devices[sbc_lun]);
	sbc_bulkio_iter();
}

/*=========================================================================
 * READ(10) command
 *=======================================================================*/
void sbc_read10_send(void)
{
	sbc_lun_t lun = sbc_current_unit(), olun;
	bulk_size_t block_size, xprt_size;
	size_t bulk_total;

	olun = sbc_save_lun(lun);
	if (scsi_target_xprt->bulk_type() == BULK_TYPE_CPU) {
		block_size = sbc_bulkio_init(OPEN_READ, BULK_TYPE_CPU);
		xprt_size = scsi_target_xprt->bulk_size(BULK_SIZE_OPT);
		bulk_cpu_write(scsi_current_cmnd.bulk,
			       xprt_size,
			       scsi_target_xprt->open,
			       scsi_target_xprt->byte_in,
			       scsi_target_xprt->close);
		bulk_cpu_read(scsi_current_cmnd.bulk,
			      block_size,
			      sbc_bulk_open,
			      sbc_devices[lun]->read_byte,
			      sbc_bulk_close);
		bulk_total = (scsi_current_cmnd.expect_length) <<
			     (scsi_current_cmnd.expect_granularity);
		bulk_cpu_execute(scsi_current_cmnd.bulk, bulk_total);
		sbc_bulkio_exit(block_size);
	} else {
	}
	sbc_restore_lun(olun);
}

static void sbc_read10_parse(uint8_t *cdb, uint8_t len)
{
	sbc_lun_t lun = sbc_current_unit();
	/* TRANSFER LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_word(cdb, 7);
	scsi_current_cmnd.expect_granularity =
		__fls16(sbc_devices[lun]->block_size);
	sbc_read_cmnd.dpo = SCSI_CDB_BITS(cdb, SBC_READ_10_DPO);
	sbc_read_cmnd.fua = SCSI_CDB_BITS(cdb, SBC_READ_10_FUA);
	sbc_read_cmnd.fua_nv = SCSI_CDB_BITS(cdb, SBC_READ_10_FUA_NV);
	sbc_read_cmnd.rdprotect = SCSI_CDB_BITS(cdb, SBC_READ_10_RDPROTECT);
	sbc_read_cmnd.lba = scsi_cdb_long(cdb, 2);
	sbc_read_cmnd.group_no = SCSI_CDB_BITS(cdb, SBC_READ_10_GROUP_NUMBER);
	scsi_command_aval_in(sbc_read10_send);
}

/*=========================================================================
 * WRITE(10) command
 *=======================================================================*/
static void sbc_write10_recv(void)
{
	sbc_lun_t lun = sbc_current_unit(), olun;
	bulk_size_t block_size, xprt_size;
	size_t bulk_total;

	olun = sbc_save_lun(lun);
	if (scsi_target_xprt->bulk_type() == BULK_TYPE_CPU) {
		block_size = sbc_bulkio_init(OPEN_READ, BULK_TYPE_CPU);
		xprt_size = scsi_target_xprt->bulk_size(BULK_SIZE_OPT);
		bulk_cpu_read(scsi_current_cmnd.bulk,
			      xprt_size,
			      scsi_target_xprt->open,
			      scsi_target_xprt->byte_out,
			      scsi_target_xprt->close);
		bulk_cpu_write(scsi_current_cmnd.bulk,
			       block_size,
			       sbc_bulk_open,
			       sbc_devices[lun]->write_byte,
			       sbc_bulk_close);
		bulk_total = (scsi_current_cmnd.expect_length) <<
			     (scsi_current_cmnd.expect_granularity);
		bulk_cpu_execute(scsi_current_cmnd.bulk, bulk_total);
		sbc_bulkio_exit(block_size);
	} else {
	}
	sbc_restore_lun(olun);
}

static void sbc_write10_parse(uint8_t *cdb, uint8_t len)
{
	sbc_lun_t lun = sbc_current_unit();
	/* TRANSFER LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_word(cdb, 7);
	scsi_current_cmnd.expect_granularity =
		__fls16(sbc_devices[lun]->block_size);
	sbc_write_cmnd.dpo = SCSI_CDB_BITS(cdb, SBC_WRITE_10_DPO);
	sbc_write_cmnd.fua = SCSI_CDB_BITS(cdb, SBC_WRITE_10_FUA);
	sbc_write_cmnd.ebp = SCSI_CDB_BITS(cdb, SBC_WRITE_10_EBP);
	sbc_write_cmnd.lba = scsi_cdb_long(cdb, 2);
	scsi_command_aval_out(sbc_write10_recv);
}

/*=========================================================================
 * READ CAPACITY(10) command
 *=======================================================================*/
static void sbc_read_capacity10_parse(uint8_t *cdb, uint8_t len)
{
	/* The READ CAPACITY (10) command requests that the device server
	 * transfer 8 bytes of parameter data describing the capacity and
	 * medium format of the direct-access block device to the data-in
	 * buffer.
	 */
	/* ALLOCATION LENGTH */
	scsi_current_cmnd.expect_length = 8;
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
	sbc_lun_t lun = sbc_current_unit();
	uint32_t lba_last = (uint32_t)((sbc_devices[lun]->nr_blocks) - 1);
	uint32_t lbs_size = (uint32_t)sbc_devices[lun]->block_size;

	scsi_def_writel(lba_last);
	scsi_def_writel(lbs_size);

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
	case SBC_CMND_WRITE_10:
		sbc_data_aval();
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
	case SBC_CMND_WRITE_10:
		sbc_data_cmpl();
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
	sbc_lun_t lun = sbc_current_unit();
	uint16_t granularity = 1;
	uint32_t transfer_length = (uint32_t)SCSI_MAX_BUFFER >>
				   __fls16(sbc_devices[lun]->block_size);
	uint32_t optimal_length = transfer_length >> granularity;

	scsi_def_writeb(0);
	scsi_def_writeb(0);
	/* optimal transfer granularity */
	scsi_def_writew(granularity);
	/* maximum transfer length */
	scsi_def_writel(transfer_length);
	/* optimal transfer length */
	scsi_def_writel(optimal_length);
}

scsi_vpd_page_t sbc_block_limit_vpd = {
	sbc_block_limit_vpd_size,
	sbc_block_limit_vpd_data,
};

sbc_lun_t sbc_current_unit(void)
{
	sbc_lun_t lun;

	for (lun = 0; lun < sbc_nr_luns; lun++) {
		if (sbc_scsi_luns[lun] == scsi_lun)
			break;
	}
	return lun;
}

sbc_lun_t sbc_register_device(struct sbc_device *dev)
{
	sbc_lun_t lun;

	BUG_ON(sbc_nr_luns >= INVALID_SCSI_LUN);
	BUG_ON(!dev);
	BUG_ON(dev->block_size < SCSI_MAX_BUFFER);

	lun = sbc_nr_luns;
	sbc_devices[lun] = dev;
	sbc_scsi_luns[lun] = scsi_register_device(&scsi_block);
	sbc_bulkios[lun].optimal_size = dev->block_size;
	sbc_nr_luns++;
	return lun;
}

void scsi_sbc_init(void)
{
	scsi_register_vpd_page(SBC_VPD_BLOCK_LIMIT, &sbc_block_limit_vpd);
}
