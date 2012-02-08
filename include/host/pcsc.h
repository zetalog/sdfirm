#ifndef __PCSC_H_INCLUDE__
#define __PCSC_H_INCLUDE__

#include <host/list.h>
#include <host/ccid.h>

#define CCID_T1_CRC		0x01
#define CCID_T1_LRC		0x00

#define	ATR_TS_DIRECT			0x00
#define	ATR_TS_INVERSE			0x01

/* Paramenters */
#define ATR_MAX_SIZE 		33	/* Maximum size of ATR byte array */
#define ATR_MAX_HISTORICAL	15	/* Maximum number of historical bytes */
#define ATR_MAX_PROTOCOLS	7	/* Maximun number of protocols */
#define ATR_MAX_IB		4	/* Maximum number of interface bytes per protocol */
#define ATR_INTERFACE_BYTE_TA	0	/* Interface byte TAi */
#define ATR_INTERFACE_BYTE_TB	1	/* Interface byte TBi */
#define ATR_INTERFACE_BYTE_TC	2	/* Interface byte TCi */
#define ATR_INTERFACE_BYTE_TD	3	/* Interface byte TDi */

/* Default values for paramenters */
#define ATR_DEFAULT_FD		0x11
#define ATR_DEFAULT_N		0
#define ATR_DEFAULT_IFSC	32		/* T=1 1st TA */
#define ATR_DEFAULT_CWI		13		/* T=1 1st high TB */
#define ATR_DEFAULT_BWI		4		/* T=1 1st low  TB */
#define ATR_DEFAULT_CRC		CCID_T1_LRC	/* T=1 1st TC */

struct pcsc_card {
	uint8_t type;
#define PCSC_CARD_ACS_ACOS5	0x02
#define PCSC_CARD_WD_TCOS	0x01
	char *name;

	/* match card */
	boolean (*match)(uint8_t *atr, int len);
	/* build special apdu by AT */
	int (*build)(uint8_t *apdu, uint8_t type, int *ne);
	int (*select_file)(uint8_t *apdu, uint16_t fid);
	int (*read_binary)(uint8_t *apdu, int length);
	int (*pin_verify)(struct ccid_pv_param *pv, uint8_t *apdu);
	int (*pin_modify)(struct ccid_pm_param *pm, uint8_t *apdu);

	struct list_head link;
};

struct pcsc_atr {
	uint8_t TS;
	uint8_t T0;
	uint8_t historical_len;
	uint8_t historical_bytes[ATR_MAX_HISTORICAL];

	uint8_t supported_protos;
	uint8_t default_proto;
	struct {
		uint8_t value;
		uint8_t present;
	} ib[ATR_MAX_PROTOCOLS][ATR_MAX_IB], TCK;
};

struct pcsc_cmds {
	uint8_t type;
	/* apdu type */
#define AT_SELECT_FILE	0x01
#define AT_READ_BINARY	0x02
#define AT_PIN_VERIFY	0x03
#define AT_PIN_MODIFY	0x04
#define AT_XFR_BLOCK	0x05
#define AT_INVALID	0xFF
	uint8_t cmd[261];
	int len;
	int ne;
};

struct atr_table {
	uint8_t atr[33];
};

#ifndef min
#define min(x, y)	((x)<(y)?(x):(y))
#define max(x, y)	((x)>(y)?(x):(y))
#endif

#define SCS_PROTO_T0		0x00
#define SCS_PROTO_T1		0x01
#define SCS_PROTO_T15		0x0F
#define IFD_HEADER_SIZE		5
#define IFD_BUF_SIZE		256+IFD_HEADER_SIZE

int pcsc_select_file(uint8_t *buf, uint16_t fid);
int pcsc_read_binary(uint8_t *apdu, int length);
int pcsc_pin_verify(struct ccid_pv_param *pv, uint8_t *apdu);
int pcsc_pin_modify(struct ccid_pm_param *pm, uint8_t *apdu);

int pcsc_register_card(struct pcsc_card *card);
void pcsc_unregister_card(struct pcsc_card *card);

int pcsc_parse_atr(uint8_t *atr, size_t atr_len, struct pcsc_atr *info);
int pcsc_build_apdu(uint8_t *buf, uint8_t type, int *ne);

int pcsc_acos5_init(void);
void pcsc_acos5_exit(void);
int pcsc_tcos_init(void);
void pcsc_tcos_exit(void);

int pcsc_init(void);
void pcsc_exit(void);

extern struct pcsc_atr g_pcsc_atr;

#endif /* __PCSC_H_INCLUDE__ */
