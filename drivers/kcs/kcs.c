#include <target/kcs.h>

static uint8_t kcs_state = KCS_IDLE_STATE;
static uint8_t kcs_event = 0;

uint8_t txn_write_buf[8];
uint8_t txn_read_buf[8];
uint8_t txn_read_idx;
uint8_t txn_write_idx;

void kcs_handle_event(void);

void kcs_obf_set_index(uint8_t idx)
{
	txn_write_idx = idx;
	if (idx == 0)
		kcs_hw_clear_status(KCS_OBF);
}

void kcs_obf_write(void)
{
	if (txn_write_idx > 0) {
		kcs_hw_write_obf(txn_write_buf[txn_write_idx]);
		kcs_obf_set_index(txn_write_idx - 1);
	}
}

void kcs_ibf_set_index(uint8_t idx)
{
	txn_read_idx = idx;
	if (idx == 0)
		kcs_hw_clear_status(KCS_OBF);
}

void kcs_ibf_read(void)
{
	if (txn_read_idx < 8) {
		txn_read_buf[txn_read_idx] = kcs_hw_read_dat();
		kcs_ibf_set_index(txn_read_idx+1);
	} else {
		kcs_enter_state(KCS_ERROR_STATE);
	}
}

void kcs_enter_state(uint8_t state)
{
	kcs_state = state;
	if (state == KCS_IDLE_STATE) {
		txn_read_idx = 0;
		txn_write_idx = 0;
	}
	if (state == KCS_WRITE_STATE)
		kcs_ibf_read();
	if (state == KCS_READ_STATE)
		kcs_obf_write();
	kcs_hw_write_state(kcs_state);

}

void kcs_raise_event(uint8_t event)
{
	kcs_event |= event;
	kcs_handle_event();
}

void kcs_handle_transaction(void)
{
	if (txn_read_idx > 0) {
		uint8_t len = txn_read_idx;

		while (len) {
			kcs_obf_write();
			len--;
		}
	} else
		kcs_obf_write();
}

void kcs_read_cmd(void)
{
	uint8_t cmd = kcs_hw_read_cmd();

	switch (cmd) {
		case KCS_GET_STATUS:
			kcs_enter_state(KCS_IDLE_STATE);
			break;
		case KCS_WRITE_START:
			kcs_enter_state(KCS_WRITE_STATE);
			break;
		case KCS_WRITE_END:
			kcs_ibf_read();
			kcs_enter_state(KCS_IDLE_STATE);
			kcs_handle_transaction();
			break;
		case KCS_READ:
			kcs_enter_state(KCS_READ_STATE);
			break;
	}
}
void kcs_handle_event(void)
{
	uint8_t event = kcs_event;
	switch (kcs_state) {
		case KCS_IDLE_STATE:
		if (event & KCS_EVENT_IBF)
			kcs_read_cmd();
	}
}

void kcs_handler(void) {
	// while (1) {
	// 	kcs_hw_slave();
	// }
}