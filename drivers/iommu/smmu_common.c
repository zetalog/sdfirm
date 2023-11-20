#include <target/iommu.h>
#include <target/panic.h>

struct smmu_device smmu_devices[NR_IOMMU_DEVICES];
/* Also known as SMMU masters, contains multiple streams, reflects a bus
 * device that can perform master side DMA.
 */
struct smmu_group smmu_groups[NR_IOMMU_GROUPS];
struct smmu_domain smmu_domains[NR_IOMMU_DOMAINS];

#if NR_SMMU_STREAMS > 1
struct smmu_stream smmu_streams[NR_SMMU_STREAMS];
smmu_gr_t smmu_sid;

void smmu_stream_restore(smmu_gr_t gr)
{
	smmu_device_ctrl.gr = gr;
}

smmu_gr_t smmu_stream_save(smmu_gr_t gr)
{
	smmu_gr_t sgr = smmu_device_ctrl.gr;

	smmu_stream_restore(gr);
	return sgr;
}
#endif

void smmu_stream_select(void)
{
	iommu_map_t sme = smmu_group_ctrl.sme;
	smmu_gr_t gr = IOMMU_MAP_BASE(sme);

	smmu_stream_restore(gr);
}

void smmu_domain_restore(smmu_cb_t cb)
{
	smmu_device_ctrl.cb = cb;
}

smmu_cb_t smmu_domain_save(smmu_cb_t cb)
{
	smmu_cb_t scb = smmu_device_ctrl.cb;

	smmu_domain_restore(cb);
	return scb;
}

void smmu_domain_select(void)
{
	smmu_domain_restore(smmu_domain_ctrl.cb);
}

#if 0
		/* No harm to re-use matched existing stream mapping */
		if ((sm & sme_sm) == sm && !((gr ^ sme_gr) & ~sme_sm))
			break;

		/* Avoid conflict stream mapping */
		if (!((gr ^ sme_gr) & ~(sme_sm | sm))) {
			con_err("smmu: conflict stream: 0x%04x/0x%04x.",
				gr, sm);
			smmu_stream_restore(ssid);
			return INVALID_SMMU_SID;
		}
		smmu_stream_restore(ssid);
	}

	grp = iommu_alloc_group(nr_sids, sids);
	if (grp == INVALID_IOMMU_GRP)
		return grp;

	smmu_group_ctrl.ssid_bits = min(smmu_device_ctrl.ssid_bits, smmu_hw_num_pasid_bits);
	return grp;
#endif
