#ifndef __SCSI_SBC_H_INCLUDE__
#define __SCSI_SBC_H_INCLUDE__

#define SBC_VER_DESC_SBC		0x0180
#define SBC_VER_DESC_SBC2		0x0320

#ifdef CONFIG_SBC_PROTECT
#define SBC_TARGET_PROTECT	SCSI_INQUIRY_PROTECT
#else
#define SBC_TARGET_PROTECT	0
#endif

#ifdef CONFIG_SBC_PROTECT_RTO
#define SBC_TARGET_RTO		SCSI_INQUIRY_EXT_RTO
#else
#define SBC_TARGET_RTO		0
#endif
#ifdef CONFIG_SBC_PROTECT_GRD
#define SBC_TARGET_GRD_CHK	SCSI_INQUIRY_EXT_GRD_CHK
#else
#define SBC_TARGET_GRD_CHK	0
#endif
#ifdef CONFIG_SBC_PROTECT_APP
#define SBC_TARGET_APP_CHK	SCSI_INQUIRY_EXT_APP_CHK
#else
#define SBC_TARGET_APP_CHK	0
#endif
#ifdef CONFIG_SBC_PROTECT_REF
#define SBC_TARGET_REF_CHK	SCSI_INQUIRY_EXT_REF_CHK
#else
#define SBC_TARGET_REF_CHK	0
#endif
#ifdef CONFIG_SBC_GROUP
#define SBC_TARGET_GROUP_SUP	SCSI_INQUIRY_EXT_GROUP_SUP
#else
#define SBC_TARGET_GROUP_SUP	0
#endif
#ifdef CONFIG_SBC_NV
#define SBC_TARGET_NV_SUP	SCSI_INQUIRY_EXT_NV_SUP
#else
#define SBC_TARGET_NV_SUP	0
#endif
#ifdef CONFIG_SBC_V
#define SBC_TARGET_V_SUP	SCSI_INQUIRY_EXT_V_SUP
#else
#define SBC_TARGET_V_SUP	0
#endif

#define SBC_VPD_BLOCK_LIMIT		0xB0
#define SBC_VPD_BLOCK_LIMIT_LENGTH	0x0C

#define SBC_CMND_FORMAT_UNIT		0x04
#define SBC_CMND_READ_10		0x28
#define SBC_CMND_WRITE_10		0x2A
#define SBC_CMND_VERIFY_10		0x2F
#define SBC_CMND_READ_CAPACITY_10	0x25
#define SBC_CMND_READ_CAPACITY_16	0x9E

struct sbc_cmnd_read_capacity {
	scsi_lba_t lba;
	uint8_t pmi;
#define SBC_READ_CAPACITY_10_PMI_IDX	8
#define SBC_READ_CAPACITY_10_PMI_MSK	0x01
#define SBC_READ_CAPACITY_10_PMI_OFF	0
};

struct sbc_cmnd_read {
	scsi_lba_t lba;
	uint8_t rdprotect;
	uint8_t flags;
	uint8_t dpo;
	uint8_t fua;
	uint8_t fua_nv;
	uint8_t group_no;
#define SBC_READ_10_GROUP_NUMBER_IDX	6
#define SBC_READ_10_GROUP_NUMBER_MSK	0x1F
#define SBC_READ_10_GROUP_NUMBER_OFF	0
#define SBC_READ_10_RDPROTECT_IDX	1
#define SBC_READ_10_RDPROTECT_MSK	0xE0
#define SBC_READ_10_RDPROTECT_OFF	0x05
#define SBC_READ_10_DPO_IDX		1
#define SBC_READ_10_DPO_MSK		0x10
#define SBC_READ_10_DPO_OFF		4
#define SBC_READ_10_FUA_IDX		1
#define SBC_READ_10_FUA_MSK		0x08
#define SBC_READ_10_FUA_OFF		3
#define SBC_READ_10_FUA_NV_IDX		1
#define SBC_READ_10_FUA_NV_MSK		0x02
#define SBC_READ_10_FUA_NV_OFF		1

	scsi_lba_t cache_lba;
	boolean cache_valid;
};

struct sbc_cmnd_write {
	scsi_lba_t lba;
	uint8_t rdprotect;
	uint8_t flags;
	uint8_t dpo;
	uint8_t fua;
	uint8_t fua_nv;
	uint8_t ebp;
	uint8_t group_no;
#define SBC_WRITE_10_DPO_IDX		1
#define SBC_WRITE_10_DPO_MSK		0x10
#define SBC_WRITE_10_DPO_OFF		4
#define SBC_WRITE_10_FUA_IDX		1
#define SBC_WRITE_10_FUA_MSK		0x08
#define SBC_WRITE_10_FUA_OFF		3
#define SBC_WRITE_10_EBP_IDX		1
#define SBC_WRITE_10_EBP_MSK		0x04
#define SBC_WRITE_10_EBP_OFF		2

	scsi_lba_t cache_lba;
	boolean cache_valid;
};
#define SBC_MODE_PAGE_CACHING		0x08
#define SBC_MODE_PAGE_CACHING_LEN	0x14

typedef uint8_t sbc_lun_t;

#ifdef CONFIG_SBC_MAX_UNITS
#define NR_SBC_UNITS	CONFIG_SBC_MAX_UNITS
#else
#define NR_SBC_UNITS	1
#endif
#define INVALID_SBC_LUN	NR_SBC_UNITS

struct sbc_device {
	uint8_t flags;
	bulk_size_t block_size;
	scsi_lba_t nr_blocks;

	uint8_t (*read_byte)(void);
	void (*write_byte)(uint8_t);
	void (*open_block)(struct sbc_device *dev, uint8_t type,
			   scsi_lba_t lba, scsi_lbs_t blks);
	void (*close_block)(struct sbc_device *dev);
};

sbc_lun_t sbc_register_device(struct sbc_device *dev);
sbc_lun_t sbc_current_unit(void);

#ifdef CONFIG_SCSI_SBC
void scsi_sbc_init(void);
#else
#define scsi_sbc_init()
#endif

#endif /* __SCSI_SBC_H_INCLUDE__ */
