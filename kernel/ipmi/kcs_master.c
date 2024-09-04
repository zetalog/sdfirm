#include <target/kcs.h>
#include <target/cmdline.h>
#include <target/barrier.h>
#include <target/lpc.h>
#include <target/console.h>

uint8_t kcs_phase;

#ifndef KCS_MAX_LEN
#define KCS_MAX_LEN				64
#endif

#define kcs_wait_ibf_0()			while (lpc_io_read8(KCS_STATUS) & KCS_IBF)
#define kcs_wait_obf_1()			while (!(lpc_io_read8(KCS_STATUS) & KCS_OBF))
#define kcs_clear_obf()						\
	do {							\
		__unused uint8_t drop;				\
		while (lpc_io_read8(KCS_STATUS) & KCS_OBF)	\
			drop = lpc_io_read8(KCS_DATA_IN);	\
	} while (0)

bool kcs_is_state(uint8_t state)
{
	uint8_t status = lpc_io_read8(KCS_STATUS);
	return (status & KCS_STATE_MASK) == state;
}

int kcs_write(uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	kcs_wait_ibf_0();
	kcs_clear_obf();
	lpc_io_write8(KCS_WRITE_START, KCS_COMMAND);
	kcs_wait_ibf_0();
	if (!kcs_is_state(KCS_WRITE_STATE))
		return -i;
	kcs_clear_obf();
	while (i < (len - 1)) {
		lpc_io_write8(data[i], KCS_DATA_IN);
		kcs_wait_ibf_0();
		if (!kcs_is_state(KCS_WRITE_STATE))
			return -i;
		kcs_clear_obf();
		i++;
	}
	lpc_io_write8(KCS_WRITE_END, KCS_COMMAND);
	kcs_wait_ibf_0();
	if (!kcs_is_state(KCS_WRITE_STATE))
		return -i;
	kcs_clear_obf();
	lpc_io_write8(data[i], KCS_DATA_IN);
	return len;
}

int kcs_read(uint8_t *data, uint8_t len)
{
	uint8_t i = 0;

	while (i < (len - 1)) {
		kcs_wait_ibf_0();
		if (!kcs_is_state(KCS_READ_STATE)) {
			if (kcs_is_state(KCS_IDLE_STATE)) {
				kcs_wait_obf_1();
				data[i + 1] = lpc_io_read8(KCS_DATA_OUT);
				return i;
			}
			else
				return -i;
		}
		kcs_wait_obf_1();
		data[i] = lpc_io_read8(KCS_DATA_OUT);
		lpc_io_write8(KCS_READ, KCS_DATA_IN);
		i++;
	}
	return -len;
}

uint8_t kcs_buf[KCS_MAX_LEN];

int do_kcs_read(int argc, char *argv[])
{
	int ret;
	int len = (uint8_t)strtoull(argv[2], 0, 0);

	if (len >= KCS_MAX_LEN) {
		printf("length oversized!");
		return -1;
	}
	ret = kcs_read(kcs_buf, len + 1);
	if (ret < 0) {
		printf("KCS read error!");
		return ret;
	}
	hexdump(0, kcs_buf, 8, ret);
	return 0;
}

int do_kcs_write(int argc, char *argv[])
{
	int ret, i;
	int len = (uint8_t)strtoull(argv[2], 0, 0);

	if (len >= KCS_MAX_LEN) {
		printf("length oversized!i\n");
		return -EINVAL;
	}
	if (len > argc - 2) {
		printf("length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		kcs_buf[i]  = (uint8_t)strtoull(argv[i + 3], 0, 0);
	}
	ret = kcs_write(kcs_buf, len + 1);
	if (ret < 0) {
		printf("KCS write error!\n");
		return -ret;
	}
	return 0;
}

int do_kcs_xfer(int argc, char *argv[])
{
	int ret, i;
	int wlen, rlen;

	if (argc < 3)
		return -EINVAL;
	wlen = (uint8_t)strtoull(argv[2], 0, 0);
	if (wlen >= KCS_MAX_LEN) {
		printf("write length oversized!\n");
		return -EINVAL;
	}
	if (wlen > argc - 3) {
		printf("write length not match!\n");
		return -EINVAL;
	}
	rlen = (uint8_t)strtoull(argv[3], 0, 0);
	if (rlen >= KCS_MAX_LEN) {
		printf("read length oversized!\n");
		return -EINVAL;
	}
	for (i = 0; i < wlen; i++) {
		kcs_buf[i]  = (uint8_t)strtoull(argv[i + 4], 0, 0);
	}
	ret = kcs_write(kcs_buf, wlen + 1);
	if (ret < 0) {
		printf("KCS write error!\n");
		return -ret;
	}
	if (rlen) {
		ret = kcs_read(kcs_buf, rlen + 1);
		if (ret < 0) {
			printf("KCS read error!\n");
			return ret;
		}
		hexdump(0, kcs_buf, 8, ret);
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
	if (strcmp(argv[1], "xfer") == 0)
		return do_kcs_xfer(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(kcs, do_kcs, "Keyboard Controller Style (KCS) Master Commands",
	"kcs read <len>\n"
	"	-kcs read data\n"
	"kcs write <len> <byte1> [byte2] ... [byteN]\n"
	"	-kcs write data\n"
	"kcs xfer <wlen> <rlen> <byte1> [byte2] ... [byteN]\n"
	"	-kcs transfer data\n"
);
