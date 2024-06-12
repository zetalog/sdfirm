#include <target/kcs.h>

uint8_t kcs_phase;

#define kcs_wait_ibf_0()			while (lpc_io_read8(KCS_STATUS) & KCS_IBF)
#define kcs_wait_obf_1()			while (!(lpc_io_read8(KCS_STATUS) & KCS_OBF))
#define kcs_clear_ibf()				do {} while (0)
#define kcs_clear_obf()				do {} while (0)

int kcs_write(uint8_t *data, uint8_t len)
{
	kcs_wait_ibf_0();
	kcs_clear_obf();
	lpc_io_write8(KCS_WRITE_START, KCS_COMMAND);
	kcs_phase = KCS_WRITE_START;
	if ((lpc_io_read8(KCS_STATUS) & KCS_STATE_MASK) != KCS_WRITE_STATE)
		return -1;
	kcs_clear_obf();
	//kcs_phase = KCS_WRITE_DATA;
	len--;
	while (len) {
		lpc_io_write8(data[len], KCS_DATA_IN);
		kcs_wait_ibf_0();
		if ((lpc_io_read8(KCS_STATUS) & KCS_STATE_MASK) != KCS_WRITE_STATE)
			return -1;
		kcs_clear_obf();
		len--;
	}
	lpc_io_write8(KCS_WRITE_END, KCS_COMMAND);
	kcs_phase = KCS_WRITE_END;
	kcs_wait_ibf_0();
	if ((lpc_io_read8(KCS_STATUS) & KCS_STATE_MASK) != KCS_WRITE_STATE)
		return -1;
	kcs_clear_obf();
	lpc_io_write8(data[len], KCS_DATA_IN);
	kcs_phase = KCS_READ;
	return len;
}

int kcs_read(uint8_t *data)
{
	uint8_t len = 0;
	while (1) {
		kcs_wait_ibf_0();
		if ((lpc_io_read8(KCS_STATUS) & KCS_STATE_MASK) != KCS_READ_STATE) {
			if ((lpc_io_read8(KCS_STATUS) & KCS_STATE_MASK) == KCS_IDLE_STATE)
				break;
			else
				return -1;
		}
		kcs_wait_obf_1();
		data[len] = lpc_io_read8(KCS_DATA_OUT);
		lpc_io_write(data[len], KCS_DATA_IN);
		len++;
	}
	kcs_wait_obf_1();
	data[len + 1] = lpc_io_read8(KCS_DATA_OUT);
	//kcs_phase = KCS_IDLE;
	return len;
}

uint8_t kcs_buf[KCS_MAX_LEN];

int do_kcs_read()
{
	int ret;

	ret = kcs_read(kcs_buf);
	if (ret == -1) {
		printf("KCS read error!");
		return -1;
	}
	return 0;
}

int do_kcs_write()
{
	int ret;

	if (len >= KCS_MAX_LEN) {
		printf("length oversized!");
		return -1;
	}
	if (len > argc - 2) {
		printf("length not match!")
		return -1;
	}
	for (i = 0; i < len; i++) {
		kcs_buf[i]  = (uint8_t)strtoull(argv[]);
	}
	ret = kcs_write(kcs_buf, len);
	if (ret) {
		printf("KCS write error!");
		return -1;
	}
	return 0;
}

int do_kcs(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_kcs_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_kcs_write(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(kcs, do_kcs, "Keyboard Controller Style (KCS) Master Commands",
	"kcs read\n"
	"kcs write <len> <byte1> [byte2] ... [byteN]\n"
);
