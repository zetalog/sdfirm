#include <target/mmc.h>
#include <target/cmdline.h>
#include <target/mem.h>

#ifdef CONFIG_MMC_SELECTED_CARD
#define MMC_SELECTED_CARD	CONFIG_MMC_SELECTED_CARD
#else
#define MMC_SELECTED_CARD	-1
#endif

struct mem_card {
	mmc_rca_t rca;
	bool selected;
	bool busy;
};

struct mem_card mem_cards[NR_MMC_CARDS];
mmc_card_t mmc_nr_cards = 0;
uint8_t mem_card_buf[MMC_DEF_BL_LEN] __align(4);
bool mem_card_busy = false;

void mmc_card_complete(mmc_rca_t rca, bool result)
{
	printf("selected card\n");
}

mmc_card_t mmc_register_card(mmc_rca_t rca)
{
	mmc_card_t card;

	BUG_ON(mmc_nr_cards >= INVALID_MMC_CARD);
	card = mmc_nr_cards;
	mem_cards[card].rca = rca;
	mem_cards[card].selected = false;
	mem_cards[card].busy = false;
	if (mmc_nr_cards == MMC_SELECTED_CARD)
		mmc_select_card(mmc_card_complete);
	mmc_nr_cards++;
	return card;
}

void mmc_card_complete_copy(mmc_rca_t rca, bool result)
{
	mmc_card_t i;

	for (i = 0; i < mmc_nr_cards; i++) {
		if (mem_cards[i].rca == rca) {
			mem_cards[i].busy = false;
			mem_print_data(0, mem_card_buf, 1,
				       MMC_DEF_BL_LEN);
			return;
		}
	}
	mem_card_busy = false;
}

int mmc_card_copy(mmc_rca_t rca, caddr_t lba, size_t size)
{
	__unused mmc_slot_t sslot;
	mmc_card_t i;
	
	if (mem_card_busy)
		return -EBUSY;
	mem_card_busy = true;

	for (i = 0; i < mmc_nr_cards; i++) {
		if (mem_cards[i].rca == rca) {
			if (!mem_cards[i].busy) {
				mem_cards[i].busy = true;
				sslot = mmc_slot_save(MMC_SLOT(rca));
				mmc_read_blocks(mem_card_buf,
						(mmc_lba_t)lba,
						size,
						mmc_card_complete_copy);
				mmc_slot_restore(sslot);
			}
			return 0;
		}
	}
	return -ENODEV;
}

static int do_card_list(int argc, char *argv[])
{
	int i;

	for (i = 0; i < mmc_nr_cards; i++) {
		printf("#\tID\tSLOT\tCARD\n");
		printf("%d\t%d\t%d\t%d\n", i, mem_cards[i].rca,
		       MMC_SLOT(mem_cards[i].rca),
		       MMC_CARD(mem_cards[i].rca));
	}
	return 0;
}

static int do_card_dump(int argc, char *argv[])
{
	uint64_t addr = 0;
	mmc_rca_t rca;

	if (argc < 3)
		return -EINVAL;
	rca = strtoul(argv[2], 0, 0);
	if (argc > 3)
		addr = strtoul(argv[3], 0, 0);
	mmc_card_copy(rca, (caddr_t)addr, MMC_DEF_BL_LEN);
	return 0;
}

static int do_card(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "list") == 0)
		return do_card_list(argc, argv);
	if (strcmp(argv[1], "dump") == 0)
		return do_card_dump(argc, argv);
	return 0;
}

DEFINE_COMMAND(mmcsd, do_card, "multimedia / secure digital card commands",
	"    - MMC/SD commands\n"
	"list\n"
	"    - list slots and cards"
	"dump rca lba\n"
	"    - dump content of MMC/SD card"
	"\n"
);
