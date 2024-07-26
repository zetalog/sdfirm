#define __version__ "4.5.0"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#ifdef HOSTED
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define __unused		__attribute__((__unused__))

/* Some systems don't define MAP_LOCKED.  Define it to 0 here
   so it's just a no-op when ORed with other constants. */
#ifndef MAP_LOCKED
  #define MAP_LOCKED 0
#endif
#else
#include <target/cmdline.h>
#include <target/heap.h>
#include <getopt.h>

typedef long off_t;
typedef long ptrdiff_t;
#define fprintf(f, ...)			printf(__VA_ARGS__)
#define fflush(f)			do { } while (0)
#define malloc(size)			heap_alloc(size);
#define free(ptr)			heap_free(ptr)
#endif

typedef unsigned long ul;
typedef unsigned long long ull;
typedef unsigned long volatile ulv;
typedef unsigned char volatile u8v;
typedef unsigned short volatile u16v;

#define rand32() ((unsigned int) rand() | ( (unsigned int) rand() << 16))

#if (ULONG_MAX == 4294967295UL)
#define rand_ul()		rand32()
#define UL_ONEBITS		0xffffffff
#define UL_LEN			32
#define CHECKERBOARD1		0x55555555
#define CHECKERBOARD2		0xaaaaaaaa
#define UL_BYTE(x)		((x | x << 8 | x << 16 | x << 24))
#elif (ULONG_MAX == 18446744073709551615ULL)
#define rand64()		(((ul) rand32()) << 32 | ((ul) rand32()))
#define rand_ul()		rand64()
#define UL_ONEBITS		0xffffffffffffffffUL
#define UL_LEN			64
#define CHECKERBOARD1		0x5555555555555555
#define CHECKERBOARD2		0xaaaaaaaaaaaaaaaa
#define UL_BYTE(x)		\
	(((ul)x | (ul)x<<8 | (ul)x<<16 | (ul)x<<24 | \
	  (ul)x<<32 | (ul)x<<40 | (ul)x<<48 | (ul)x<<56))
#else
#error long on this platform is not 32 or 64 bits
#endif

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04

/* Global vars - so tests have access to this information */
static int use_phys = 0;
static off_t physaddrbase = 0;

#ifdef HOSTED
/* Sanity checks and portability helper macros. */
#ifdef _SC_VERSION
static void check_posix_system(void)
{
	if (sysconf(_SC_VERSION) < 198808L) {
		fprintf(stderr,
			"A POSIX system is required. "
			"Don't be surprised if this craps out.\n");
		fprintf(stderr, "_SC_VERSION is %lu\n", sysconf(_SC_VERSION));
	}
}
#else
#define check_posix_system()		do { } while (0)
#endif

#ifdef _SC_PAGE_SIZE
static int memtester_pagesize(void)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);

	if (pagesize == -1) {
		perror("_SC_PAGE_SIZE");
		return -EXIT_FAIL_NONSTARTER;
	}
	printf("pagesize is %ld\n", (long) pagesize);
	return pagesize;
}
#else
#define memtester_pagesize()		8192
#endif

/* Device to mmap memory from with -p, default is normal core */
static const char *device_name = "/dev/mem";
static int device_specified = 0;

/* Function definitions */
static void usage(void)
{
	fprintf(stderr, "\n"
		"Usage: memtester [-p physaddrbase] [-d device] "
		"<mem>[B|K|M|G] [loops]\n");
}

static ul memtester_testmask(void)
{
	char *env_testmask = 0;
	ul testmask = 0;

	env_testmask = getenv("MEMTESTER_TEST_MASK");
	if (env_testmask) {
		errno = 0;
		testmask = strtoul(env_testmask, 0, 0);
		if (errno) {
			fprintf(stderr,
				"error parsing MEMTESTER_TEST_MASK %s: %s\n",
			env_testmask, strerror(errno));
			usage();
			return -EXIT_FAIL_NONSTARTER;
		}
		printf("using testmask 0x%lx\n", testmask);
	}
	return testmask;
}

static int memtester_parseopt(int argc, char **argv, size_t pagesize)
{
	int opt;
	char *addrsuffix;
	struct stat statbuf;

	while ((opt = getopt(argc, argv, "p:d:")) != -1) {
		switch (opt) {
		case 'p':
			errno = 0;
			physaddrbase = (off_t)strtoull(optarg, &addrsuffix, 16);
			if (errno != 0) {
				fprintf(stderr,
					"failed to parse physaddrbase arg; "
					"should be hex address (0x123...)\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			if (*addrsuffix != '\0') {
				/* got an invalid character in the address */
				fprintf(stderr,
					"failed to parse physaddrbase arg; "
					"should be hex address (0x123...)\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			if (physaddrbase & (pagesize - 1)) {
				fprintf(stderr,
					"bad physaddrbase arg; "
					"does not start on page boundary\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			/* okay, got address */
			use_phys = 1;
			break;
		case 'd':
			if (stat(optarg,&statbuf)) {
				fprintf(stderr,
					"can not use %s as device: %s\n",
					optarg, strerror(errno));
				usage();
				return -EXIT_FAIL_NONSTARTER;
			} else {
				if (!S_ISCHR(statbuf.st_mode)) {
					fprintf(stderr,
						"can not mmap non-char device %s\n",
						optarg);
					usage();
					return -EXIT_FAIL_NONSTARTER;
				} else {
					device_name = optarg;
					device_specified = 1;
				}
			}
			break;
		default: /* '?' */
			usage();
			return -EXIT_FAIL_NONSTARTER;
		}
	}

	if (device_specified && !use_phys) {
		fprintf(stderr,
			"for mem device, "
			"physaddrbase (-p) must be specified\n");
		usage();
		return -EXIT_FAIL_NONSTARTER;
	}
	return optind;
}

static int memtester_usephys(size_t wantbytes, void __unused volatile **pbuf)
{
	int memfd;
	void volatile *buf;

	if (!use_phys)
		return 0;

	memfd = open(device_name, O_RDWR | O_SYNC);
	if (memfd == -1) {
		fprintf(stderr,
			"failed to open %s for physical memory: %s\n",
			device_name, strerror(errno));
		return -EXIT_FAIL_NONSTARTER;
	}
	buf = (void volatile *)mmap(0, wantbytes, PROT_READ | PROT_WRITE,
				    MAP_SHARED | MAP_LOCKED, memfd,
				    physaddrbase);
	if (buf == MAP_FAILED) {
		fprintf(stderr,
			"failed to mmap %s for physical memory: %s\n",
			device_name, strerror(errno));
		return -EXIT_FAIL_NONSTARTER;
	}
	return 1;
}
#else
#define mlock(a, s)			0
#define munlock(a, s)			do { } while (0)
#define check_posix_system()		do { } while (0)

static void usage(void)
{
	fprintf(stderr, "\n"
		"Usage: memtester [-p physaddrbase] <mem>[B|K|M|G] [loops]\n");
}

#define memtester_pagesize()				PAGE_SIZE
#define memtester_testmask()				0

static int memtester_parseopt(int argc, char **argv, size_t pagesize)
{
	int opt;
	char *addrsuffix;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			errno = 0;
			physaddrbase = (off_t)strtoull(optarg, &addrsuffix, 16);
			if (errno != 0) {
				fprintf(stderr,
					"failed to parse physaddrbase arg; "
					"should be hex address (0x123...)\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			if (*addrsuffix != '\0') {
				/* got an invalid character in the address */
				fprintf(stderr,
					"failed to parse physaddrbase arg; "
					"should be hex address (0x123...)\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			if (physaddrbase & (pagesize - 1)) {
				fprintf(stderr,
					"bad physaddrbase arg; "
					"does not start on page boundary\n");
				usage();
				return -EXIT_FAIL_NONSTARTER;
			}
			/* okay, got address */
			use_phys = 1;
			break;
		default: /* '?' */
			usage();
			return -EXIT_FAIL_NONSTARTER;
		}
	}

	return optind;
}

static int memtester_usephys(size_t wantbytes, void volatile **pbuf)
{
	if (!use_phys)
		return 0;

	/* TODO: mmap if MMU is enabled */
	*pbuf = (void *)physaddrbase;
	return 1;
}
#endif

static void memtester_badregion(ulv *p1, ulv *p2, size_t i)
{
	off_t physaddr;

	if (use_phys) {
		physaddr = physaddrbase + (i * sizeof(ul));
		fprintf(stderr,
			"FAILURE: 0x%08lx != 0x%08lx at physical address "
			"0x%08lx.\n",
			(ul)*p1, (ul)*p2, physaddr);
	} else {
		fprintf(stderr,
			"FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n",
			(ul)*p1, (ul)*p2, (ul)(i * sizeof(ul)));
	}
}

static void memtester_badaddr(size_t i)
{
	off_t physaddr;

	if (use_phys) {
		physaddr = physaddrbase + (i * sizeof(ul));
		fprintf(stderr,
			"FAILURE: possible bad address line at physical "
			"address 0x%08lx.\n", physaddr);
	} else {
		fprintf(stderr,
			"FAILURE: possible bad address line at offset "
			"0x%08lx.\n", (ul)(i * sizeof(ul)));
	}
}

static char progress[] = "-\\|/";

#define PROGRESSLEN		4
#define PROGRESSOFTEN		2500
#define ONE			0x00000001L

union {
	unsigned char bytes[UL_LEN/8];
	ul val;
} mword8;

union {
	unsigned short u16s[UL_LEN/16];
	ul val;
} mword16;

static int compare_regions(ulv *bufa, ulv *bufb, size_t count)
{
    int r = 0;
    size_t i;
    ulv *p1 = bufa;
    ulv *p2 = bufb;

    for (i = 0; i < count; i++, p1++, p2++) {
        if (*p1 != *p2) {
            memtester_badregion(p1, p2, i);
            /* printf("Skipping to next test..."); */
            r = -1;
        }
    }
    return r;
}

static int test_stuck_address(ulv *bufa, size_t count)
{
    ulv *p1 = bufa;
    unsigned int j;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < 16; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        printf("setting %3u\n", j);
	printf("count:%lu\n",count);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            *p1 = ((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1);
            *p1++;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        for (i = 0; i < count; i++, p1++) {
		printf("i:%ld\n", i);
            if (*p1 != (((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1))) {
                memtester_badaddr(i);
                printf("Skipping to next test...\n");
                fflush(stdout);
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_random_value(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    ul j = 0;
    size_t i;

    putchar(' ');
    fflush(stdout);
    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = rand_ul();
        if (!(i % PROGRESSOFTEN)) {
            putchar('\b');
            putchar(progress[++j % PROGRESSLEN]);
            fflush(stdout);
        }
    }
    printf("\b \b");
    fflush(stdout);
    return compare_regions(bufa, bufb, count);
}

static int test_xor_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ ^= q;
        *p2++ ^= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_sub_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ -= q;
        *p2++ -= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_mul_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ *= q;
        *p2++ *= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_div_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        if (!q) {
            q++;
        }
        *p1++ /= q;
        *p2++ /= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_or_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ |= q;
        *p2++ |= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_and_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ &= q;
        *p2++ &= q;
    }
    return compare_regions(bufa, bufb, count);
}

static int test_seqinc_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = (i + q);
    }
    return compare_regions(bufa, bufb, count);
}

static int test_solidbits_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? UL_ONEBITS : 0;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_checkerboard_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
        printf("setting %3u", j);
        fflush(stdout);
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_blockseq_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < 256; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (ul) UL_BYTE(j);
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_walkbits0_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < UL_LEN * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = ONE << j;
            } else { /* Walk it back down. */
                *p1++ = *p2++ = ONE << (UL_LEN * 2 - j - 1);
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_walkbits1_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < UL_LEN * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
            } else { /* Walk it back down. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << (UL_LEN * 2 - j - 1));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_bitspread_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (j = 0; j < UL_LEN * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        printf("setting %3u", j);
        fflush(stdout);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << j) | (ONE << (j + 2))
                    : UL_ONEBITS ^ ((ONE << j)
                                    | (ONE << (j + 2)));
            } else { /* Walk it back down. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << (UL_LEN * 2 - 1 - j)) | (ONE << (UL_LEN * 2 + 1 - j))
                    : UL_ONEBITS ^ (ONE << (UL_LEN * 2 - 1 - j)
                                    | (ONE << (UL_LEN * 2 + 1 - j)));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        fflush(stdout);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

static int test_bitflip_comparison(ulv *bufa, ulv *bufb, size_t count)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j, k;
    ul q;
    size_t i;

    printf("           ");
    fflush(stdout);
    for (k = 0; k < UL_LEN; k++) {
        q = ONE << k;
        for (j = 0; j < 8; j++) {
            printf("\b\b\b\b\b\b\b\b\b\b\b");
            q = ~q;
            printf("setting %3u", k * 8 + j);
            fflush(stdout);
            p1 = (ulv *) bufa;
            p2 = (ulv *) bufb;
            for (i = 0; i < count; i++) {
                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
            }
            printf("\b\b\b\b\b\b\b\b\b\b\b");
            printf("testing %3u", k * 8 + j);
            fflush(stdout);
            if (compare_regions(bufa, bufb, count)) {
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    fflush(stdout);
    return 0;
}

#ifdef CONFIG_MEMTESTER_NARROW_WRITES
static int test_8bit_wide_random(ulv* bufa, ulv* bufb, size_t count)
{
    u8v *p1, *t;
    ulv *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    putchar(' ');
    fflush(stdout);
    for (attempt = 0; attempt < 2;  attempt++) {
        if (attempt & 1) {
            p1 = (u8v *) bufa;
            p2 = bufb;
        } else {
            p1 = (u8v *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++) {
            t = mword8.bytes;
            *p2++ = mword8.val = rand_ul();
            for (b=0; b < UL_LEN/8; b++) {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN)) {
                putchar('\b');
                putchar(progress[++j % PROGRESSLEN]);
                fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b \b");
    fflush(stdout);
    return 0;
}

static int test_16bit_wide_random(ulv* bufa, ulv* bufb, size_t count)
{
    u16v *p1, *t;
    ulv *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    putchar( ' ' );
    fflush( stdout );
    for (attempt = 0; attempt < 2; attempt++) {
        if (attempt & 1) {
            p1 = (u16v *) bufa;
            p2 = bufb;
        } else {
            p1 = (u16v *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++) {
            t = mword16.u16s;
            *p2++ = mword16.val = rand_ul();
            for (b = 0; b < UL_LEN/16; b++) {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN)) {
                putchar('\b');
                putchar(progress[++j % PROGRESSLEN]);
                fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b \b");
    fflush(stdout);
    return 0;
}
#endif

static struct test {
	const char *name;
	int (*fp)(ulv *, ulv *, size_t);
} tests[] = {
	{ "Random Value", test_random_value },
	{ "Compare XOR", test_xor_comparison },
	{ "Compare SUB", test_sub_comparison },
	{ "Compare MUL", test_mul_comparison },
	{ "Compare DIV",test_div_comparison },
	{ "Compare OR", test_or_comparison },
	{ "Compare AND", test_and_comparison },
	{ "Sequential Increment", test_seqinc_comparison },
	{ "Solid Bits", test_solidbits_comparison },
	{ "Block Sequential", test_blockseq_comparison },
	{ "Checkerboard", test_checkerboard_comparison },
	{ "Bit Spread", test_bitspread_comparison },
	{ "Bit Flip", test_bitflip_comparison },
	{ "Walking Ones", test_walkbits1_comparison },
	{ "Walking Zeroes", test_walkbits0_comparison },
#ifdef CONFIG_MEMTESTER_NARROW_WRITES
	{ "8-bit Writes", test_8bit_wide_random },
	{ "16-bit Writes", test_16bit_wide_random },
#endif
	{ NULL, NULL }
};

#ifdef HOSTED
int main(int argc, char **argv)
#else
static int do_memtester(int argc, char **argv)
#endif
{
	ul loops, loop, i;
	size_t pagesize, wantraw, wantmb, wantbytes, wantbytes_orig;
	size_t bufsize, halflen, count;
	char *memsuffix, *loopsuffix;
	ptrdiff_t pagesizemask;
	void volatile *buf, *aligned;
	ulv *bufa, *bufb;
	int do_mlock = 1, done_mem = 0;
	int exit_code = 0;
	int opt, memshift;
	size_t maxbytes = -1; /* addressable memory, in bytes */
	size_t maxmb = (maxbytes >> 20) + 1; /* addressable memory, in MB */
	ul testmask;

	printf("memtester version " __version__ " (%d-bit)\n", UL_LEN);
	printf("\n");
	check_posix_system();

	pagesize = memtester_pagesize();
	if ((int)pagesize < 0)
		return -pagesize;
	pagesizemask = (ptrdiff_t) ~(pagesize - 1);
	printf("pagesizemask is 0x%lx\n", pagesizemask);

	/* If MEMTESTER_TEST_MASK is set, we use its value as a mask of
	 * which tests we run.
	 */
	testmask = memtester_testmask();
	if ((int)testmask < 0)
		return -testmask;

	opt = memtester_parseopt(argc, argv, pagesize);
	if (opt < 0)
		return -opt;
	argc -= opt;
	argv += opt;

	if (argc < 2) {
		fprintf(stderr, "need memory argument, in MB\n");
		usage();
		return EXIT_FAIL_NONSTARTER;
	}

	errno = 0;
	wantraw = (size_t)strtoul(argv[1], &memsuffix, 0);
	argc--;
	argv++;
	if (errno != 0) {
		fprintf(stderr, "failed to parse memory argument");
		usage();
		return EXIT_FAIL_NONSTARTER;
	}
	switch (*memsuffix) {
	case 'G':
	case 'g':
		memshift = 30; /* gigabytes */
		break;
	case 'M':
	case 'm':
		memshift = 20; /* megabytes */
		break;
	case 'K':
	case 'k':
		memshift = 10; /* kilobytes */
		break;
	case 'B':
	case 'b':
		memshift = 0; /* bytes*/
		break;
	case '\0':  /* no suffix */
		memshift = 20; /* megabytes */
		break;
	default:
		/* bad suffix */
		usage();
		return EXIT_FAIL_NONSTARTER;
	}
	wantbytes_orig = wantbytes = ((size_t)wantraw << memshift);
	wantmb = (wantbytes_orig >> 20);
	if (wantmb > maxmb) {
		fprintf(stderr,
			"This system can only address %llu MB.\n",
			(ull)maxmb);
		return EXIT_FAIL_NONSTARTER;
	}
	if (wantbytes < pagesize) {
		fprintf(stderr,
			"bytes %ld < pagesize %ld -- memory argument too large?\n",
			wantbytes, pagesize);
		return EXIT_FAIL_NONSTARTER;
	}

	if (argc < 1) {
		loops = 0;
	} else {
		errno = 0;
		loops = strtoul(argv[0], &loopsuffix, 0);
		if (errno != 0) {
			fprintf(stderr, "failed to parse number of loops");
			usage();
			return EXIT_FAIL_NONSTARTER;
		}
		if (*loopsuffix != '\0') {
			fprintf(stderr, "loop suffix %c\n", *loopsuffix);
			usage();
			return EXIT_FAIL_NONSTARTER;
		}
	}

	printf("want %lluMB (%llu bytes)\n", (ull)wantmb, (ull)wantbytes);
	buf = NULL;

	done_mem = memtester_usephys(wantbytes, &buf);
	if (done_mem < 0)
		return -done_mem;

	if (done_mem) {
		if (mlock((void *)buf, wantbytes) < 0) {
			fprintf(stderr, "failed to mlock mmap'ed space\n");
			do_mlock = 0;
		}
		bufsize = wantbytes; /* accept no less */
		aligned = buf;
	}

	while (!done_mem) {
		while (!buf && wantbytes) {
			buf = (void volatile *)malloc(wantbytes);
			if (!buf)
				wantbytes -= pagesize;
		}
		bufsize = wantbytes;
		printf("got  %lluMB (%llu bytes)",
		       (ull)wantbytes >> 20, (ull)wantbytes);
		fflush(stdout);
		if (do_mlock) {
			printf(", trying mlock ...");
			fflush(stdout);
			if ((size_t) buf % pagesize) {
				/* printf("aligning to page -- was 0x%tx\n",
				 *        buf);
				*/
				aligned = (void volatile *)
					((size_t)buf & pagesizemask) + pagesize;
				/* printf("  now 0x%tx -- lost %d bytes\n",
				 *        aligned,
				 *       (size_t)aligned - (size_t)buf);
				 */
				bufsize -= ((size_t)aligned - (size_t)buf);
			} else {
				aligned = buf;
			}
			/* Try mlock */
			if (mlock((void *)aligned, bufsize) < 0) {
				switch (errno) {
				case EAGAIN: /* BSDs */
				case ENOMEM:
					printf("reducing...\n");
					free((void *)buf);
					buf = NULL;
					wantbytes -= pagesize;
					break;
				case EPERM:
					printf("insufficient permission.\n");
					printf("Trying again, unlocked:\n");
					do_mlock = 0;
					free((void *)buf);
					buf = NULL;
					wantbytes = wantbytes_orig;
					break;
				default:
					printf("failed for unknown reason.\n");
					do_mlock = 0;
					done_mem = 1;
				}
			} else {
				printf("locked.\n");
				done_mem = 1;
			}
		} else {
			done_mem = 1;
			printf("\n");
		}
	}

	if (!do_mlock) {
		fprintf(stderr,
			"Continuing with unlocked memory; "
			"testing will be slower and less reliable.\n");
	}

	/* Do alighnment here as well, as some cases won't trigger above
	 * if you define out the use of mlock() (cough HP/UX 10 cough).
	 */
	if ((size_t)buf % pagesize) {
		/* printf("aligning to page -- was 0x%tx\n", buf); */
		aligned = (void volatile *)
			((size_t) buf & pagesizemask) + pagesize;
		/* printf("  now 0x%tx -- lost %d bytes\n",
		 *        aligned, (size_t)aligned - (size_t)buf);
		 */
		bufsize -= ((size_t)aligned - (size_t)buf);
	} else {
		aligned = buf;
	}

	halflen = bufsize / 2;
	count = halflen / sizeof(ul);
	bufa = (ulv *)aligned;
	bufb = (ulv *)((size_t)aligned + halflen);

	for (loop=1; ((!loops) || loop <= loops); loop++) {
		printf("Loop %lu", loop);
		if (loops)
			printf("/%lu", loops);
		printf(":\n");
		printf("  %-20s: ", "Stuck Address");
		fflush(stdout);
		if (!test_stuck_address(aligned, bufsize / sizeof(ul)))
			printf("ok\n");
		else
			exit_code |= EXIT_FAIL_ADDRESSLINES;
		for (i = 0; ; i++) {
			if (!tests[i].name)
				break;
			/* If using a custom testmask, only run this test
			 * if the bit corresponding to this test was set
			 * by the user.
			 */
			if (testmask && (!((1 << i) & testmask)))
				continue;
			printf("  %-20s: ", tests[i].name);
			if (!tests[i].fp(bufa, bufb, count))
				printf("ok\n");
			else
				exit_code |= EXIT_FAIL_OTHERTEST;
			fflush(stdout);
			/* clear buffer */
			memset((void *)buf, 255, wantbytes);
		}
		printf("\n");
		fflush(stdout);
	}
	if (do_mlock)
		munlock((void *)aligned, bufsize);
	printf("Done.\n");
	fflush(stdout);
	return exit_code;
}

#ifndef HOSTED
DEFINE_COMMAND(memtester, do_memtester, "Memory stress tests",
	"memtester [-p physaddrbase] <mem>[B|K|M|G] [loops]\n"
);
#endif
