#include <target/noc.h>

#define CCN512_HNF_BASE(n)			(0x200000 + n*0x10000)
#define CCN512_SNOOP_DOMAIN_CTRL_SET_OFF	UL(0x210)

#define get_hnf_snoop_set_addr(base, idx)	(base + CCN512_HNF_BASE(idx) + CCN512_SNOOP_DOMAIN_CTRL_SET_OFF)

// uint8_t ccn512_hnf_idx2nid[] = {2, 5, 6, 9, 20, 23, 24, 27};

void ccn512_join_snoop_domain(uint64_t base, uint64_t rqf_nid_bitmap,
			      uint8_t hnf_idx)
{
	uint64_t *hnf_snoop_set_addr =
		(uint64_t *)get_hnf_snoop_set_addr(base, hnf_idx);
	uint64_t hnf_snoop_set_val;

	hnf_snoop_set_val = *(hnf_snoop_set_addr);
	hnf_snoop_set_val |= rqf_nid_bitmap;
	*(hnf_snoop_set_addr) = hnf_snoop_set_val;
}

void ccn512_init(struct s_ccn512 *ccn512)
{
	uint64_t base_addr = ccn512->base;
	uint64_t rqf_node_bitmap = ccn512->cc_rqf_bitmap;
	uint8_t idx, num_hnf = ccn512->num_hnf;

	for (idx = 0; idx < num_hnf; idx++)
		ccn512_join_snoop_domain(base_addr, rqf_node_bitmap, idx);
}
