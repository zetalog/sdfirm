#include <target/ddr.h>

#ifdef CONFIG_DDR_SPD_3200
#define DDR_SPD_DEFAULT		DDR4_3200
#endif
#ifdef CONFIG_DDR_SPD_2933
#define DDR_SPD_DEFAULT		DDR4_2933
#endif
#ifdef CONFIG_DDR_SPD_2666
#define DDR_SPD_DEFAULT		DDR4_2666
#endif
#ifdef CONFIG_DDR_SPD_2400
#define DDR_SPD_DEFAULT		DDR4_2400
#endif
#ifdef CONFIG_DDR_SPD_2133
#define DDR_SPD_DEFAULT		DDR4_2133
#endif
#ifdef CONFIG_DDR_SPD_1866
#define DDR_SPD_DEFAULT		DDR4_1866
#endif
#ifdef CONFIG_DDR_SPD_1600
#define DDR_SPD_DEFAULT		DDR4_1600
#endif
#ifdef CONFIG_DDR_SPD_1333
#define DDR_SPD_DEFAULT		DDR3_1333
#endif
#ifdef CONFIG_DDR_SPD_1066
#define DDR_SPD_DEFAULT		DDR3_1066
#endif
#ifdef CONFIG_DDR_SPD_800
#define DDR_SPD_DEFAULT		DDR3_800
#endif
#ifdef CONFIG_DDR_SPD_667
#define DDR_SPD_DEFAULT		DDR2_667
#endif
#ifdef CONFIG_DDR_SPD_533
#define DDR_SPD_DEFAULT		DDR2_533
#endif
#ifdef CONFIG_DDR_SPD_400
#define DDR_SPD_DEFAULT		DDR2_400
#endif
#ifdef CONFIG_DDR_SPD_266
#define DDR_SPD_DEFAULT		DDR_266
#endif
#ifdef CONFIG_DDR_SPD_200
#define DDR_SPD_DEFAULT		DDR_200
#endif
uint8_t ddr_spd = DDR_SPD_DEFAULT;
uint8_t ddr_dev = 0;
struct ddr_slot ddr_slots[NR_DDR_SLOTS];
struct ddr_chan ddr_chans[NR_DDR_CHANS];
ddr_cid_t ddr_cid;
ddr_sid_t ddr_sid;

#if NR_DDR_CHANS > 1
void ddr_chan_restore(ddr_cid_t chan)
{
	ddr_cid = chan;
	ddr_hw_chan_select(chan);
}

ddr_cid_t ddr_chan_save(ddr_cid_t chan)
{
	ddr_cid_t ocid = ddr_cid;
	ddr_chan_restore(chan);
	return ocid;
}
#endif

#if NR_DDR_SLOTS > 1
void ddr_slot_restore(ddr_sid_t slot)
{
	ddr_sid = slot;
	ddr_hw_slot_select(slot);
}

ddr_sid_t ddr_slot_save(ddr_sid_t slot)
{
	ddr_sid_t osid = ddr_sid;
	ddr_slot_restore(slot);
	return osid;
}
#endif

uint16_t ddr_spd_speeds[DDR_MAX_SPDS] = {
	[DDR_200] = 200,
	[DDR_266] = 266,
	[DDR_333] = 333,
	[DDR_400] = 400, /* DDR2_400 */
	[DDR2_533] = 533,
	[DDR2_667] = 667,
	[DDR2_800] = 800, /* DDR3_800 */
	[DDR3_1066] = 1066,
	[DDR3_1333] = 1333,
	[DDR3_1600] = 1600, /* DDR4_1600 */
	[DDR4_1866] = 1866,
	[DDR4_2133] = 2133,
	[DDR4_2400] = 2400,
	[DDR4_2666] = 2666,
	[DDR4_2933] = 2933,
	[DDR4_3200] = 3200,
};

uint16_t ddr_spd2speed(uint8_t spd)
{
	if (spd >= DDR_MAX_SPDS)
		spd = ddr_spd;
	return ddr_spd_speeds[spd];
}

void ddr_config_speed(uint8_t spd)
{
	ddr_spd = spd;
	ddr_hw_config_speed(spd);
	ddr4_config_speed(ddr_dev);
}

#if 0
void ddr_config_module(uint8_t n, uint8_t type,
		       uint8_t dies, uint8_t ranks, uint8_t width)
{
	ddr4_config_module(n, type, dies, ranks, width);
}
#endif

void ddr_reset_slot(void)
{
	ddr_slot_ctrl.sid = ddr_sid;
	ddr_hw_slot_reset();
	ddr_spd_read(ddr_slot_ctrl.spd_buf);
}

void ddr_reset_chan(void)
{
	ddr_chan_ctrl.cid = ddr_cid;
}

void ddr_init(void)
{
	ddr_cid_t chan;
	ddr_sid_t slot;
	__unused ddr_cid_t schan;
	__unused ddr_sid_t sslot;

	DEVICE_INTF(DEVICE_INTF_DDR);
	for (slot = 0; slot < NR_DDR_SLOTS; slot++) {
		sslot = ddr_slot_save(slot);
		ddr_reset_slot();
		ddr_slot_restore(sslot);
	}
	for (chan = 0; chan < NR_DDR_CHANS; chan++) {
		schan = ddr_chan_save(chan);
		ddr_reset_chan();
		ddr_chan_restore(schan);
	}
	ddr_config_speed(DDR_SPD_DEFAULT);
	ddr_enable_speed(ddr_spd);

	ddr_hw_ctrl_init();
	ddr_hw_ctrl_start();
}
