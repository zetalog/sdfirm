/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)ddr_spd.h: DDR serial presence detect (SPD) interface
 * $Id: ddr_spd.h,v 1.0 2022-05-20 12:00:00 zhenglv Exp $
 */

#ifndef __DDR_SPD_H_INCLUDE__
#define __DDR_SPD_H_INCLUDE__

#include <target/generic.h>

/* JEDEC Standard No. 21-C
 * 4.1.2.4 – Appendix D, Rev. 1.0 : SPD’s for DDR SDRAM
 */
struct ddr1_spd_eeprom {
	/*   0: Number of Serial PD Bytes written during module production */
	uint8_t spd_size;
	/*   1: Total number of Bytes in Serial PD device */
	uint8_t mem_size;
	/*   2: Fundamental Memory Type (FP, EDO, SDRAM ...) */
	uint8_t mem_type;
	/*   3: Number of Row Addresses of this assembly */
	uint8_t nr_row_addr;
	/*   4: Number of Column Addresses of this assembly */
	uint8_t nr_col_addr;
	/*   5: Number of DIMM Banks */
	uint8_t nr_banks;
	/*   6: Data Width of this assembly */
	uint8_t data_width;
	/*   7: Data Width of this assembly */
	uint8_t data_width_msb;
	/*   8: Voltage Interface Level of this assembly */
	uint8_t voltage;
	/*   9: SDRAM Cycle time at Maximum Supported CASn Latency (CL),
	 *      CL=X
	 */
	uint8_t clk_cycle;
	/*  10: SDRAM Access from Clock */
	uint8_t tAC;
	/*  11: DIMM configuration type (Non-parity, Parity or ECC) */
	uint8_t dimm_config_type;
	/*  12: Refresh Rate/Type */
	uint8_t refresh;
	/*  13: Primary SDRAM Width */
	uint8_t primary_width;
	/*  14: Error Checking SDRAM width */
	uint8_t error_check_width;
	/*  15: SDRAM Device Attributes: Minimum Clock Delay, Back-to-Back
	 *      Random Column Access.
	 */
	uint8_t dev_min_clk_delay;
	/*  16: SDRAM Device Attributes: Burst Length Supported */
	uint8_t dev_burst_len;
	/*  17: SDRAM Device Attributes: Number of Banks on SDRAM Device */
	uint8_t dev_nr_banks;
	/*  18: SDRAM Device Attributes: CASn Latency */
	uint8_t dev_cas_latency;
	/*  19: SDRAM Device Attributes: CSn Latency */
	uint8_t dev_cs_latency;
	/*  20: SDRAM Device Attributes: Write Latency */
	uint8_t dev_write_latency;
	/*  21: SDRAM Module Attributes */
	uint8_t mod_attr;
	/*  22: SDRAM Device Attributes: General */
	uint8_t dev_attr;
	/*  23: Minimum Clock Cycle at CL=X-0.5 */
	uint8_t min_clk_cycle_0_5;
	/*  24: Maximum Data Access Time (tAC) from Clock at CL=X-0.5 */
	uint8_t max_tAC_0_5;
	/*  25: Minimum Clock Cycle at CL=X-1 */
	uint8_t min_clk_cycle_1;
	/*  26: Maximum Data Access Time (tAC) from Clock at CL=X-1 */
	uint8_t max_tAC_1;
	/*  27: Minimum Row Precharge Time (tRP) */
	uint8_t min_tRP;
	/*  28: Minimum Row Active to Row Active delay */
	uint8_t min_tRRD;
	/*  29: Minumum RASn to CASn delay */
	uint8_t min_tRCD;
	/*  30: Minimum Active to Precharge Time */
	uint8_t min_tRAS;
	/*  31: Module Bank Density */
	uint8_t mod_bank_density;
	/*  32: Address and Commnad Input Setup Time Before Clock */
	uint8_t addr_cmd_setup;
	/*  33: Address and Command Input Hold Time After Clock */
	uint8_t addr_cmd_hold;
	/*  34: Data Input Setup Time Before Clock */
	uint8_t data_setup;
	/*  35: Data Input Hold Time After Clock */
	uint8_t data_hold;
	/* 36-40: Superset Information (may be used in the future */
	uint8_t rsvd_36[5];
	/*  41: SDRAM Device Minimum Active to Active/Auto Refresh Time
	 *      (tRC)
	 */
	uint8_t dev_min_tRC;
	/*  42: SDRAM Device Minimum Auto Refresh to Active/Auto Refresh
	 *      (tRFC)
	 */
	uint8_t dev_min_tRFC;
	/*  43: SDRAM Device Maximum device cycle time (tCKmax) */
	uint8_t dev_tCKmax;
	/*  44: SDRAM Device Maximum skew between DQS and DQ signal */
	uint8_t dev_max_tDQSQ;
	/*  45: DDR SDRAM Device Maximum Read Data Hold Skew Factor
	 *      (tQHS)
	 */
	uint8_t dev_max_tQHS;
	/*  46: Reserved for future use */
	uint8_t rsvd_46;
	/*  47: SDRAM Device Attribute - DDR SDRAM DIMM Height */
	uint8_t dev_dimm_height;
	/* 48-61: Reserved */
	uint8_t rsvd_48[14];
	/*  62: SPD Revision */
	uint8_t spd_rev;
	/*  63: Checksum for Bytes 0-62 */
	uint8_t csum;
	/* 64-71: Manufacturer’s JEDEC ID Code (JEP-106) */
	uint8_t jedec_id[8];
	/*  72: Module Manufacturing Location */
	uint8_t mod_loc;
	/* 73-90: Module Part Number */
	uint8_t mod_part[8];
	/* 91-92: Module Revision Code */
	uint8_t mod_rev[2];
	/* 93-94: Module Manufacturing Date */
	uint8_t mod_date[2];
	/* 95-98: Module Serial Number */
	uint8_t mod_sn[4];
	/* 99-127: Manufacturer's Specific Data */
	uint8_t mod_spec_data[27];
} __packed;

/* JEDEC Standard No. 21-C
 * Annex J: Serial Presence Detects for DDR2 SDRAM (Revision 1.3)
 */
struct ddr2_spd_eeprom {
	/*   0: Number of Serial PD Bytes written during module production */
	uint8_t spd_size;
	/*   1: Total number of Bytes in Serial PD device */
	uint8_t mem_size;
	/*   2: Fundamental Memory Type (FP, EDO, SDRAM ...) */
	uint8_t mem_type;
	/*   3: Number of Row Addresses of this assembly */
	uint8_t nr_row_addr;
	/*   4: Number of Column Addresses of this assembly */
	uint8_t nr_col_addr;
	/*   5: Number of DIMM Banks */
	uint8_t nr_banks;
	/*   6: Data Width of this assembly */
	uint8_t data_width;
	/*   7: Reserved */
	uint8_t rsvd_7;
	/*   8: Voltage Interface Level of this assembly */
	uint8_t voltage;
	/*   9: SDRAM Cycle time at Maximum Supported CASn Latency (CL),
	 *      CL=X
	 */
	uint8_t clk_cycle;
	/*  10: SDRAM Access from Clock */
	uint8_t tAC;
	/*  11: DIMM configuration type (Non-parity, Parity or ECC) */
	uint8_t dimm_config_type;
	/*  12: Refresh Rate/Type */
	uint8_t refresh;
	/*  13: Primary SDRAM Width */
	uint8_t primary_width;
	/*  14: Error Checking SDRAM width */
	uint8_t error_check_width;
	/*  15: Reserved */
	uint8_t rsvd_15;
	/*  16: SDRAM Device Attributes: Burst Length Supported */
	uint8_t dev_burst_len;
	/*  17: SDRAM Device Attributes: Number of Banks on SDRAM Device */
	uint8_t dev_nr_banks;
	/*  18: SDRAM Device Attributes: CASn Latency */
	uint8_t dev_cas_latency;
	/*  19: DIMM Mechanical Characteristics */
	uint8_t dimm_mach_char;
	/*  20: DIMM Type Information */
	uint8_t dimm_type_info;
	/*  21: SDRAM Module Attributes */
	uint8_t mod_attr;
	/*  22: SDRAM Device Attributes: General */
	uint8_t dev_attr;
	/*  23: Minimum Clock Cycle at CL=X-1 */
	uint8_t min_clk_cycle_1;
	/*  24: Maximum Data Access Time (tAC) from Clock at CL=X-1 */
	uint8_t max_tAC_1;
	/*  25: Minimum Clock Cycle at CL=X-2 */
	uint8_t min_clk_cycle_2;
	/*  26: Maximum Data Access Time (tAC) from Clock at CL=X-2 */
	uint8_t max_tAC_2;
	/*  27: Minimum Row Precharge Time (tRP) */
	uint8_t min_tRP;
	/*  28: Minimum Row Active to Row Active delay */
	uint8_t min_tRRD;
	/*  29: Minumum RASn to CASn delay */
	uint8_t min_tRCD;
	/*  30: Minimum Active to Precharge Time */
	uint8_t min_tRAS;
	/*  31: Module Rank Density */
	uint8_t mod_rank_density;
	/*  32: Address and Commnad Input Setup Time Before Clock */
	uint8_t addr_cmd_setup;
	/*  33: Address and Command Input Hold Time After Clock */
	uint8_t addr_cmd_hold;
	/*  34: Data Input Setup Time Before Clock */
	uint8_t data_setup;
	/*  35: Data Input Hold Time After Clock */
	uint8_t data_hold;
	/*  36: Write recovery time (tWR) */
	uint8_t tWR;
	/*  37: Internal write to read command delay (tWTR) */
	uint8_t tWTR;
	/*  38: Internal read to precharge command delay (tRTP) */
	uint8_t tRTP;
	/*  39: Memory Analysis Probe Characteristics */
	uint8_t mem_analysis_probe_char;
	/*  40: Extension of Byte 41 tRC and Byte 42 tRFC */
	uint8_t ext_tRC_tRFC;
	/*  41: SDRAM Device Minimum Active to Active/Auto Refresh Time
	 *      (tRC)
	 */
	uint8_t dev_min_tRC;
	/*  42: SDRAM Device Minimum Auto Refresh to Active/Auto Refresh
	 *      (tRFC)
	 */
	uint8_t dev_min_tRFC;
	/*  43: SDRAM Device Maximum device cycle time (tCKmax) */
	uint8_t dev_tCKmax;
	/*  44: SDRAM Device Maximum skew between DQS and DQ signal */
	uint8_t dev_max_tDQSQ;
	/*  45: DDR SDRAM Device Maximum Read Data Hold Skew Factor
	 *      (tQHS)
	 */
	uint8_t dev_max_tQHS;
	/*  46: PLL Relock Time */
	uint8_t pll_relock_time;
	/*  47: Bits 7:4: Tcasemax Delta (DRAM case temperature difference
	 *      between maximum case temperature  and baseline maximum
	 *      case temperature), the baseline maximum case temperature
	 *      is 85 0C.
	 *      Bits 3:0: DT4R4W Delta (Case temperature rise difference
	 *      between IDD4R/page open burst read and IDD4W/page open
	 *      burst write operations).
	 */
	uint8_t tcase_delta;
	/*  48: Thermal resistance of DRAM device package from top (case)
	 *      to ambient (Psi T-A DRAM) at still air condition based on
	 *      JESD51-2 standard.
	 */
	uint8_t psi_ta_dram;
	/*  49: DT0/Tcase Mode Bits:
	 *      Bits 7:2:Case temperature rise from ambient due to
	 *      IDD0/activate-precharge operation minus 2.8-0C offset
	 *      temperature.
	 *      Bit 1: Double Refresh mode bit.
	 *      Bit 0: High Temperature self-refresh rate support
	 *      indication
	 */
	uint8_t DT0_tcase_mode;
	/*  50: DT2N/DT2Q: Case temperature rise from ambient due to
	 *      IDD2N/precharge standby operation for UDIMM and due to
	 *      IDD2Q/precharge quiet standby operation for RDIMM.
	 */
	uint8_t DT2N_DT2Q;
	/*  51: DT2P: Case temperature rise from ambient due to
	 *      IDD2P/precharge power-down operation.
	 */
	uint8_t DT2P;
	/*  52: DT3N: Case temperature rise from ambient due to
	 *      IDD3N/active standby operation.
	 */
	uint8_t DT3N;
	/*  53: DT3Pfast: Case temperature rise from ambient due to IDD3P
	 *      Fast PDN Exit/active powerdown with Fast PDN Exit
	 *      operation.
	 */
	uint8_t DT3Pfast;
	/*  54: DT3Pslow: Case temperature rise from ambient due to IDD3P
	 *      Slow PDN Exit/active powerdown with Slow PDN Exit
	 *      operation.
	 */
	uint8_t DT3Pslow;
	/*  55: DT4R/Mode Bit:
	 *      Bits 7:1: Case temperature rise from ambient due to
	 *      IDD4R/page open burst read operation.
	 *      Bit 0: Mode bit to specify if DT4W is greater or less than
	 *      DT4R.
	 */
	uint8_t DR4R_mode;
	/*  56: DT5B: Case temperature rise from ambient due to
	 *      IDD5B/burst refresh operation.
	 */
	uint8_t DT5B;
	/*  57: DT7: Case temperature rise from ambient due to IDD7/bank
	 *      interleave read mode operation.
	 */
	uint8_t DT7;
	/*  58: Thermal resistance of PLL device package from top (case)
	 *      to ambient (Psi T-A PLL) at still air condition based on
	 *      JESD51-2 standard.
	 */
	uint8_t psi_ta_pll;
	/*  59: Thermal resistance of register device package from top
	 *      (case) to ambient (Psi T-A Register) at still air
	 *      condition based on JESD51-2 standard.
	 */
	uint8_t psi_ta_reg;
	/*  60: DT PLL Active: Case temperature rise from ambient due to
	 *      PLL in active mode at VCC = 1.9 V, the PLL loading is the
	 *      DIMM loading.
	 */
	uint8_t dt_pll_active;
	/*  61: DT Register Active/Mode Bit:
	 *      Bits 7:1: Case temperature rise from ambient due to
	 *      register in active mode at VCC = 1.9 V, the register
	 *      loading is the RDIMM loading.
	 *      Bit 0: mode bit to specify register data output toggle
	 *      rate 50% or 100%.
	 */
	uint8_t dt_reg_active_mode;
	/*  62: SPD Revision */
	uint8_t spd_rev;
	/*  63: Checksum for Bytes 0-62 */
	uint8_t csum;
	/* 64-71: Manufacturer’s JEDEC ID Code (JEP-106) */
	uint8_t jedec_id[8];
	/*  72: Module Manufacturing Location */
	uint8_t mod_loc;
	/* 73-90: Module Part Number */
	uint8_t mod_part[8];
	/* 91-92: Module Revision Code */
	uint8_t mod_rev[2];
	/* 93-94: Module Manufacturing Date */
	uint8_t mod_date[2];
	/* 95-98: Module Serial Number */
	uint8_t mod_sn[4];
	/* 99-127: Manufacturer's Specific Data */
	uint8_t mod_spec_data[27];
} __packed;

/* Annex K.1: Module Specific Bytes for Unbuffered Memory Module Types
 */
struct ddr3_spd_unbufferred_eeprom {
	/*  60: Raw Card Extension, Module Nominal Height */
	uint8_t mod_height;
	/*  61: Module Maximum Thickness */
	uint8_t mod_thickness;
	/*  62: Reference Raw Card Used */
	uint8_t ref_raw_card;
	/*  63: Address Mapping from Edge Connector to DRAM */
	uint8_t addr_mapping;
	/* 64-116: Reserved */
	uint8_t rsvd_64[53];
} __packed;

/* Annex K.2: Module Specific Bytes for Registered Memory Module Types
 */
struct ddr3_spd_registered_eeprom {
	/*  60: Raw Card Extension, Module Nominal Height */
	uint8_t mod_height;
	/*  61: Module Maximum Thickness */
	uint8_t mod_thickness;
	/*  62: Reference Raw Card Used */
	uint8_t ref_raw_card;
	/*  63: DIMM Module Attributes */
	uint8_t dimm_mod_attr;
	/*  64: RDIMM Thermal Heat Spreader Solution */
	uint8_t rdimm_thermal;
	/*  65: Register Manufacturer ID Code, LSB */
	uint8_t reg_id_lsb;
	/*  66: Register Manufacturer ID Code, MSB */
	uint8_t reg_id_msb;
	/*  67: Register Revision Number */
	uint8_t reg_rev;
	/*  68: Register Type */
	uint8_t reg_type;
	/*  69: RC1 (MS Nibble) / RC0 (LS Nibble) */
	uint8_t RC1_msn_RC0_lsn;
	/*  70: RC3 (MS Nibble) / RC2 (LS Nibble) - Drive Strength,
	 *      Command/Address
	 */
	uint8_t RC3_msn_RC2_lsn;
	/*  71: RC5 (MS Nibble) / RC4 (LS Nibble) - Drive Strength,
	 *      Control and Clock
	 */
	uint8_t RC5_msn_RC4_lsn;
	/*  72: RC7 (MS Nibble) / RC6 (LS Nibble) */
	uint8_t RC7_msn_RC6_lsn;
	/*  73: RC9 (MS Nibble) / RC10 (LS Nibble) */
	uint8_t RC9_msn_RC10_lsn;
	/*  74: RC11 (MS Nibble) / RC12 (LS Nibble) */
	uint8_t RC11_msn_RC12_lsn;
	/*  75: RC13 (MS Nibble) / RC14 (LS Nibble) */
	uint8_t RC13_msn_RC14_lsn;
	/*  76: RC15 (MS Nibble) / RC16 (LS Nibble) */
	uint8_t RC14_msn_RC16_lsn;
	/* 77-116: Reserved */
	uint8_t rsvd_77[40];
} __packed;

/* JEDEC Standard No. 21-C
 * Annex K: Serial Presence Detect (SPD) for DDR3 SDRAM Modules
 */
struct ddr3_spd_eeprom {
	/*   0: Number of Serial PD Bytes Written / SPD Device Size / CRC
	 *      Coverage
	 */
	uint8_t spd_size_crc;
	/*   1: SPD Revision */
	uint8_t spd_rev;
	/*   2: Key Byte/DRAM Device Type */
	uint8_t mem_type;
	/*   3: Key Byte/Module Type */
	uint8_t mod_type;
	/*   4: SDRAM Density and Banks */
	uint8_t density_banks;
	/*   5: SDRAM Addressing */
	uint8_t addressing;
	/*   6: Module Nominal Voltage, VDD */
	uint8_t mod_VDD;
	/*   7: Module Organization */
	uint8_t mod_org;
	/*   8: Module Memory Bus Width */
	uint8_t mod_bus_width;
	/*   9: Fine Timebase (FTB) Dividend/Divisor */
	uint8_t FTB_div;
	/*  10: Medium Timebase (MTB) Dividend */
	uint8_t MTB_dividend;
	/*  11: Medium Timebase (MTD) Divisor */
	uint8_t MTB_divisor;
	/*  12: SDRAM Minimum Cycle Time (tCKmin) */
	uint8_t tCKmin;
	/*  13: Reserved */
	uint8_t rsvd_13;
	/*  14: CAS Latencies Supported, LSB */
	uint8_t cas_latency_lsb;
	/*  15: CAS Latencies Supported, MSB */
	uint8_t cas_latency_msb;
	/*  16: Minimum CAS Latency Time (tAAmin) */
	uint8_t tAAmin;
	/*  17: Minimum Write Recovery Time (tWRmin) */
	uint8_t tWRmin;
	/*  18: Minimum RAS to CAS Delay Time (tRCDmin) */
	uint8_t tRCDmin;
	/*  19: Minimum Row Active to Row Active Delay Time (tRRDmin) */
	uint8_t tRRDmin;
	/*  20: Minimum Row Precharge Delay Time (tRPmin) */
	uint8_t tRPmin;
	/*  21: Upper Nibbles for tRAS and tRC */
	uint8_t tRAS_tRC_msn;
	/*  22: Minimum Active to Precharge Delay Time (tRASmin), LSB */
	uint8_t tRASmin_lsb;
	/*  23: Minimum Active to Active/Refresh Delay Time (tRCmin), LSB */
	uint8_t tRCmin_lsb;
	/*  24: Minimum Refresh Recovery Delay Time (tRFC1min), LSB */
	uint8_t tRFCmin_lsb;
	/*  25: Minimum Refresh Recovery Delay Time (tRFC1min), MSB */
	uint8_t tRFCmin_msb;
	/*  26: Minimum Internal Write to Read Command Delay Time (tWTRmin) */
	uint8_t tWTRmin;
	/*  27: Minimum Internal Read to Precharge Command Delay Time
	 *      (tRTPmin)
	 */
	uint8_t tRTPmin;
	/*  28: Upper Nibble for tFAWmin, MSN */
	uint8_t tFAWmin_msn;
	/*  29: Minimum Four Activate Window Time (tFAWmin) */
	uint8_t tFAWmin_lsb;
	/*  30: SDRAM Optional Features */
	uint8_t opt_feature;
	/*  31: SDRAM Thernal and Refresh Options */
	uint8_t thermal_refresh;
	/*  32: Module Thermal Sensor */
	uint8_t mod_tsens;
	/*  33: SDRAM Device Type */
	uint8_t dev_type;
	/*  34: Fine Offset for SDRAM Minimum Cycle Time (tCKmax) */
	int8_t fine_tCKmin;
	/*  35: Fine Offset for Minimum CAS Latency Time (tAAmin) */
	int8_t fine_tAAmin;
	/*  36: Fine Offset for Minimum RAS to CAS Delay Time (tRCDmin) */
	int8_t fine_tRCDmin;
	/*  37: Fine Offset for Minimum Row Precharge Delay Time (tRPmin) */
	int8_t fine_tRPmin;
	/*  38: Fine Offset for Minimum Activate to Activate/Refresh Delay
	 *      Time (tRCmin)
	 */
	int8_t fine_tRCmin;
	/* 39-59: Reserved */
	uint8_t rsvd_39[19];
	/* 60-116: Module Type Specific Section, Indexed by Key Byte 3 */
	union {
		struct ddr3_spd_unbufferred_eeprom unbufferred;
		struct ddr3_spd_registered_eeprom registered;
	} mod;
	/* 117-118: Manufacturer’s JEDEC ID Code (JEP-106) */
	uint8_t jedec_id[2];
	/* 119: Module Manufacturing Location */
	uint8_t mod_loc;
	/* 120-121: Module Manufacturing Date */
	uint8_t mod_date[2];
	/* 122-125: Module Serial Number */
	uint8_t mod_sn[4];
	/* 126-127: Cyclical Redundancy Code */
	uint8_t crc[2];
	/* 128-145: Module Part Number */
	uint8_t mod_part[8];
	/* 146-147: Module Revision Code */
	uint8_t mod_rev[2];
	/* 148-149: DRAM Manufacturer's ID Code, LSB */
	uint8_t dram_id[2];
	/* 150-175: Manufacturer's Specific Data */
	uint8_t mod_spec_data[26];
	/* 176:255: Open for customer use */
	uint8_t cust[80];
} __packed;

/* Annex L.1: Module Specific Bytes for Unbuffered Memory Module Types
 */
struct ddr4_spd_unbufferred_eeprom {
	/* 128: Raw Card Extension, Module Nominal Height */
	uint8_t mod_height;
	/* 129: Module Maximum Thickness */
	uint8_t mod_thickness;
	/* 130: Reference Raw Card Used */
	uint8_t ref_raw_card;
	/* 131: Address Mapping from Edge Connector to DRAM */
	uint8_t addr_mapping;
	/* 132-253: Reserved */
	uint8_t rsvd_132[122];
	/* 254: CRC for Module Specific Section, LSB */
	uint8_t crc_lsb;
	/* 255: CRC for Module Specific Section, MSB */
	uint8_t crc_msb;
} __packed;

/* Annex L.2: Module Specific Bytes for Registered Memory Module Types
 */
struct ddr4_spd_registered_eeprom {
	/* 128: Raw Card Extension, Module Nominal Height */
	uint8_t mod_height;
	/* 129: Module Maximum Thickness */
	uint8_t mod_thickness;
	/* 130: Reference Raw Card Used */
	uint8_t ref_raw_card;
	/* 131: DIMM Module Attributes */
	uint8_t dimm_mod_attr;
	/* 132: RDIMM Thermal Heat Spreader Solution */
	uint8_t rdimm_thermal;
	/* 133: Register Manufacturer ID Code, LSB */
	uint8_t reg_id_lsb;
	/* 134: Register Manufacturer ID Code, MSB */
	uint8_t reg_id_msb;
	/* 135: Register Revision Number */
	uint8_t reg_rev;
	/* 136: Address mapping from register to DRAM */
	uint8_t addr_mapping;
	/* 137-253: Reserved */
	uint8_t rsvd_137[117];
	/* 254: CRC for Module Specific Section, LSB */
	uint8_t crc_lsb;
	/* 255: CRC for Module Specific Section, MSB */
	uint8_t crc_msb;
} __packed;

/* Annex L.3: Module Specific Bytes for Load Reduction Memory Module Types
 */
struct ddr4_spd_load_reduced_eeprom {
	/* 128: Raw Card Extension, Module Nominal Height */
	uint8_t mod_height;
	/* 129: Module Maximum Thickness */
	uint8_t mod_thickness;
	/* 130: Reference Raw Card Used */
	uint8_t ref_raw_card;
	/* 131: DIMM Module Attributes */
	uint8_t dimm_mod_attr;
	/* 132: LRDIMM Thermal Heat Spreader Solution */
	uint8_t lrdimm_thermal;
	/* 133: Register and Data Buffer Manufacturer ID Code, LSB */
	uint8_t reg_id_lsb;
	/* 134: Register and Data Buffer Manufacturer ID Code, MSB */
	uint8_t reg_id_msb;
	/* 135: Register Revision Number */
	uint8_t reg_rev;
	/* 136: Address Mapping from Register to DRAM */
	uint8_t addr_mapping;
	/* 137: Register Output Drive Strength for Control and
	 *      Command/Address
	 */
	uint8_t reg_drv;
	/* 138: Register Output Drive Strength for CK */
	uint8_t reg_drv_ck;
	/* 139: Data Buffer Revision Number */
	uint8_t dbuf_rev;
	/* 140: DRAM VrefDQ for Package Rank 0 */
	uint8_t dram_VrefDQ_r0;
	/* 141: DRAM VrefDQ for Package Rank 1 */
	uint8_t dram_VrefDQ_r1;
	/* 142: DRAM VrefDQ for Package Rank 2 */
	uint8_t dram_VrefDQ_r2;
	/* 143: DRAM VrefDQ for Package Rank 3 */
	uint8_t dram_VrefDQ_r3;
	/* 144: Data Buffer VrefDQ for DRAM Interface */
	uint8_t dbuf_VrefDQ_dram;
	/* 145: Data Buffer MDQ Drive Strength and RTT
	 *      for data rate <= 1866
	 */
	uint8_t dbuf_MDQ_drv_1866;
	/* 146: Data Buffer MDQ Drive Strength and RTT
	 *      for 1866 < data rate <= 2400
	 */
	uint8_t dbuf_MDQ_drv_2400;
	/* 147: Data Buffer MDQ Drive Strength and RTT
	 *      for 2400 < data rate <= 3200
	 */
	uint8_t dbuf_MDQ_drv_3200;
	/* 148 DRAM Drive Strength
	 *     (for data rates <= 1866, 1866 < data rate < 2400, and
	 *      2400 < data rate <= 3200)
	 */
	uint8_t dram_drv;
	/* 149: DRAM ODT (RTT_WR, RTT_NOM)
	 *      for data rate <= 1866
	 */
	uint8_t dram_ODT_1866;
	/* 150: DRAM ODT (RTT_WR, RTT_NOM)
	 *      for 1866 < data rate <= 2400
	 */
	uint8_t dram_ODT_2400;
	/* 151: DRAM ODT (RTT_WR, RTT_NOM)
	 *      for 2400 < data rate <= 3200
	 */
	uint8_t dram_ODT_3200;
	/* 152: DRAM ODT (RTT_PARK)
	 *      for data rate <= 1866
	 */
	uint8_t dram_ODT_park_1866;
	/* 153: DRAM ODT (RTT_PARK)
	 *      for 1866 < data rate <= 2400
	 */
	uint8_t dram_ODT_park_2400;
	/* 154: DRAM ODT (RTT_PARK)
	 *      for 2400 < data rate <= 3200
	 */
	uint8_t dram_ODT_park_3200;
	/* 155-253: Reserved */
	uint8_t rsvd_155[99];
	/* 254: CRC for Module Specific Section, LSB */
	uint8_t crc_lsb;
	/* 255: CRC for Module Specific Section, MSB */
	uint8_t crc_msb;
} __packed;

/* JEDEC Standard No. 21-C
 * Annex L: Serial Presence Detect (SPD) for DDR4 SDRAM Modules
 */
struct ddr4_spd_eeprom {
	/* Block 0: Base Configuration and DRAM Parameters */
	/*   0: Number of Serial PD Bytes Written / SPD Device Size / CRC
	 *      Coverage
	 */
	uint8_t spd_size_crc;
	/*   1: SPD Revision */
	uint8_t spd_rev;
	/*   2: Key Byte/DRAM Device Type */
	uint8_t mem_type;
	/*   3: Key Byte/Module Type */
	uint8_t mod_type;
	/*   4: SDRAM Density and Banks */
	uint8_t density_banks;
	/*   5: SDRAM Addressing */
	uint8_t addressing;
	/*   6: SDRAM Package Type */
	uint8_t package_type;
	/*   7: SDRAM Optional Features */
	uint8_t opt_feature;
	/*   8: SDRAM Thernal and Refresh Options */
	uint8_t thermal_refresh;
	/*   9: Other SDRAM Optional Features */
	uint8_t other_opt_features;
	/*  10: Reserved */
	uint8_t rsvd_10;
	/*  11: Module Nominal Voltage, VDD */
	uint8_t mod_VDD;
	/*  12: Module Organization */
	uint8_t mod_org;
	/*  13: Module Memory Bus Width */
	uint8_t mod_bus_width;
	/*  14: Module Thermal Sensor */
	uint8_t mod_tsens;
	/* 15-16: Reserved */
	uint8_t rsvd_15[2];
	/*  17: Timebases */
	uint8_t timebases;
	/*  18: SDRAM Minimum Cycle Time (tCKAVGmin) */
	uint8_t tCKAVGmin;
	/*  19: SDRAM Maximum Cycle Time (tCKAVGmax) */
	uint8_t tCKAVGmax;
	/*  20: CAS Latencies Supported, First Byte */
	uint8_t cas_latency_1;
	/*  21: CAS Latencies Supported, Second Byte */
	uint8_t cas_latency_2;
	/*  22: CAS Latencies Supported, Third Byte */
	uint8_t cas_latency_3;
	/*  23: CAS Latencies Supported, Fourth Byte */
	uint8_t cas_latency_4;
	/*  24: Minimum CAS Latency Time (tAAmin) */
	uint8_t tAAmin;
	/*  25: Minimum RAS to CAS Delay Time (tRCDmin) */
	uint8_t tRCDmin;
	/*  26: Minimum Row Precharge Delay Time (tRPmin) */
	uint8_t tRPmin;
	/*  27: Upper Nibbles for tRAS and tRC */
	uint8_t tRAS_tRC_msn;
	/*  28: Minimum Active to Precharge Delay Time (tRASmin), LSB */
	uint8_t tRASmin_lsb;
	/*  29: Minimum Active to Active/Refresh Delay Time (tRCmin), LSB */
	uint8_t tRCmin_lsb;
	/*  30: Minimum Refresh Recovery Delay Time (tRFC1min), LSB */
	uint8_t tRFC1min_lsb;
	/*  31: Minimum Refresh Recovery Delay Time (tRFC1min), MSB */
	uint8_t tRFC1min_msb;
	/*  32: Minimum Refresh Recovery Delay Time (tRFC2min), LSB */
	uint8_t tRFC2min_lsb;
	/*  33: Minimum Refresh Recovery Delay Time (tRFC2min), MSB */
	uint8_t tRFC2min_msb;
	/*  34: Minimum Refresh Recovery Delay Time (tRFC4min), LSB */
	uint8_t tRFC4min_lsb;
	/*  35: Minimum Refresh Recovery Delay Time (tRFC4min), MSB */
	uint8_t tRFC4min_msb;
	/*  36: Minimum Four Activate Window Time (tFAWmin), MSN */
	uint8_t tFAWmin_msn;
	/*  37: Minimum Four Activate Window Time (tFAWmin), LSB */
	uint8_t tFAWmin_lsb;
	/*  38: Minimum Activate to Activate Delay Time (tRRD_Smin),
	 *      different bank group
	 */
	uint8_t tRRD_Smin;
	/*  39: Minimum Activate to Activate Delay Time (tRRD_Lmin),
	 *      same bank group
	 */
	uint8_t tRRD_Lmin;
	/*  40: Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank
	 *      group
	 */
	uint8_t tCCD_Lmin;
	/* 41-59: Reserved */
	uint8_t rsvd_41[19];
	/* 60-77: Connector to SRAM Bit Mapping */
	uint8_t conn_sram_bitmap[18];
	/* 78-116: Reserved */
	uint8_t rsvd_78[39];
	/* 117: Fine Offset for Minimum CAS to CAS Delay Time (tCCD_Lmin),
	 *      same bank group
	 */
	int8_t fine_tCCD_Lmin;
	/* 118: Fine Offset for Minimum Activate to Activate Delay Time
	 *      (tRRD_Lmin), same bank group
	 */
	int8_t fine_tRRD_Lmin;
	/* 119: Fine Offset for Minimum Activate to Activate Delay Time
	 *      (tRRD_Smin), different bank group
	 */
	int8_t fine_tRRD_Smin;
	/* 120: Fine Offset for Minimum Activate to Activate/Refresh Delay
	 *      Time (tRCmin)
	 */
	int8_t fine_tRCmin;
	/* 121: Fine Offset for Minimum Row Precharge Delay Time (tRPmin) */
	int8_t fine_tRPmin;
	/* 122: Fine Offset for Minimum RAS to CAS Delay Time (tRCDmin) */
	int8_t fine_tRCDmin;
	/* 123: Fine Offset for Minimum CAS Latency Time (tAAmin) */
	int8_t fine_tAAmin;
	/* 124: Fine Offset for SDRAM Maximum Cycle Time (tCKAVGmax) */
	int8_t fine_tCKAVGmax;
	/* 125: Fine Offset for SDRAM Minimum Cycle Time (tCKAVGmax) */
	int8_t fine_tCKAVGmin;
	/* 126: CRC for Base Configuration Section, LSB */
	uint8_t crc_lsb;
	/* 126: CRC for Base Configuration Section, MSB */
	uint8_t crc_msb;

	/* Block 1: Module Specific Parameters
	 * Module-Specific Sections: Bytes 128~255 (0x080~0x0FF)
	 */
	union {
		struct ddr4_spd_unbufferred_eeprom unbufferred;
		struct ddr4_spd_registered_eeprom registered;
		struct ddr4_spd_load_reduced_eeprom load_reduced;
		uint8_t raw[128];
	} mod;

	/* Block 2, low half: Reserved */
	uint8_t rsvd_256[64];

	/* Block 2, upper half: Manufacturing Information */
	/* 320: Module Manufacturer’s ID Code, LSB */
	uint8_t mod_id_lsb;
	/* 321: Module Manufacturer’s ID Code, MSB */
	uint8_t mod_id_msb;
	/* 322: Module Manufacturing Location */
	uint8_t mod_loc;
	/* 323-324: Module Manufacturing Date */
	uint8_t mod_date[2];
	/* 325-328: Module Serial Number */
	uint8_t mod_sn[4];
	/* 329-348: Module Part Number */
	uint8_t mod_part[20];
	/* 349: Module Revision Code */
	uint8_t mod_rev;
	/* 350: DRAM Manufacturer's ID Code, LSB */
	uint8_t dram_id_lsb;
	/* 351: DRAM Manufacturer's ID Code, MSB */
	uint8_t dram_id_msb;
	/* 352: DRAM Stepping */
	uint8_t dram_stepping;
	/* 353-381: Module Manufacturer's Specific Data */
	uint8_t mod_spec_data[29];
	/* 382-383: Reserved */
	uint8_t rsvd_382[2];

	/* Block 3: End User Programmable */
	uint8_t user[128];
} __packed;

union ddr_spd_eeprom {
	struct ddr1_spd_eeprom ddr1;
	struct ddr2_spd_eeprom ddr2;
	struct ddr3_spd_eeprom ddr3;
	struct ddr4_spd_eeprom ddr4;
	struct ddr_rev {
		uint8_t spd_size;
		uint8_t spd_rev;
	} ddr;
} __packed;

/*
 * Byte 2 Fundamental Memory Types.
 */
#define SPD_MEMTYPE_FPM		(0x01)
#define SPD_MEMTYPE_EDO		(0x02)
#define SPD_MEMTYPE_PIPE_NIBBLE	(0x03)
#define SPD_MEMTYPE_SDRAM	(0x04)
#define SPD_MEMTYPE_ROM		(0x05)
#define SPD_MEMTYPE_SGRAM	(0x06)
#define SPD_MEMTYPE_DDR		(0x07)
#define SPD_MEMTYPE_DDR2	(0x08)
#define SPD_MEMTYPE_DDR2_FBDIMM	(0x09)
#define SPD_MEMTYPE_DDR2_FBDIMM_PROBE	(0x0A)
#define SPD_MEMTYPE_DDR3	(0x0B)
#define SPD_MEMTYPE_DDR4	(0x0C)

/* DIMM Type for DDR2 SPD (according to v1.3) */
#define DDR2_SPD_DIMMTYPE_UNDEFINED	(0x00)
#define DDR2_SPD_DIMMTYPE_RDIMM		(0x01)
#define DDR2_SPD_DIMMTYPE_UDIMM		(0x02)
#define DDR2_SPD_DIMMTYPE_SO_DIMM	(0x04)
#define DDR2_SPD_DIMMTYPE_72B_SO_CDIMM	(0x06)
#define DDR2_SPD_DIMMTYPE_72B_SO_RDIMM	(0x07)
#define DDR2_SPD_DIMMTYPE_MICRO_DIMM	(0x08)
#define DDR2_SPD_DIMMTYPE_MINI_RDIMM	(0x10)
#define DDR2_SPD_DIMMTYPE_MINI_UDIMM	(0x20)

/* Byte 3 Key Byte / Module Type for DDR3 SPD */
#define DDR3_SPD_MODULETYPE_MASK	(0x0f)
#define DDR3_SPD_MODULETYPE_RDIMM	(0x01)
#define DDR3_SPD_MODULETYPE_UDIMM	(0x02)
#define DDR3_SPD_MODULETYPE_SO_DIMM	(0x03)
#define DDR3_SPD_MODULETYPE_MICRO_DIMM	(0x04)
#define DDR3_SPD_MODULETYPE_MINI_RDIMM	(0x05)
#define DDR3_SPD_MODULETYPE_MINI_UDIMM	(0x06)
#define DDR3_SPD_MODULETYPE_MINI_CDIMM	(0x07)
#define DDR3_SPD_MODULETYPE_72B_SO_UDIMM	(0x08)
#define DDR3_SPD_MODULETYPE_72B_SO_RDIMM	(0x09)
#define DDR3_SPD_MODULETYPE_72B_SO_CDIMM	(0x0A)
#define DDR3_SPD_MODULETYPE_LRDIMM	(0x0B)
#define DDR3_SPD_MODULETYPE_16B_SO_DIMM	(0x0C)
#define DDR3_SPD_MODULETYPE_32B_SO_DIMM	(0x0D)

/* DIMM Type for DDR4 SPD */
#define DDR4_SPD_MODULETYPE_MASK	(0x0f)
#define DDR4_SPD_MODULETYPE_EXT		(0x00)
#define DDR4_SPD_MODULETYPE_RDIMM	(0x01)
#define DDR4_SPD_MODULETYPE_UDIMM	(0x02)
#define DDR4_SPD_MODULETYPE_SO_DIMM	(0x03)
#define DDR4_SPD_MODULETYPE_LRDIMM	(0x04)
#define DDR4_SPD_MODULETYPE_MINI_RDIMM	(0x05)
#define DDR4_SPD_MODULETYPE_MINI_UDIMM	(0x06)
#define DDR4_SPD_MODULETYPE_72B_SO_UDIMM	(0x08)
#define DDR4_SPD_MODULETYPE_72B_SO_RDIMM	(0x09)
#define DDR4_SPD_MODULETYPE_16B_SO_DIMM	(0x0C)
#define DDR4_SPD_MODULETYPE_32B_SO_DIMM	(0x0D)

#define SPD_BLOCK_SIZE                          0x100
#define SPD_MAX_SERIAL_EEPROM_SIZE              (2 * SPD_BLOCK_SIZE)

#if 0
// SPD Module Values Common to DDR3 &  DDR4
#define SPD_MODULE_TYPE_RDIMM                   0x1
#define SPD_MODULE_TYPE_UDIMM                   0x2
#define SPD_MODULE_SODIMM                       0x3
#define SPD_MODULE_TYPE_DDR4_LRDIMM             0x4
#define SPD_MODULE_SOUDIMM                      0x8
#define SPD_MODULE_TYPE_DDR3_LRDIMM             0xB

// SPD device width Common to DDR3 &  DDR4
#define SPD_SDRAM_WIDTH_X4                      0x4
#define SPD_SDRAM_WIDTH_X8                      0x8
#define SPD_SDRAM_WIDTH_X16                     0x10
#define SPD_SDRAM_WIDTH_X32                     0x20

// SPD module width Common to DDR3 &  DDR4
#define SPD_MODULE_WIDTH                        64

// SPD maximum supported number of ranks Common to DDR3 &  DDR4
#define SPD_MAX_RANKS_SUPPORTED_2DPC            2
#define SPD_MAX_RANKS_SUPPORTED                 4
#define SPD_UNSUPPORTED_RANK_VAL                3    // MC does not support 3 rank DIMMs
#define SPD_INVALID_RANK_COUNT                  0xFF // Marker value for number of ranks indicating an invalid configuration.

// SPD Row Bits range
#define SPD_MIN_ROWS                            12
#define SPD_MAX_ROWS                            18

// SPD Column Bits range
#define SPD_MIN_COLUMNS                         10
#define SPD_MAX_COLUMNS                         12

// Minimum DIMM size supported in MB
#define SPD_MIN_DIMM_SIZE                       0x400 // 1GB

// Max DIMM size supported in MB
#define SPD_MAX_DIMM_SIZE                       0x20000 // 128 GB

// Max DIMM size supported in MB
#define SPD_MAX_DIMM_SIZE_2DPC                  0x10000 // 64 GB

// Max Channel size supported in GB
#define SPD_MAX_CHANNEL_SIZE                    0x80 // 128 GB

// RDIMM Signal Strengths
#define RDIMM_LIGHT_DRIVE                       0
#define RDIMM_MODERATE_DRIVE                    1
#define RDIMM_STRONG_DRIVE                      2
#define RDIMM_INVALID_DRIVE                     3

// RDIMM marker value for combined invalid configuration words.
#define RDIMM_INVALID_CWS                       0xFFFFFFFFFFFFFFFF

// Memory attribute values used to populate the SPD type_detail bit-field.
#define MEM_ATTR_TYPE_DETAIL_RESERVED_0         (0x1 << 0)
#define MEM_ATTR_TYPE_DETAIL_OTHER              (0x1 << 1)
#define MEM_ATTR_TYPE_DETAIL_UNKNOWN            (0x1 << 2)
#define MEM_ATTR_TYPE_DETAIL_FAST_PAGED         (0x1 << 3)
#define MEM_ATTR_TYPE_DETAIL_STATIC_COLUMN      (0x1 << 4)
#define MEM_ATTR_TYPE_DETAIL_PSEUDO_STATIC      (0x1 << 5)
#define MEM_ATTR_TYPE_DETAIL_RAMBUS             (0x1 << 6)
#define MEM_ATTR_TYPE_DETAIL_SYNCHRONOUS        (0x1 << 7)
#define MEM_ATTR_TYPE_DETAIL_CMOS               (0x1 << 8)
#define MEM_ATTR_TYPE_DETAIL_EDO                (0x1 << 9)
#define MEM_ATTR_TYPE_DETAIL_WINDOW_DRAM        (0x1 << 10)
#define MEM_ATTR_TYPE_DETAIL_CACHE_DRAM         (0x1 << 11)
#define MEM_ATTR_TYPE_DETAIL_NON_VOLATILE       (0x1 << 12)
#define MEM_ATTR_TYPE_DETAIL_REGISTERED         (0x1 << 13)
#define MEM_ATTR_TYPE_DETAIL_UNBUFFERED         (0x1 << 14)
#define MEM_ATTR_TYPE_DETAIL_RESERVED_15        (0x1 << 15)

// Module Manufacturer ID - Micron
#define SPD_MODULE_MANUFACTURER_ID_MICRON       0x2C80
#endif

/* Module part number length in bytes */
#define SPD_MODULE_PART_NUM_LEN_DDR3		18
#define SPD_MODULE_PART_NUM_LEN_DDR4		20
#define SPD_MODULE_PART_NUM_LEN			SPD_MODULE_PART_NUM_LEN_DDR4
#define SPD_MODULE_PART_NUM_LEN_STR		(SPD_MODULE_PART_NUM_LEN + 1)

/* DDR SPD information:
 *
 * revision             The revision of the SPD.
 * dev_type             Device type. DDR3 or DDR4.
 * mod_type             DIMM module type. UDIMM, RDIMM, SODIMM or SOUDIMM.
 * num_ranks            Number of ranks on the the DIMM.
 * sdram_width          SDRAM device width indicator.
 * has_ecc              Boolean indication of if the DIMM has ECC.
 * dimm_module_width    Width in bits of the DIMM module, excluding ECC
 *                      bits.
 * num_bank_groups      Bank group indication value. DDR4 only.
 * device_capacity      Capacity of a single device on the DIMM.
 * num_rows             Number of rows.
 * num_columns          Number of columns.
 * die_count            Number of dies on the DIMM.
 * is_3DS               Indication of if the DIMM is 3DS stacked.
 * ca_mirrored          Indication of if the DIMM is CA mirrored.
 * dimm_size_mb         Size of the DIMM in MB.
 * has_therm_sensor     Indication of if the DIMM incorporates a thermal
 *                      sensor.
 * is_rank_mix          Indication of if the DIMM has mixed ranks
 *                      densities.
 * is_voltage_ok        Indication of if the DIMM is operable at the
 *                      expected voltage.
 * ftb_fs               Fine timebase in fs, femtoseconds.
 * mtb_ps               Medium timebase in ps.
 * cas_lat_sup          CAS latencies supported flags.
 * taa_min_ps           Minimum CAS Latency Time in ps.
 * tck_min_ps           SDRAM Minimum Cycle Time in ps.
 * tck_max_ps           SDRAM Maximum Cycle Time in ps.
 * trcd_min_ps          Minimum RAS to CAS Delay Time in ps.
 * trp_min_ps           Minimum Row Precharge Delay Time in ps.
 * tras_min_ps          Minimum Active to Precharge Delay Time in ps.
 * trc_min_ps           Minimum Active to Active/Refresh Delay Time in ps.
 * tfaw_min_ps          Minimum Four Activate Window Time in ps.
 * twr_min_ps           Minimum Write Recovery Time in ps.
 * twtr_min_ps          Minimum Internal Write to Read Command Delay Time
 *                      in ps.
 * out_drv_ck_y13       RDIMM / LRDIMM clock driver Y13 signal output
 *                      strength.
 * out_drv_ck_y02       RDIMM / LRDIMM clock driver Y02 signal output
 *                      strength.
 *=========================================================================
 * DDR3 only parameters:
 * trrd_min_ps          Minimum Row Active to Row Active Delay Time in ps.
 * trtp_min_ps          Minimum Internal Read to Precharge Command Delay
 *                      Time in ps.
 * trfc_min_ps          Minimum Refresh Recovery Delay Time in ps.
 * rdm_out_drv_ctl_a    RDIMM driver output strength for control signal A.
 * rdm_out_drv_ctl_b    RDIMM driver output strength for control signal B.
 * rdm_out_drv_ca_a     RDIMM drive output strength for commands/addresses
 *                      signal A.
 * rdm_out_drv_ca_b     RDIMM drive output strength for commands/addresses
 *                      signal B.
 * rdm_ddr3_cw15_0      RDIMM configuration words. DDR3 only.
 *=========================================================================
 * DDR4 only parameters:
 * trrdl_min_ps         Minimum Activate to Activate Delay Time, same bank
 *                      group, in ps.
 * trrds_min_ps         Minimum Activate to Activate Delay Time, different
 *                      bank group in ps.
 * tccdl_min_ps         Minimum CAS to CAS Delay Time, same bank group.
 * trfc1_min_ps         Minimum Refresh Recovery Delay Time - RFC1 in ps.
 * trfc2_min_ps         Minimum Refresh Recovery Delay Time - RFC2 in ps.
 * trfc4_min_ps         Minimum Refresh Recovery Delay Time - RFC4 in ps.
 * twtrs_min_ps         Delay from start of internal write trans-action to
 *                      internal read command for dif-ferent bank group.
 * out_drv_ctl_cke      RDIMM / LRDIMM drive output strength for the CKE
 *                      signal.
 * out_drv_ctl_odt      RDIMM / LRDIMM drive output strength for the ODT
 *                      signal.
 * out_drv_ctl_ca       RDIMM / LRDIMM drive output strength for the
 *                      commands/addresses signal.
 * out_drv_ctl_cs       RDIMM / LRDIMM drive output strength for the chip
 *                      select signal.
 *=========================================================================
 * type_detail          Bit field that contains memory attributes.
 * max_clk_speed        Maximum speed of the memory (MHz).
 * manufacturer_id      ID specifying the memory manufacturer.
 * serial_number        The serial number of the memory device.
 * part_number          Byte array specifying the part number.
 */
typedef struct ddr_spd {
	uint8_t revision;
	uint8_t dev_type;
	uint8_t mod_type;
	uint8_t num_ranks;
	uint8_t sdram_width;
	bool has_ecc;
	uint8_t dimm_module_width;
	uint8_t num_bank_groups;
	uint16_t device_capacity;
	uint8_t num_rows;
	uint8_t num_columns;
	uint8_t die_count;
	bool is_3DS;
	bool ca_mirrored;
	uint32_t dimm_size_mb;
	bool has_therm_sensor;
	bool is_rank_mix;
	bool is_voltage_ok;
	uint32_t ftb_fs;
	uint32_t mtb_ps;
	uint32_t cas_lat_sup;
	uint32_t taa_min_ps;
	uint32_t tck_min_ps;
	uint32_t tck_max_ps;
	/* Common Timing Values */
	uint32_t trcd_min_ps;
	uint32_t trp_min_ps;
	uint32_t tras_min_ps;
	uint32_t trc_min_ps;
	uint32_t tfaw_min_ps;
	uint32_t twr_min_ps;
	uint32_t twtr_min_ps;
	uint8_t out_drv_ck_y13;
	uint8_t out_drv_ck_y02;
	/* DDR3 Only Timing Values */
	uint32_t trrd_min_ps;
	uint32_t trtp_min_ps;
	uint32_t trfc_min_ps;
	/* DDR3 Only RDIMM Values */
	uint8_t rdm_out_drv_ctl_a;
	uint8_t rdm_out_drv_ctl_b;
	uint8_t rdm_out_drv_ca_a;
	uint8_t rdm_out_drv_ca_b;
	uint64_t rdm_ddr3_cw15_0;
	/* DDR4 Only Timing Values */
	uint32_t trrdl_min_ps;
	uint32_t trrds_min_ps;
	uint32_t tccdl_min_ps;
	uint32_t trfc1_min_ps;
	uint32_t trfc2_min_ps;
	uint32_t trfc4_min_ps;
	uint32_t twtrs_min_ps;
	/* DDR4 RDIMM / LRDIMM Values */
	uint8_t out_drv_ctl_cke;
	uint8_t out_drv_ctl_odt;
	uint8_t out_drv_ctl_ca;
	uint8_t out_drv_ctl_cs;
	/* DDR4 Only LRDIMM Values */
	uint8_t lrdm_db_drv_ck_bcom_bodt_bcke;
	uint8_t lrdm_db_drv_ck_bck;
	/* DIMM Identification */
	uint16_t type_detail;
	uint16_t max_clk_speed;
	uint16_t manufacturer_id;
	uint32_t serial_number;
	uint8_t  part_number[SPD_MODULE_PART_NUM_LEN_STR];
} ddr_spd_t;

#endif /* __DDR_SPD_H_INCLUDE__ */
