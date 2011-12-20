#ifndef __SCSI_TARGET_H_INCLUDE__
#define __SCSI_TARGET_H_INCLUDE__

#include <target/bulk.h>

/* sense data descriptor */
typedef scsi_crn_t scsi_sdd_t;

#ifdef CONFIG_SCSI_MAX_UNITS
#define NR_SCSI_UNITS		CONFIG_SCSI_MAX_UNITS
#else
#define NR_SCSI_UNITS		1
#endif
#define INVALID_SCSI_LUN	NR_SCSI_UNITS

#ifdef CONFIG_SCSI_MAX_PORTS
#define NR_SCSI_PORTS		CONFIG_SCSI_MAX_PORTS
#else
#define NR_SCSI_PORTS		1
#endif
#define INVALID_SCSI_PID	NR_SCSI_PORTS

#ifdef CONFIG_SCSI_MAX_TASKS
#define NR_SCSI_TASKS		CONFIG_SCSI_MAX_TASKS
#else
#define NR_SCSI_TASKS		1
#endif
#define INVALID_SCSI_TAG	NR_SCSI_TASKS

#ifdef CONFIG_SCSI_MAX_CMNDS
#define NR_SCSI_CMNDS		CONFIG_SCSI_MAX_CMNDS
#else
#define NR_SCSI_CMNDS		1
#endif
#define INVALID_SCSI_CRN	NR_SCSI_CMNDS

#define NR_SCSI_SENSES		NR_SCSI_CMNDS+1
#define INVALID_SCSI_SDD	NR_SCSI_SENSES

#ifdef CONFIG_SCSI_MAX_BUFFER
#define SCSI_MAX_BUFFER		CONFIG_SCSI_MAX_BUFFER
#else
#define SCSI_MAX_BUFFER		256
#endif

#ifdef CONFIG_SCSI_MAX_MODES
#define NR_SCSI_MODES			CONFIG_SCSI_MAX_MODES
#else
#define NR_SCSI_MODES			1
#endif
#ifdef CONFIG_SCSI_MAX_PAGES
#define SCSI_MAX_PAGES			CONFIG_SCSI_MAX_PAGES
#else
#define SCSI_MAX_PAGES			64
#endif

typedef void (*scsi_cmpl_cb)(void);

struct scsi_device {
	uint8_t type;
	text_char_t *name;
	uint16_t ver_desc;
	uint32_t flags;
#define SCSI_DEVICE_RMB		0x01

	uint8_t (*inquiry)(uint8_t page_code, uint8_t offset);
	void (*parse_cdb)(uint8_t *cdb, uint8_t len);
	void (*cmnd_aval)(void);
	void (*cmnd_cmpl)(void);
	boolean (*test_ready)(void);
};
typedef struct scsi_device scsi_device_t;

struct scsi_transport {
	uint8_t proto;
	uint16_t phys_ver_desc;
	uint16_t xprt_ver_desc;

	uint8_t (*inquiry)(uint8_t page_code, uint8_t offset);
	void (*cmnd_cmpl)(void);

	/* bulk IO transports */
	uint8_t (*bulk_type)(void);
	bulk_size_t (*bulk_size)(uint8_t type);

	/* char transfer APIs */
	bulk_open_cb open;
	bulk_close_cb close;
	bulk_write_cb byte_in;
	bulk_read_cb byte_out;

	/* block transfer APIs */
	bulk_xmit_cb data_in;
	bulk_xmit_cb data_out;
	bulk_space_cb bulk_idle;
};
typedef struct scsi_transport scsi_transport_t;

struct scsi_vpd_page {
	uint16_t (*page_size)(uint8_t page_code);
	void (*page_data)(uint8_t page_code);
};
typedef struct scsi_vpd_page scsi_vpd_page_t;

struct scsi_ident_desc {
	uint8_t pid : 4;
#define SCSI_PROTO_FCP		0x00
#define SCSI_PROTO_SPI		0x01
#define SCSI_PROTO_SSA		0x02
#define SCSI_PROTO_SBP		0x03
#define SCSI_PROTO_SRP		0x04
#define SCSI_PROTO_ISCSI	0x05
#define SCSI_PROTO_SAS		0x06
#define SCSI_PROTO_ADT		0x07
#define SCSI_PROTO_AT		0x08

	uint8_t code_set : 4;
#define SCSI_CODE_SET_BINARY		0x01
#define SCSI_CODE_SET_ASCII		0x02
#define SCSI_CODE_SET_UINT8		0x03

	uint8_t assoc : 2;
#define SCSI_ASSOC_LOGICAL_UNIT		0x00
#define SCSI_ASSOC_TARGET_PORT		0x10
#define SCSI_ASSOC_TARGET_DEVICE	0x20

	uint8_t type : 4;
#define SCSI_IDENT_TYPE_VENDOR			0x00
#define SCSI_IDENT_TYPE_T10_VID			0x01
#define SCSI_IDENT_TYPE_EUI64			0x02
#define SCSI_IDENT_TYPE_NAA			0x03
#define SCSI_IDENT_TYPE_REL_PID			0x04
#define SCSI_IDENT_TYPE_TPG			0x05
#define SCSI_IDENT_TYPE_LUG			0x06
#define SCSI_IDENT_TYPE_MD5_LUI			0x07
#define SCSI_IDENT_TYPE_STRING			0x08

#define SCSI_NAA_IEEE_EXT			0x02
#define SCSI_NAA_IEEE_REG			0x05
#define SCSI_NAA_IEEE_REG_EXT			0x06

	uint8_t (*desc_size)(void);
	void (*desc_data)(void);
};
typedef struct scsi_ident_desc scsi_ident_desc_t;

/* INQUIRY */
#ifdef CONFIG_SCSI_RMB
#define SCSI_TARGET_RMB			SCSI_INQUIRY_RMB
#else
#define SCSI_TARGET_RMB			0x00
#endif
#ifdef CONFIG_SCSI_NACA
#define SCSI_TARGET_NORMACA		SCSI_INQUIRY_NORMACA
#else
#define SCSI_TARGET_NORMACA		0x00
#endif
#ifdef CONFIG_SCSI_HISUP
#define SCSI_TARGET_HISUP		SCSI_INQUIRY_HISUP
#else
#define SCSI_TARGET_HISUP		0x00
#endif
#ifdef CONFIG_SCSI_STD_ACC
#define SCSI_TARGET_ACC			SCSI_INQUIRY_ACC
#else
#define SCSI_TARGET_ACC			0x00
#endif
#define SCSI_TARGET_TPGS		SCSI_INQUIRY_TPGS
#ifdef CONFIG_SCSI_3PC
#define SCSI_TARGET_3PC			SCSI_INQUIRY_3PC
#else
#define SCSI_TARGET_3PC			0x00
#endif
#ifdef CONFIG_SCSI_BQUE
#define SCSI_TARGET_BQUE		SCSI_INQUIRY_BQUE
#else
#define SCSI_TARGET_BQUE		0x00
#endif
#if NR_SCSI_PORTS > 1
#define SCSI_TARGET_MULTIP		SCSI_INQUIRY_MULTIP
#else
#define SCSI_TARGET_MULTIP		0x00
#endif
#ifdef CONFIG_SCSI_LINK
#define SCSI_TARGET_LINKED		SCSI_INQUIRY_LINKED
#else
#define SCSI_TARGET_LINKED		0x00
#endif
#ifdef CONFIG_SCSI_CMDQUE
#define SCSI_TARGET_CMDQUE		SCSI_INQUIRY_CMDQUE
#else
#define SCSI_TARGET_CMDQUE		0x00
#endif
#ifdef CONFIG_SCSI_T10_REVISION
#define SCSI_TARGET_PRODUCT_REV		((uint32_t)CONFIG_SCSI_T10_REVISION)
#else
#define SCSI_TARGET_PRODUCT_REV		((uint32_t)0)
#endif
#ifdef CONFIG_SCSI_PRIOR
#define SCSI_TARGET_PRIOR_SUP		SCSI_INQUIRY_EXT_PRIOR_SUP
#else
#define SCSI_TARGET_PRIOR_SUP		0x00
#endif
#define SCSI_TARGET_SAM			SCSI_VER_DESC_SAM3
#define SCSI_TARGET_SPC			SCSI_VER_DESC_SPC3

struct scsi_cmnd {
	scsi_crn_t crn;
	bulk_cid_t bulk;
	scsi_nexus_t nexus;

	uint8_t dir;
#define SCSI_CMND_OUT		0x00
#define SCSI_CMND_IN		0x01

	/* CDB fields */
	uint8_t opcode;
	uint8_t control;
	union scsi_cmnd_fields fields;
#define scsi_inquiry_cmnd		scsi_current_cmnd.fields.inquiry
#define scsi_request_sense_cmnd		scsi_current_cmnd.fields.request_sense
#define scsi_report_luns_cmnd		scsi_current_cmnd.fields.report_luns
#define scsi_mode_sense_cmnd		scsi_current_cmnd.fields.mode_sense
#define scsi_mode_select_cmnd		scsi_current_cmnd.fields.mode_select
#define scsi_prevent_allow_medium_removal_cmnd	\
					scsi_current_cmnd.fields.prevent_allow_medium_removal
	/* SBC commands */
#define sbc_read_capacity_cmnd		scsi_current_cmnd.fields.read_capacity
#define sbc_read_cmnd			scsi_current_cmnd.fields.read
#define sbc_write_cmnd			scsi_current_cmnd.fields.write

	/* ALLOCATION LENGTH/PARAMTER LIST LENGTH/TRANSFER LENGTH */
	uint32_t expect_length;
	uint8_t expect_granularity;

	uint8_t data_buffer[SCSI_MAX_BUFFER];
	uint16_t data_length;

	void (*bulk_io)(void);

	/* state machine */
	uint8_t state;
#define SCSI_CMND_IDLE		0x00
#define SCSI_CMND_BUSY		0x01
#define SCSI_CMND_AVAL		0x02
#define SCSI_CMND_CMPL		0x03
#define SCSI_CMND_DONE		0x04

	/* status */
	scsi_sdd_t sdd;
	uint8_t status;

	/* list linkage of commands in one task */
	struct scsi_cmnd *next;
};

struct scsi_task {
	scsi_tag_t tag;
	boolean linked;

	/* nexus */
	scsi_pid_t pid;
	scsi_lun_t lun;

	/* linked command support */
	struct scsi_cmnd *linked_cmnds;

	uint8_t state;
	uint8_t attr;

	struct scsi_task *next;
};

struct scsi_task_set {
	struct scsi_task *head_of_queue;
	uint8_t ACA : 1;	/* ACA condition */
	uint8_t UA : 1;		/* unit attention condition */
};

struct scsi_sense_data {
	scsi_sdd_t sdd;
	struct scsi_sense sense_data;
	struct scsi_sense_data *next;
	scsi_crn_t crn;
};

struct scsi_mode_page {
	uint8_t page_code;
	uint8_t subpage_code;
	uint8_t policy;
	uint8_t offset;
	uint8_t length;
};

struct scsi_unit {
	scsi_tag_t linked_tag;
	struct scsi_task_set task_set;
#if NR_SCSI_TASKS > 1
	struct scsi_task tasks[NR_SCSI_TASKS];
#else
	struct scsi_task task;
#endif
	struct scsi_sense_data *sense_data_queue;

#ifdef CONFIG_SCSI_MODE
	struct scsi_mode_page mode_pages[NR_SCSI_MODES];
	uint8_t nr_mode_pages;
	uint8_t free_pages_pos;
	uint8_t current_pages[SCSI_MAX_PAGES];
#ifdef CONFIG_SCSI_MODE_SAVING
	uint8_t saved_pages[SCSI_MAX_PAGES];
#endif
#endif
};

struct scsi_port {
	/* application client, initiator device identifier */
	uint8_t iid;
	/* device server, target device identifier */
	uint8_t tid;
#ifdef CONFIG_SCSI_SCSI_RPID
	scsi_pid_t rpid;
#endif
};

struct scsi_target {
	/* logical units */
	struct scsi_unit units[NR_SCSI_UNITS];
	/* target ports */
	struct scsi_port ports[NR_SCSI_PORTS];
};

#ifdef CONFIG_SCSI_TMF_ONLY
#define SCSI_CONTROL_TMF_ONLY		true
#else
#define SCSI_CONTROL_TMF_ONLY		false
#endif
#define SCSI_CONTROL_TST_SINGLE		0x000
#define SCSI_CONTROL_TST_MULTIPLE	0x001
#define SCSI_CONTROL_TST		SCSI_CONTROL_TST_MULTIPLE

#if NR_SCSI_PORTS > 1
scsi_pid_t scsi_pid;
scsi_pid_t scsi_port_save(scsi_pid_t pid);
void scsi_port_restore(scsi_pid_t pid);
#else
#define scsi_pid			0
#define scsi_port_save(pid)		0
#define scsi_port_restore(pid)
#endif
#define scsi_port_select(pid)		scsi_port_restore(pid)

extern scsi_lun_t scsi_lun;

scsi_lun_t scsi_max_units(void);
void scsi_unit_restore(scsi_lun_t lun);
scsi_lun_t scsi_unit_save(scsi_lun_t lun);
#define scsi_unit_select(lun)		scsi_unit_restore(lun)
#define scsi_target_unit		scsi_target.units[scsi_lun]
#define scsi_target_port		scsi_target.ports[scsi_pid]
#define scsi_target_device		scsi_devices[scsi_lun]
#define scsi_target_xprt		scsi_transports[scsi_pid]

#define scsi_target_task_set		scsi_target_unit.task_set
#define scsi_head_of_queue		scsi_target_task_set.head_of_queue

extern scsi_transport_t *scsi_transports[NR_SCSI_PORTS];

#if NR_SCSI_TASKS > 1
scsi_tag_t scsi_tag;
void scsi_task_restore(scsi_tag_t tag);
scsi_tag_t scsi_task_save(scsi_tag_t tag);
#define scsi_target_task		scsi_target_unit.tasks[scsi_tag]
#else
#define scsi_tag			0
#define scsi_task_save(tag)		0
#define scsi_task_restore(tag)
#define scsi_target_task		scsi_target_unit.task
#endif
#define scsi_task_select(tag)		scsi_task_restore(tag)

#if NR_SCSI_CMNDS > 1
extern struct scsi_cmnd scsi_cmnds[NR_SCSI_CMNDS];
scsi_crn_t scsi_crn;
void scsi_cmnd_restore(scsi_crn_t crn);
scsi_crn_t scsi_cmnd_save(scsi_crn_t crn);
#define scsi_current_cmnd		scsi_cmnds[scsi_crn]
#else
extern struct scsi_cmnd scsi_cmnd;
#define scsi_crn			0
#define scsi_cmnd_save(cmd)		0
#define scsi_cmnd_restore(cmd)
#define scsi_current_cmnd		scsi_cmnd
#endif
#define scsi_recent_cmnd		scsi_target_task.linked_cmnds
#define scsi_cmnd_select(cmd)		scsi_cmnd_restore(cmd)

extern struct scsi_sense_data scsi_senses[NR_SCSI_SENSES];
scsi_sdd_t scsi_sdd;
void scsi_sense_data_restore(scsi_sdd_t sdd);
scsi_sdd_t scsi_sense_data_save(scsi_sdd_t sense);
#define scsi_current_sense_data		scsi_senses[scsi_sdd]
#define scsi_recent_sense_data		scsi_target_unit.sense_data_queue
#define scsi_sense_data_select(sdd)	scsi_sense_data_restore(sdd)

#ifdef CONFIG_SCSI_MAX_VPDS
#define NR_SCSI_VPDS			CONFIG_SCSI_MAX_VPDS
#else
#define NR_SCSI_VPDS			2
#endif
void scsi_register_vpd_page(uint8_t page_code, scsi_vpd_page_t *vpd);
scsi_vpd_page_t *scsi_vpd_page_by_code(uint8_t code);

#ifdef CONFIG_SCSI_VPD_ASCII
#ifdef CONFIG_SCSI_MAX_ASCIIS
#define NR_SCSI_ASCIIS			CONFIG_SCSI_MAX_ASCIIS
#else
#define NR_SCSI_ASCIIS			0
#endif
uint8_t scsi_register_ascii_info(text_char_t *ascii_info);
#else
#define scsi_register_ascii_info(ascii_info)	0
#endif

#ifdef CONFIG_SCSI_MODE
void scsi_allocate_mode_page(scsi_lun_t lun, uint8_t length,
			     uint8_t page_code, uint8_t subpage_code,
			     uint8_t policy);
void scsi_saving_not_supported(void);
#else
#define scsi_allocate_mode_page(lun, length, page_code, subpage_code, policy)
#endif

/* called by commands implentations */
void scsi_command_aval(uint8_t dir);
void scsi_command_cmpl(void);
void scsi_command_done(uint8_t status);
void scsi_command_aval_in(void (*data_in)(void));
void scsi_command_aval_out(void (*data_out)(void));

/* called by SCSI devices for default IO */
uint8_t scsi_def_readb(uint16_t offset);
void scsi_def_writeb(uint8_t byte);
void scsi_def_writew(uint16_t word);
void scsi_def_writel(uint32_t dword);
void scsi_def_data_in(void);
void scsi_def_data_out(void);

/* register a storage device type */
scsi_lun_t scsi_register_device(scsi_device_t *dev);

void scsi_illegal_request(uint8_t asc);
void scsi_invalid_cmd_opcode(void);
void scsi_invalid_cdb_field(void);
void scsi_invalid_msg_error(void);
void scsi_lack_task_resource(void);

/* scsi_command_validate: task router function, called by transport
 *                         protocol to route received commands.
 * @iid: initiator device identifier
 * @tid: target port identifier
 * @lun: logical unit number
 * @cdb: command descriptor block buffer
 * @len: command descriptor block length
 * @attr: task attribute
 */
scsi_crn_t scsi_command_validate(uint8_t iid, uint8_t tid,
				 scsi_pid_t pid, scsi_lun_t lun,
				 uint8_t *cdb, uint8_t len, uint8_t attr);
boolean scsi_command_schedule(scsi_crn_t crn, boolean sync);
void scsi_command_complete(scsi_crn_t crn);
scsi_lbs_t scsi_command_expect(scsi_crn_t crn);
uint8_t scsi_command_status(scsi_crn_t crn);
void scsi_command_bulkio(scsi_crn_t crn);

void scsi_implicit_head_of_queue(void);

scsi_pid_t scsi_register_transport(scsi_transport_t *xpt);

uint8_t scsi_xprt_specific_inquiry(uint8_t page_code);
uint8_t scsi_device_specific_inquiry(uint8_t page_code);
void scsi_device_specific_parse(uint8_t *cdb, uint8_t len);
void scsi_device_specific_aval(void);
void scsi_device_specific_cmpl(void);

struct scsi_cmnd *scsi_cmnd_by_id(scsi_crn_t crn);
struct scsi_task *scsi_task_by_id(scsi_tag_t tag);

/* Event Notification SCSI Transport Protocol Services */
void scsi_nexus_loss(scsi_nexus_t nexus);
void scsi_transport_reset(scsi_pid_t pid);

#ifndef CONFIG_SCSI_INITIATOR
#define scsi_target_dev(pid)	pid
#endif

void scsi_set_status(uint8_t status);
void scsi_set_direction(uint8_t dir);
uint8_t scsi_get_direction(void);
uint8_t scsi_get_status(void);
void scsi_set_sense_flag(uint8_t flag, boolean on);
void scsi_set_sense_spec(uint32_t spec);
void scsi_set_sense_fruc(uint8_t fruc);
void scsi_clear_sense_flags(void);
void scsi_set_sense_code(uint8_t key, uint8_t asc, uint8_t ascq);
uint8_t scsi_get_sense_key(void);
uint8_t scsi_get_sense_asc(void);
uint8_t scsi_get_sense_ascq(void);
uint8_t scsi_get_sense_fruc(void);
uint32_t scsi_get_sense_spec(void);
uint8_t scsi_get_sense_flag(uint8_t flag);

#ifdef CONFIG_SCSI_TARGET
void scsi_target_init(void);
#else
#define scsi_target_init()
#endif

#endif
