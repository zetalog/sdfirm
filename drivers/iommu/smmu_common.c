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
	smmu_sme_t sme = smmu_group_ctrl.sme;
	smmu_gr_t gr = smmu_sme_gr(sme);

	BUG_ON(smmu_sme_dev(sme) != iommu_dev);
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

smmu_gr_t smmu_stream_alloc(smmu_sme_t sme)
{
	__unused smmu_gr_t sid = INVALID_SMMU_SID, ssid;

	for (sid = 0; sid < NR_SMMU_STREAMS; ++sid) {
		ssid = smmu_stream_save(sid);
		if (!smmu_stream_ctrl.valid) {
			smmu_stream_ctrl.sme = sme;
			smmu_stream_install();
			smmu_stream_restore(ssid);
			return sid;
		}
		smmu_stream_restore(ssid);
	}
	return INVALID_SMMU_SID;
}

smmu_gr_t smmu_stream_find(smmu_sme_t sme)
{
	__unused smmu_gr_t sid = INVALID_SMMU_SID, ssid;
	smmu_gr_t sme_gr, sme_sm;
	smmu_gr_t gr, sm;
	iommu_dev_t dev;

	dev = smmu_sme_dev(sme);
	gr = smmu_sme_gr(sme);
	sm = smmu_sme_sm(sme);
	for (sid = 0; sid < NR_SMMU_STREAMS; ++sid) {
		ssid = smmu_stream_save(sid);

		/* Skip other SMMU devices */
		if (iommu_dev != dev || !smmu_stream_ctrl.valid) {
			smmu_stream_restore(ssid);
			continue;
		}

		sme = smmu_stream_ctrl.sme;
		sme_gr = smmu_sme_gr(sme);
		sme_sm = smmu_sme_sm(sme);

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
	return sid == INVALID_SMMU_SID ? smmu_stream_alloc(sme) : sid;
}

void smmu_stream_free(smmu_gr_t sid)
{
	__unused smmu_gr_t ssid;

	ssid = smmu_stream_save(sid);
	if (smmu_stream_ctrl.count > 0)
		smmu_stream_ctrl.count--;
	if (smmu_stream_ctrl.count == 0) {
		smmu_stream_uninstall();
		smmu_stream_ctrl.sme = INVALID_SMMU_SME;
	}
	smmu_stream_restore(ssid);
}

iommu_grp_t smmu_group_alloc(int nr_sids, smmu_sme_t *sids)
{
	__unused iommu_dev_t dev, sdev;
	__unused smmu_gr_t sid, ssid;
	__unused smmu_gr_t gr, sm;
	__unused iommu_grp_t grp;
	int i;

	grp = iommu_alloc_group(nr_sids, sids);
	if (grp == INVALID_IOMMU_GRP)
		return grp;

#if 0
	/* Check the SIDs are in range of the SMMU and our stream table */
	for (i = 0; i < smmu_group_ctrl.num_sids; i++) {
		uint32_t sid = smmu_group_ctrl.sids[i];

	}
	smmu_group_ctrl.ssid_bits = min(smmu_device_ctrl.ssid_bits, smmu_hw_num_pasid_bits);
#endif
	for (i = 0; i < nr_sids; i++) {
		smmu_sme_t sme = sids[i];

#if 0
		BUG_ON(!arm_smmu_sid_in_range(sid));
#endif

		dev = smmu_sme_dev(sme);
		sdev = iommu_device_save(dev);

		gr = smmu_sme_gr(sme);
		sm = smmu_sme_sm(sme);

		sid = smmu_stream_find(sme);
		if (sid != INVALID_SMMU_SID) {
			ssid = smmu_stream_save(sid);
			if (smmu_stream_ctrl.count == 0) {
				smmu_stream_alloc(sme);
				smmu_stream_ctrl.sme = sme;
			}
			smmu_stream_ctrl.count++;
			iommu_group_restore(ssid);
		}
		iommu_device_restore(sdev);
	}
	return grp;
}
