#include "pcsc_priv.h"

LIST_HEAD(cards);

static struct pcsc_card *g_pcsc_icc = NULL;
struct pcsc_atr g_pcsc_atr;

#define for_each_card(c)	\
	list_for_each_entry(struct pcsc_card, c, &cards, link)

#define for_each_slot(c)	\
	list_for_each_entry(struct pcsc_slot, c, &slots, link)

void pcsc_probe_card(uint8_t *atr, int len)
{
	struct pcsc_card *card;
	
	for_each_card(card) {
		if (card->match)
			if (card->match(atr, len)) {
				g_pcsc_icc = card;
				return;
			}
	}
	return;
}

int pcsc_build_apdu(uint8_t *buf, uint8_t type, int *ne)
{
	if (g_pcsc_icc && g_pcsc_icc->build)
		return g_pcsc_icc->build(buf, type, ne);
	return 0;
}

int pcsc_select_file(uint8_t *apdu, uint16_t fid)
{
	if (g_pcsc_icc && g_pcsc_icc->select_file)
		return g_pcsc_icc->select_file(apdu, fid);
	return 0;
}

int pcsc_read_binary(uint8_t *apdu, int length)
{
	if (g_pcsc_icc && g_pcsc_icc->read_binary)
		return g_pcsc_icc->read_binary(apdu, length);
	return 0;
}

int pcsc_pin_verify(struct ccid_pv_param *pv, uint8_t *apdu)
{
	if (g_pcsc_icc && g_pcsc_icc->pin_verify)
		return g_pcsc_icc->pin_verify(pv, apdu);
	return 0;
}

int pcsc_pin_modify(struct ccid_pm_param *pm, uint8_t *apdu)
{
	if (g_pcsc_icc && g_pcsc_icc->pin_modify)
		return g_pcsc_icc->pin_modify(pm, apdu);
	return 0;
}

struct pcsc_card *pcsc_card_by_name(const char *name)
{
	if (name) {
		struct pcsc_card *c;
		for_each_card(c)
			if (!strcasecmp(name, c->name))
				return c;
	}
	return NULL;
}

int pcsc_register_card(struct pcsc_card *card)
{
	if (NULL == pcsc_card_by_name(card->name)) {
		INIT_LIST_HEAD(&card->link);
		list_insert_before(&card->link, &cards);
	}
	return 0;
}

void pcsc_unregister_card(struct pcsc_card *card)
{
	if (card)
		list_del_init(&card->link);
}

int pcsc_parse_atr(uint8_t *atr, size_t atr_len, struct pcsc_atr *info)
{
	uint16_t p;
	uint8_t K, TCK;	/* MSN of T0/Check Sum */
	uint8_t Y1i, T;	/* MSN/LSN of TDi */
	int i = 1;	/* value of the index in TAi, TBi, etc. */

	p = K = TCK = Y1i = T = 0;

	memset(info, 0, sizeof (struct pcsc_atr));

	info->default_proto = SCD_PROTOCOL_NONE;
	info->supported_protos = SCD_PROTOCOL_NONE;

	if (atr_len < 2)
		return -1;

	if (atr[0] != 0x3F && atr[0] != 0x3B)
		return -1;

	if (atr[0] == 0x3F)
		info->TS = ATR_TS_INVERSE;
	else
		info->TS = ATR_TS_DIRECT;

	Y1i = atr[1] >> 4;
	K = atr[1] & 0x0F;

	p = 2;

	do {
		int TAi, TBi, TCi, TDi;

		TAi = (Y1i & 0x01) ? atr[p++] : -1;
		TBi = (Y1i & 0x02) ? atr[p++] : -1;
		TCi = (Y1i & 0x04) ? atr[p++] : -1;
		TDi = (Y1i & 0x08) ? atr[p++] : -1;

#define SET_IB(ch)\
	do {\
		if (T##ch##i == -1) {\
			info->ib[i][ATR_INTERFACE_BYTE_T##ch].present = 0;\
		} else {\
			info->ib[i][ATR_INTERFACE_BYTE_T##ch].present = 1;\
			info->ib[i][ATR_INTERFACE_BYTE_T##ch].value = T##ch##i;\
		}\
	} while (0)

		SET_IB(A);
		SET_IB(B);
		SET_IB(C);
		SET_IB(D);

		if (TDi >= 0) {
			Y1i = TDi >> 4;
			T = TDi & 0x0F;

			if (info->default_proto == SCD_PROTOCOL_NONE) {
				switch (T) {
				case 0: 
					info->default_proto = SCD_PROTOCOL_T0;
					break;
				case 1:
					info->default_proto = SCD_PROTOCOL_T1;
					break;
				default:
					return -1;
				}
			}

			if (T == 0) {
				info->supported_protos |= SCD_PROTOCOL_T0;
			} else if (T == 1) {
				info->supported_protos |= SCD_PROTOCOL_T1;
			} else if (T == 15) {
				log_msg(LOG_INFO, "TODO: T15 found, handle class");
				info->supported_protos |= SCD_PROTOCOL_T15;
			} else {
				/* Do nothing for now since other protocols are not 
				 * supported at this time. */
			}

			/* TODO: get protocol 1st value */
#if 0
			if ((info->supported_protos & SCD_PROTOCOL_T1) &&
			    (g_t1_crc == CCID_T1_CRC_INVALID) &&
			    info->ib[i][ATR_INTERFACE_BYTE_TC].present) {
				if (info->ib[i][ATR_INTERFACE_BYTE_TC].value & 0x01)
					g_t1_crc = CCID_T1_CRC;
				else
					g_t1_crc = CCID_T1_LRC;
			}
#endif
			if ((i == 2) && (TAi >= 0)) {
				T = TAi & 0x0F;
				/* not fix for 7816-3 */
				switch (T) {
				case 0:
					info->default_proto = 
						info->supported_protos = 
						SCD_PROTOCOL_T0;
					break;
				case 1:
					info->default_proto =
						info->supported_protos = 
						SCD_PROTOCOL_T1;
					break;
				default:
					return	-1;
				}
			}
		} else {
			Y1i = 0;
		}


		i++;
	} while (Y1i != 0);
	
	if (info->default_proto == SCD_PROTOCOL_NONE) {
		info->default_proto = SCD_PROTOCOL_T0;
		info->supported_protos |= SCD_PROTOCOL_T0;
	}

	info->historical_len = K;
	memcpy(info->historical_bytes, &atr[p], min(K, ATR_MAX_HISTORICAL));
	p += K;

	if (info->supported_protos & SCD_PROTOCOL_T1)
		TCK = atr[p++];
	pcsc_probe_card(atr, atr_len);
	return 0;
}

int pcsc_init(void)
{
	pcsc_acos5_init();
	pcsc_tcos_init();
	return 0;
}

void pcsc_exit(void)
{
	pcsc_tcos_exit();
	pcsc_acos5_exit();
}
