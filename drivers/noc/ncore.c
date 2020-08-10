/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)ncore.c: Arteris Ncore interconnect implementation
 * $Id: ncore.c,v 1.0 2020-08-10 13:30:00 zhenglv Exp $
 */

#include <target/noc.h>

void ncore_init(void)
{
	uint8_t nr_mems, i;
	uint8_t nr_caius, nr_ncbus, nr_dirus, nr_cmius;

	nr_mems = ncore_num_sfs();
	nr_caius = ncore_num_caius();
	nr_ncbus = ncore_num_ncbus();
	nr_dirus = ncore_num_dirus();
	nr_cmius = ncore_num_cmius();

	printf("Release version: %d\n", ncore_release_version());
	printf("Directory cacheline offset: %d\n", ncore_dir_cacheline_offset());
	printf("Number of memory regions: %d\n", nr_mems);
	printf("Number of CAIU: %d\n", nr_caius);
	printf("Number of NCBU: %d\n", nr_ncbus);
	printf("Number of DIRU: %d\n", nr_dirus);
	printf("Number of CMIU: %d\n", nr_cmius);
	for (i = 0; i < nr_mems; i++) {
		printf("Snoop filter type: %d\n", ncore_sf_type(i));
		printf("Snoop filter number of ways: %d\n", ncore_sf_num_ways(i));
		printf("Snoop filter number of sets: %d\n", ncore_sf_num_sets(i));
	}
	for (i = 0; i < NCORE_MAX_SUS; i++) {
		printf("SU %d implementation version: %d\n",
		       ncore_su_i2t(i), ncore_su_impl_ver(i));
		switch (ncore_su_i2t(i)) {
		case NCORE_SU_CAIU:
			if ((i - ncore_su_t2i(NCORE_SU_CAIU)) >= nr_caius)
				continue;
			printf("CAIU id: %d\n", ncore_caiu_id(i));
			printf("CAIU type: %d\n", ncore_caiu_type(i));
			printf("CAIU caching agent: %d\n", ncore_caiu_ca(i));
			printf("CAIU snoop filter id: %d\n", ncore_caiu_sf_id(i));
			break;
		case NCORE_SU_NCBU:
			if ((i - ncore_su_t2i(NCORE_SU_NCBU)) >= nr_ncbus)
				continue;
			printf("NCBU id: %d\n", ncore_ncbu_id(i));
			printf("NCBU type: %d\n", ncore_ncbu_type(i));
			printf("NCBU caching agent: %d\n", ncore_ncbu_ca(i));
			printf("NCBU snoop filter id: %d\n", ncore_ncbu_sf_id(i));
			printf("NCBU PC type: %d\n", ncore_ncbu_pc_type(i));
			printf("NCBU PC number of ways: %d\n",
			       ncore_ncbu_pc_num_ways(i));
			printf("NCBU PC number of sets: %d\n",
			       ncore_ncbu_pc_num_sets(i));
			break;
		case NCORE_SU_DIRU:
			if ((i - ncore_su_t2i(NCORE_SU_DIRU)) >= nr_dirus)
				continue;
			break;
		case NCORE_SU_CMIU:
			if ((i - ncore_su_t2i(NCORE_SU_CMIU)) >= nr_cmius)
				continue;
			printf("CMIU id: %d\n", ncore_cmiu_id(i));
			printf("CMIU hint capable: %d\n", ncore_cmiu_hnt_cap(i));
			printf("CMIU CMC (coherent memory cache): %d\n", ncore_cmiu_cmc(i));
			if (ncore_cmiu_cmc(i)) {
				printf("CMIU CMC type: %d\n", ncore_cmiu_cmc_type(i));
				printf("CMIU CMC number of ways: %d\n",
				       ncore_cmiu_cmc_num_ways(i));
				printf("CMIU CMC number of sets: %d\n",
				       ncore_cmiu_cmc_num_sets(i));
			}
			break;
		}
	}
}
