/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2010
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
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
 * @(#)scsi_trgt.c: small computer storage interface target functions
 * $Id: scsi_trgt.c,v 1.409 2011-04-20 09:55:42 zhenglv Exp $
 */

#include <target/scsi.h>
#include <target/state.h>

scsi_lun_t scsi_nr_luns = 0;
scsi_device_t *scsi_devices[NR_SCSI_UNITS];
scsi_lun_t scsi_lun = INVALID_SCSI_LUN;

scsi_pid_t scsi_nr_pids = 0;
scsi_transport_t *scsi_transports[NR_SCSI_PORTS];

uint8_t scsi_nr_vpds = 0;
scsi_vpd_page_t *scsi_vpd_pages[NR_SCSI_VPDS];
uint8_t scsi_vpd_codes[NR_SCSI_VPDS];
#if NR_SCSI_CMNDS > 1
struct scsi_cmnd scsi_cmnds[NR_SCSI_CMNDS];
#else
struct scsi_cmnd scsi_cmnd;
#endif
struct scsi_sense_data scsi_senses[NR_SCSI_SENSES];

struct scsi_target scsi_target;

void scsi_def_data_in(void)
{
	uint8_t *buffer_pos;
	bulk_size_t bulk_size;
	uint32_t i = 0, j, total_size;

	BUG_ON(!scsi_target_xprt);

	bulk_size = scsi_target_xprt->bulk_size(BULK_SIZE_OPT);
	total_size = scsi_current_cmnd.expect_length;
	buffer_pos = scsi_current_cmnd.data_buffer;

	while (total_size) {
		bulk_size_t xfr_size = (bulk_size_t)min(total_size, (uint32_t)bulk_size);
		scsi_target_xprt->open(i);
		for (j = 0; j < xfr_size; j++) {
			scsi_target_xprt->byte_in(buffer_pos[j]);
		}
		total_size -= xfr_size;
		i += xfr_size;
		buffer_pos += xfr_size;
		scsi_target_xprt->close(xfr_size);
	}
}

void scsi_def_data_out(void)
{
	uint8_t *buffer_pos;
	bulk_size_t bulk_size;
	uint32_t i = 0, j, total_size;

	BUG_ON(!scsi_target_xprt);

	bulk_size = scsi_target_xprt->bulk_size(BULK_SIZE_OPT);
	total_size = scsi_current_cmnd.expect_length;
	buffer_pos = scsi_current_cmnd.data_buffer;

	while (total_size) {
		bulk_size_t xfr_size = (bulk_size_t)min(total_size, (uint32_t)bulk_size);
		scsi_target_xprt->open(i);
		for (j = 0; j < xfr_size; j++) {
			buffer_pos[j] = scsi_target_xprt->byte_out();
		}
		total_size -= xfr_size;
		i += xfr_size;
		buffer_pos += xfr_size;
		scsi_target_xprt->close(xfr_size);
	}
}

/*=========================================================================
 * nexus related
 *=======================================================================*/
static void __scsi_cmnd_init(void)
{
	scsi_current_cmnd.state = SCSI_CMND_IDLE;
	scsi_current_cmnd.nexus = INVALID_SCSI_NEXUS;
	scsi_current_cmnd.sdd = INVALID_SCSI_SDD;
	scsi_current_cmnd.expect_length = 0;
	scsi_current_cmnd.expect_granularity = 0;
	scsi_current_cmnd.data_length = 0;
	scsi_current_cmnd.bulk_io = NULL;
	scsi_current_cmnd.dir = SCSI_CMND_OUT;
	scsi_set_status(SCSI_STATUS_GOOD);
	scsi_current_cmnd.next = NULL;
}

void scsi_command_aval(uint8_t dir)
{
	/* default data transfers will use internal buffer */
	if (scsi_current_cmnd.expect_length > SCSI_MAX_BUFFER) {
		scsi_invalid_cdb_field();
	} else {
		scsi_current_cmnd.state = SCSI_CMND_AVAL;
		scsi_set_direction(dir);
		if (dir == SCSI_CMND_IN) {
			scsi_current_cmnd.bulk_io = scsi_def_data_in;
		} else {
			scsi_current_cmnd.bulk_io = scsi_def_data_out;
		}
	}
}

void scsi_command_aval_in(void (*data_in)(void))
{
	scsi_command_aval(SCSI_CMND_IN);
	scsi_current_cmnd.bulk_io = data_in;
}

void scsi_command_aval_out(void (*data_out)(void))
{
	scsi_command_aval(SCSI_CMND_OUT);
	scsi_current_cmnd.bulk_io = data_out;
}

void scsi_command_cmpl(void)
{
	scsi_current_cmnd.state = SCSI_CMND_CMPL;
}

void scsi_command_done(uint8_t status)
{
	scsi_set_status(status);
	scsi_current_cmnd.state = SCSI_CMND_DONE;
}

static void scsi_cmnd_init(void)
{
	scsi_current_cmnd.crn = scsi_crn;
	scsi_current_cmnd.bulk = bulk_set_buffer(scsi_current_cmnd.data_buffer,
						 SCSI_MAX_BUFFER);
	__scsi_cmnd_init();
}

#ifdef CONFIG_SCSI_LINK
static void __scsi_link_init(void)
{
	scsi_target_task.linked = false;
	scsi_target_task.linked_tag = INVALID_SCSI_TAG;
}
#else
static void __scsi_link_init(void)
{
	scsi_target_task.linked = false;
}
#endif

static void __scsi_task_init(void)
{
	scsi_target_task.state = SCSI_TASK_ENDED;
	scsi_target_task.attr = SCSI_TASK_SIMPLE;
	scsi_target_task.lun = INVALID_SCSI_LUN;
	scsi_target_task.pid = INVALID_SCSI_PID;
	scsi_recent_cmnd = NULL;
	scsi_target_task.next = NULL;
	__scsi_link_init();
}

static void scsi_task_init(void)
{
	scsi_target_task.tag = scsi_tag;
	__scsi_task_init();
}

scsi_sdd_t scsi_sdd = INVALID_SCSI_SDD;

void __scsi_sense_data_init(void)
{
	BUG_ON(scsi_current_sense_data.next);
	scsi_current_sense_data.crn = INVALID_SCSI_CRN;
	scsi_set_sense_code(SCSI_SENSE_NO_SENSE,
			    SCSI_ASC_NO_ADDITIONAL_SENSE_INFO,
			    0);
	scsi_clear_sense_flags();
}

void scsi_sense_data_init(void)
{
	scsi_current_sense_data.sdd = scsi_sdd;
	scsi_current_sense_data.next = NULL;
	__scsi_sense_data_init();
}

void scsi_sense_data_exit(void)
{
	__scsi_sense_data_init();
}

void scsi_sense_data_restore(scsi_sdd_t sdd)
{
	scsi_sdd = sdd;
}

scsi_sdd_t scsi_sense_data_save(scsi_sdd_t sense)
{
	scsi_sdd_t ssdd = scsi_sdd;
	scsi_sense_data_restore(sense);
	return ssdd;
}

boolean scsi_sense_data_ready(void)
{
	scsi_sdd_t sdd;
	for (sdd = 0; sdd < NR_SCSI_SENSES; sdd++) {
		scsi_sense_data_select(sdd);
		if (scsi_current_sense_data.crn == INVALID_SCSI_CRN &&
		    scsi_current_sense_data.next == NULL) {
			scsi_current_cmnd.sdd = sdd;
			scsi_current_sense_data.crn = scsi_crn;
			return true;
		}
	}
	return false;
}

static void scsi_sense_data_enter(void)
{
	BUG_ON(scsi_current_sense_data.next);

	scsi_current_sense_data.next = scsi_recent_sense_data;
	scsi_recent_sense_data = &scsi_current_sense_data;
}

static void scsi_sense_data_leave(void)
{
	if (&scsi_current_sense_data == scsi_recent_sense_data) {
		scsi_recent_sense_data = scsi_current_sense_data.next;
	} else {
		struct scsi_sense_data *prev = scsi_recent_sense_data;
		while (prev && prev->next != &scsi_current_sense_data) {
			prev = prev->next;
		}
		BUG_ON(!prev);
		prev->next = scsi_current_sense_data.next;
	}
	scsi_current_sense_data.next = NULL;
}

static void scsi_senses_init(void)
{
	scsi_sdd_t sdd;
	scsi_recent_sense_data = NULL;
	for (sdd = 0; sdd < NR_SCSI_SENSES; sdd++) {
		scsi_sense_data_select(sdd);
		scsi_sense_data_init();
	}
}

#if NR_SCSI_TASKS > 1
scsi_tag_t scsi_tag = INVALID_SCSI_TAG;

void scsi_task_restore(scsi_tag_t tag)
{
	scsi_tag = tag;
}

scsi_tag_t scsi_task_save(scsi_tag_t tag)
{
	scsi_tag_t stag = scsi_tag;
	scsi_task_restore(tag);
	return stag;
}

boolean scsi_task_ready(void)
{
	scsi_tag_t tag;
	for (tag = 0; tag < NR_SCSI_TASKS; tag++) {
		scsi_task_select(tag);
		if (scsi_target_task.state == SCSI_TASK_ENDED)
			return true;
	}
	return false;
}

void scsi_tasks_init(void)
{
	scsi_tag_t tag;
	for (tag = 0; tag < NR_SCSI_TASKS; tag++) {
		scsi_task_select(tag);
		scsi_task_init();
	}
}
#else
#define scsi_tasks_init()	scsi_task_init()
#define scsi_task_ready()	(scsi_target_task.state == SCSI_TASK_ENDED)
#endif

#if NR_SCSI_CMNDS > 1
scsi_crn_t scsi_crn = INVALID_SCSI_CRN;

void scsi_cmnd_restore(scsi_crn_t crn)
{
	scsi_crn = crn;
}

scsi_crn_t scsi_cmnd_save(scsi_crn_t crn)
{
	scsi_crn_t scrn = scsi_crn;
	scsi_cmnd_restore(crn);
	return scrn;
}

boolean scsi_cmnd_ready(void)
{
	scsi_crn_t crn;
	for (crn = 0; crn < NR_SCSI_CMNDS; crn++) {
		scsi_cmnd_select(crn);
		if (scsi_current_cmnd.stae == SCSI_CMND_IDLE &&
		    scsi_current_cmnd.next == NULL) {
			return true;
		}
	}
	return false;
}

void scsi_cmnds_init(void)
{
	scsi_crn_t crn;
	for (crn = 0; crn < NR_SCSI_CMNDS; crn++) {
		scsi_cmnd_select(crn);
		scsi_cmnd_init();
	}
}

static void scsi_cmnds_sched(void)
{
	/* TODO: check tasks attributes and execute tasks commands in
	 * scsi_target_handler
	 */
}
#else
#define scsi_cmnds_init()	scsi_cmnd_init()
#define scsi_cmnd_ready()	(scsi_current_cmnd.state == SCSI_CMND_IDLE &&	\
				 scsi_current_cmnd.next == NULL)
#define scsi_cmnds_sched()
#endif

#if NR_SCSI_PORTS > 1
scsi_pid_t scsi_pid = INVALID_SCSI_PID;

void scsi_port_restore(scsi_pid_t pid)
{
	scsi_pid = pid;
	/* TODO: initiator/target selection */
	/* scsi_appl_select(scsi_target_port.iid); */
	/* scsi_tgrt_select(scsi_target_port.tid); */
}

scsi_pid_t scsi_port_save(scsi_pid_t pid)
{
	scsi_pid_t spid = scsi_pid;
	scsi_port_restore(pid);
	return spid;
}
#endif

scsi_lun_t scsi_max_units(void)
{
	return scsi_nr_luns;
}

void scsi_unit_restore(scsi_lun_t lun)
{
	scsi_lun = lun;
}

scsi_lun_t scsi_unit_save(scsi_lun_t lun)
{
	scsi_lun_t slun = scsi_lun;
	scsi_unit_restore(lun);
	return slun;
}

scsi_nexus_t scsi_nexus_current(void)
{
	return SCSI_NEXUS_C_ITLQ(0, scsi_target_dev(scsi_pid), scsi_lun, scsi_tag);
}

static void scsi_nexus_select(scsi_nexus_t nexus)
{
	scsi_unit_select(SCSI_NEXUS_L(nexus));
	scsi_task_select(SCSI_NEXUS_Q(nexus));

	BUG_ON(scsi_target_task.pid >= INVALID_SCSI_PID);
	scsi_port_select(scsi_target_task.pid);
}

/*=========================================================================
 * data IN/OUT buffer
 *=======================================================================*/
uint8_t scsi_def_readb(uint16_t offset)
{
	uint8_t byte = 0;
	BUG_ON(scsi_tag >= INVALID_SCSI_TAG);
	if (offset < scsi_current_cmnd.data_length) {
		byte = scsi_current_cmnd.data_buffer[offset];
	}
	return byte;
}

void scsi_def_writeb(uint8_t byte)
{
	BUG_ON(scsi_tag >= INVALID_SCSI_TAG);
	if (scsi_current_cmnd.data_length < scsi_current_cmnd.expect_length) {
		scsi_current_cmnd.data_buffer[scsi_current_cmnd.data_length] = byte;
		scsi_current_cmnd.data_length++;
	}
}

void scsi_def_writew(uint16_t word)
{
	scsi_def_writeb(HIBYTE(word));
	scsi_def_writeb(LOBYTE(word));
}

void scsi_def_writel(uint32_t dword)
{
	scsi_def_writew(HIWORD(dword));
	scsi_def_writew(LOWORD(dword));
}

void scsi_in_write_ascii(text_char_t *string, uint8_t size,
			 uint8_t pad)
{
	uint8_t i, n;

	n = min(text_strlen(string), size);
	for (i = 0; i < n; i++) {
		scsi_def_writeb(string[i]);
	}
	if (n < size) {
		for (i = 0; i < size-n; i++)
			scsi_def_writeb(pad);
	}
}

/*=========================================================================
 * NACA(CDB Control byte) & NormACA(standard INQUIRY data)
 *=======================================================================*/
#ifdef CONFIG_SCSI_NACA
static boolean scsi_aca_task_exist(void)
{
	struct scsi_task *last;
	last = scsi_head_of_queue;
	while (last) {
		if (last->attr == SCSI_TASK_ACA)
			return true;
		last = last->next;
	}
	return false;
}

boolean scsi_aca_valid(void)
{
	if (scsi_target_task_set.ACA) {
		if (SCSI_CONTROL_TMF_ONLY || scsi_aca_task_exist()) {
			return false;
		}
	}
	return true;
}

static void scsi_aca_cmpl(void)
{
	/* establishing ACA */
	if (scsi_get_status() == SCSI_STATUS_CHECK_CONDITION &&
	    SCSI_CONTROL_NACA(scsi_current_cmnd.control)) {
		scsi_invalid_cdb_field();
		scsi_target_task_set.ACA = 1;
	}
}

static boolean scsi_aca_route(void)
{
	scsi_target_task.state = SCSI_TASK_ENABLED;
	if (!scsi_target_task_set.ACA) {
		scsi_invalid_msg_error();
	}
	BUG_ON(scsi_target_task_set.ACA &&
	       (SCSI_CONTROL_TMF_ONLY || scsi_aca_task_exist()));
	return false;
}
#else
#define scsi_aca_valid()		(true)
#define scsi_aca_cmpl()

static boolean scsi_aca_route(void)
{
	scsi_invalid_msg_error();
	return false;
}
#endif

#ifdef CONFIG_SCSI_EVENT
static boolean scsi_ua_valid(void)
{
	if (scsi_target_task_set.UA) {
		return false;
	}
}
#else
#define scsi_ua_valid()		true
#endif

/*=========================================================================
 * LINK(CDB Control byte) & LINKED(standard INQUIRY data)
 *=======================================================================*/
#ifdef CONFIG_SCSI_LINK
static void scsi_link_valid(uint8_t attr, uint8_t ctrl)
{
	if (scsi_target_unit.linked_tag == INVALID_SCSI_TAG) {
		if (!scsi_task_ready()) {
			scsi_lack_task_resource();
			return;
		}
		scsi_target_task.linked = true;
		scsi_target_unit.linked_tag = scsi_target_task.tag;
	} else {
		scsi_task_select(scsi_target_unit.linked_tag);
	}
	scsi_invalid_cdb_field();
}

void scsi_link_completed(void)
{
	scsi_task_completed();
}
#else
static void scsi_link_valid(uint8_t attr, uint8_t ctrl)
{
	scsi_invalid_cdb_field();
}
#define scsi_link_completed()		scsi_task_completed()
#endif

struct scsi_task *scsi_task_by_id(scsi_tag_t tag)
{
	struct scsi_task *task;

	task = scsi_head_of_queue;
	while (task) {
		if (task->tag == tag)
			return task;
		task = task->next;
	}
	return NULL;
}

struct scsi_cmnd *scsi_cmnd_by_id(scsi_crn_t crn)
{
	struct scsi_cmnd *cmnd;

	cmnd = scsi_recent_cmnd;
	while (cmnd) {
		if (cmnd->crn == crn)
			return cmnd;
		cmnd = cmnd->next;
	}
	return NULL;
}

static struct scsi_task *scsi_task_last(void)
{
	struct scsi_task *last;
	last = scsi_head_of_queue;
	while (last)
		last = last->next;
	return last;
}

static struct scsi_cmnd *scsi_cmnd_last(void)
{
	struct scsi_cmnd *last;
	last = scsi_recent_cmnd;
	while (last)
		last = last->next;
	return last;
}

static void scsi_run_tasks_state(void)
{
	struct scsi_task *last;
	boolean blocking_simple = false, blocking_ordered = false;

	last = scsi_head_of_queue;
	while (last) {
		if (scsi_target_task.state == SCSI_TASK_ENDED)
			goto next;

		if (scsi_target_task_set.ACA) {
			if (scsi_target_task.attr != SCSI_TASK_ACA &&
			    scsi_target_task.state == SCSI_TASK_ENABLED) {
				scsi_target_task.state = SCSI_TASK_BLOCKED;
			}
		} else {
			BUG_ON(scsi_target_task.attr == SCSI_TASK_ACA);
			if (scsi_target_task.state == SCSI_TASK_BLOCKED ||
			    scsi_target_task.state == SCSI_TASK_ENABLED) {
				BUG_ON(scsi_target_task.attr == SCSI_TASK_SIMPLE &&
				       blocking_simple);
				BUG_ON(scsi_target_task.attr == SCSI_TASK_ORDERED &&
				       blocking_ordered);
				scsi_target_task.state = SCSI_TASK_ENABLED;
				blocking_simple = true;
				blocking_ordered = true;
			} else if (scsi_target_task.state == SCSI_TASK_DORMANT) {
				BUG_ON(scsi_target_task.attr == SCSI_TASK_HEAD_OF_QUEUE);
				if (scsi_target_task.attr == SCSI_TASK_ORDERED) {
					if (!blocking_ordered) {
						blocking_ordered = true;
						blocking_simple = true;
						scsi_target_task.state = SCSI_TASK_ENABLED;
					}
				} else {
					BUG_ON(scsi_target_task.attr != SCSI_TASK_SIMPLE);
					if (!blocking_simple) {
						blocking_simple = true;
						scsi_target_task.state = SCSI_TASK_ENABLED;
					}
				}
			}
		}
next:
		last = last->next;
	}
}

static void scsi_task_set_leave(void)
{
	scsi_target_task.state = SCSI_TASK_ENDED;

	if (&scsi_target_task == scsi_head_of_queue) {
		scsi_head_of_queue = scsi_target_task.next;
	} else {
		struct scsi_task *prev = scsi_head_of_queue;

		while (prev->next != &scsi_target_task)
			prev = prev->next;
		prev->next = scsi_target_task.next;
	}
	scsi_target_task.next = NULL;

	scsi_run_tasks_state();
}

static void scsi_task_set_enter(void)
{
	boolean head = false;

	switch (scsi_target_task.attr) {
	case SCSI_TASK_SIMPLE:
	case SCSI_TASK_ORDERED:
		scsi_target_task.state = SCSI_TASK_DORMANT;
		break;
	case SCSI_TASK_HEAD_OF_QUEUE:
		scsi_target_task.state = SCSI_TASK_ENABLED;
		head = true;
		break;
	case SCSI_TASK_ACA:
		head = scsi_aca_route();
		break;
	default:
		scsi_invalid_msg_error();
		break;
	}
	if (head) {
		scsi_target_task.next = scsi_head_of_queue;
		scsi_head_of_queue = &scsi_target_task;
	} else {
		struct scsi_task *last;
		last = scsi_task_last();
		if (last)
			last->next = &scsi_target_task;
		else
			scsi_head_of_queue = &scsi_target_task;
	}

	scsi_run_tasks_state();
}

/*=========================================================================
 * status/sense data
 *=======================================================================*/
void scsi_set_status(uint8_t status)
{
	BUG_ON(scsi_crn >= INVALID_SCSI_CRN);
	scsi_current_cmnd.status = status;
}

uint8_t scsi_get_status(void)
{
	BUG_ON(scsi_crn >= INVALID_SCSI_CRN);
	return scsi_current_cmnd.status;
}

uint8_t scsi_get_direction(void)
{
	return scsi_current_cmnd.dir;
}

void scsi_set_direction(uint8_t dir)
{
	scsi_current_cmnd.dir |= dir;
}

void scsi_clear_sense_flags(void)
{
	scsi_current_sense_data.sense_data.flags = 0;
}

void scsi_set_sense_code(uint8_t key, uint8_t asc, uint8_t ascq)
{
	BUG_ON(scsi_tag >= INVALID_SCSI_TAG);
	BUG_ON(scsi_crn >= INVALID_SCSI_CRN);
	scsi_current_sense_data.sense_data.key = key;
	scsi_current_sense_data.sense_data.asc = asc;
	scsi_current_sense_data.sense_data.ascq = ascq;
}

void scsi_set_sense_fruc(uint8_t fruc)
{
	scsi_current_sense_data.sense_data.fruc = fruc;
}

void scsi_set_sense_spec(uint32_t spec)
{
	scsi_current_sense_data.sense_data.spec = spec;
}

void scsi_set_sense_flag(uint8_t flag, boolean on)
{
	if (on)
		scsi_current_sense_data.sense_data.flags |= (1<<flag);
	else
		scsi_current_sense_data.sense_data.flags &= ~(1<<flag);
}

uint8_t scsi_get_sense_key(void)
{
	return scsi_current_sense_data.sense_data.key;
}

uint8_t scsi_get_sense_asc(void)
{
	return scsi_current_sense_data.sense_data.asc;
}

uint8_t scsi_get_sense_ascq(void)
{
	return scsi_current_sense_data.sense_data.ascq;
}

uint8_t scsi_get_sense_fruc(void)
{
	return scsi_current_sense_data.sense_data.fruc;
}

uint32_t scsi_get_sense_spec(void)
{
	return scsi_current_sense_data.sense_data.spec;
}

uint8_t scsi_get_sense_flag(uint8_t flag)
{
	if (scsi_current_sense_data.sense_data.flags & (1<<flag)) {
		switch (flag) {
		case SCSI_SENSE_FLAG_VALID:
			return SCSI_SENSE_FIXED_VALID;
		case SCSI_SENSE_FLAG_SKSV:
			return SCSI_SENSE_FIXED_SKSV;
		default:
			return flag;
		}
	} else {
		return 0;
	}
}

void scsi_illegal_request(uint8_t asc)
{
	scsi_sense_data_select(scsi_current_cmnd.sdd);
	scsi_set_sense_code(SCSI_SENSE_ILLEGAL_REQUEST, asc, 0x00);
	scsi_command_done(SCSI_STATUS_CHECK_CONDITION);
}

void scsi_invalid_cmd_opcode(void)
{
	scsi_illegal_request(SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
}

void scsi_invalid_cdb_field(void)
{
	scsi_illegal_request(SCSI_ASC_INVALID_FIELD_IN_CDB);
}

void scsi_lack_task_resource(void)
{
	if (scsi_head_of_queue == NULL)
		scsi_set_status(SCSI_STATUS_BUSY);
	else
		scsi_set_status(SCSI_STATUS_TASK_SET_FULL);
}

void scsi_invalid_msg_error(void)
{
	scsi_illegal_request(SCSI_ASC_INVALID_MESSAGE_ERROR);
}

void scsi_command_bulkio(scsi_crn_t crn)
{
	scsi_cmnd_select(msd_scsi_crn);
	BUG_ON(!scsi_current_cmnd.bulk_io);
	scsi_current_cmnd.bulk_io();
}

/*=========================================================================
 * TEST UNIT READY command
 *=======================================================================*/
static void scsi_test_unit_ready_parse(uint8_t *cdb, uint8_t len)
{
	scsi_command_aval(SCSI_CMND_IN);
}

static void scsi_test_unit_ready_aval(void)
{
	scsi_command_cmpl();
}

void scsi_sense_not_ready(void)
{
	scsi_sense_data_select(scsi_current_cmnd.sdd);
	scsi_set_sense_code(SCSI_SENSE_NOT_READY,
			    SCSI_ASC_NOT_READY_TO_READY_CHANGE, 0x00);
	scsi_command_done(SCSI_STATUS_CHECK_CONDITION);
}

static void scsi_test_unit_ready_cmpl(void)
{
	BUG_ON(!scsi_target_device->test_ready);
	if (scsi_target_device->test_ready()) {
		scsi_command_done(SCSI_STATUS_GOOD);
	} else {
		scsi_sense_not_ready();
	}
}

#ifdef CONFIG_SCSI_RMB
/*=========================================================================
 * PREVENT ALLOW MEDIUM REMOVAL command
 *=======================================================================*/
static void scsi_prevent_allow_medium_removal_parse(uint8_t *cdb, uint8_t len)
{
	scsi_prevent_allow_medium_removal_cmnd.prevent =
		SCSI_CDB_BITS(cdb, SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL_PREVENT);;
	scsi_command_aval(SCSI_CMND_IN);
}

static void scsi_prevent_allow_medium_removal_aval(void)
{
	scsi_command_cmpl();
}

static void scsi_prevent_allow_medium_removal_cmpl(void)
{
	/* TODO: perform prevention. */
	scsi_command_done(SCSI_STATUS_GOOD);
}
#else
#define scsi_prevent_allow_medium_removal_parse(cdb, len)	\
	scsi_invalid_cmd_opcode()
#define scsi_prevent_allow_medium_removal_aval()
#define scsi_prevent_allow_medium_removal_cmpl()
#endif

/*=========================================================================
 * REQUEST SENSE command
 *=======================================================================*/
#define scsi_sense_fixed_vendor_size()	0
#define scsi_sense_fixed_vendor_data()	0

static uint8_t scsi_sense_fixed_length(void)
{
	return SCSI_SENSE_FIXED_MIN_LENGTH + scsi_sense_fixed_vendor_size();
}

static void scsi_request_sense_parse(uint8_t *cdb, uint8_t len)
{
	/* ALLOCATION LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_byte(cdb, 4);
	scsi_request_sense_cmnd.desc = SCSI_CDB_BITS(cdb, SCSI_REQUEST_SENSE_DESC);
	scsi_command_aval(SCSI_CMND_IN);
}

#ifdef CONFIG_SCSI_SENSE_DESC
static boolean scsi_sense_desc_aval(void)
{
	return true;
}

static void scsi_sense_desc_cmpl(void)
{
}
#else
static boolean scsi_sense_desc_aval(void)
{
	scsi_invalid_cdb_field();
	return false;
}

static void scsi_sense_desc_cmpl(void)
{
}
#endif

static void scsi_request_sense_aval(void)
{
	if (scsi_request_sense_cmnd.desc) {
		if (!scsi_sense_desc_aval())
			return;
	} else {
		scsi_request_sense_cmnd.length = scsi_sense_fixed_length();
	}
	scsi_command_cmpl();
}

static uint8_t scsi_sense_resp_code(void)
{
	return scsi_recent_sense_data ?
	       SCSI_RESPONSE_CODE_CURRENT : SCSI_RESPONSE_CODE_DEFERRED;
}

static void scsi_request_sense_cmpl(void)
{
	uint8_t i;

	if (scsi_request_sense_cmnd.desc) {
		scsi_sense_desc_cmpl();
	} else {
		scsi_sdd_t sdd;

		if (scsi_recent_sense_data) {
			sdd = scsi_sense_data_save(scsi_recent_sense_data->sdd);
			scsi_def_writeb((uint8_t)(scsi_get_sense_flag(SCSI_SENSE_FLAG_VALID) |
						  scsi_sense_resp_code()));
			scsi_def_writeb(0x00);
			scsi_def_writeb((uint8_t)(scsi_get_sense_flag(SCSI_SENSE_FLAG_FILEMARK) |
						  scsi_get_sense_flag(SCSI_SENSE_FLAG_EOM) |
						  scsi_get_sense_flag(SCSI_SENSE_FLAG_ILI) |
						  scsi_get_sense_key()));
			scsi_def_writel(0x00000000);
			scsi_def_writeb((uint8_t)(scsi_sense_fixed_length()-7));
			scsi_def_writel(0x00000000);
			scsi_def_writeb(scsi_get_sense_asc());
			scsi_def_writeb(scsi_get_sense_ascq());
			scsi_def_writeb(scsi_recent_sense_data->sense_data.fruc);
			/* sense key specific */
			scsi_def_writeb((uint8_t)(scsi_get_sense_flag(SCSI_SENSE_FLAG_SKSV) |
						  LOBYTE(HIWORD(scsi_get_sense_spec()))));
			scsi_def_writew(LOWORD(scsi_get_sense_spec()));
			scsi_sense_data_leave();
			scsi_sense_data_exit();
			scsi_sense_data_restore(sdd);
		} else {
			scsi_def_writeb(SCSI_RESPONSE_CODE_CURRENT);
			scsi_def_writeb(SCSI_SENSE_NO_SENSE);
			scsi_def_writeb(0x00);
			scsi_def_writel(0x00000000);
			scsi_def_writeb((uint8_t)(scsi_sense_fixed_length()-7));
			scsi_def_writel(0x00000000);
			scsi_def_writeb(SCSI_ASC_NO_ADDITIONAL_SENSE_INFO);
			scsi_def_writeb(0x00);
			/* field replaceable unit code */
			scsi_def_writeb(0x00);
			/* sense key specific */
			scsi_def_writeb(0x00);
			scsi_def_writeb(0x00);
			scsi_def_writeb(0x00);
		}

		/* Vendor Specific */
		for (i = SCSI_SENSE_FIXED_MIN_LENGTH;
		     i < scsi_sense_fixed_length(); i++) {
			scsi_def_writew(scsi_sense_fixed_vendor_data());
		}
	}

	scsi_command_done(SCSI_STATUS_GOOD);
}

/*=========================================================================
 * REPORT LUNS command
 *=======================================================================*/
static void scsi_report_luns_parse(uint8_t *cdb, uint8_t len)
{
	/* ALLOCATION LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_long(cdb, 6);
	scsi_report_luns_cmnd.select_report =
		SCSI_CDB_BYTE(cdb, SCSI_REPORT_LUNS_SELECT_REPORT);
	scsi_implicit_head_of_queue();
	scsi_command_aval(SCSI_CMND_IN);
}

static void scsi_report_luns_aval(void)
{
	scsi_invalid_cmd_opcode();
}

static void scsi_report_luns_cmpl(void)
{
}

#ifdef CONFIG_SCSI_MODE
void scsi_saving_not_supported(void)
{
	scsi_illegal_request(SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED);
}

/*=========================================================================
 * MODE SELECT(6) command
 *=======================================================================*/
static void scsi_mode_select6_parse(uint8_t *cdb, uint8_t len)
{
	scsi_mode_select_cmnd.pf = SCSI_CDB_BITS(cdb, SCSI_MODE_SELECT_PF);
	scsi_mode_select_cmnd.sp = SCSI_CDB_BITS(cdb, SCSI_MODE_SELECT_SP);
	/* PARAMETER LIST LENGTH */
	scsi_current_cmnd.expect_length =
		SCSI_CDB_BYTE(cdb, SCSI_MODE_SELECT_PARAMETER_LIST_LENGTH);
	scsi_command_aval(SCSI_CMND_OUT);
}

static void scsi_mode_select6_aval(void)
{
	scsi_invalid_cmd_opcode();
}

static void scsi_mode_select6_cmpl(void)
{
}

/*=========================================================================
 * MODE SENSE(6) command
 *=======================================================================*/
static void scsi_mode_sense6_parse(uint8_t *cdb, uint8_t len)
{
	/* ALLOCATION LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_byte(cdb, 4);
	scsi_mode_sense_cmnd.dbd = SCSI_CDB_BITS(cdb, SCSI_MODE_SENSE_DBD);
	scsi_mode_sense_cmnd.pc = SCSI_CDB_BITS(cdb, SCSI_MODE_SENSE_PC);
	scsi_mode_sense_cmnd.page_code = SCSI_CDB_BITS(cdb, SCSI_MODE_SENSE_PAGE_CODE);
	scsi_mode_sense_cmnd.subpage_code =
		SCSI_CDB_BYTE(cdb, SCSI_MODE_SENSE_SUBPAGE_CODE);
	scsi_command_aval(SCSI_CMND_IN);
}

static void scsi_mode_sense6_aval(void)
{
	scsi_invalid_cmd_opcode();
}

static void scsi_mode_sense6_cmpl(void)
{
}

void scsi_allocate_mode_page(scsi_lun_t lun, uint8_t length,
			     uint8_t page_code, uint8_t subpage_code,
			     uint8_t policy)
{
	uint8_t index;

	/* TODO: only SHARED mode page policy is supported */
	BUG_ON(policy != SCSI_MODE_PAGE_POLICY_SHARED);
	scsi_unit_select(lun);
	BUG_ON(scsi_target_unit.nr_mode_pages >= NR_SCSI_MODES);
	BUG_ON((scsi_target_unit.free_pages_pos+length) >= SCSI_MAX_PAGES);
	index = scsi_target_unit.nr_mode_pages;
	scsi_target_unit.mode_pages[index].page_code = page_code;
	scsi_target_unit.mode_pages[index].subpage_code = subpage_code;
	scsi_target_unit.mode_pages[index].offset = scsi_target_unit.free_pages_pos;
	scsi_target_unit.mode_pages[index].length = length;
	scsi_target_unit.mode_pages[index].policy = policy;
	scsi_target_unit.nr_mode_pages++;
	scsi_target_unit.free_pages_pos += length;
}

static void scsi_mode_init(void)
{
	uint8_t i;

	for (i = 0; i < NR_SCSI_MODES; i++) {
		scsi_target_unit.mode_pages[i].offset = 0;
		scsi_target_unit.mode_pages[i].length = 0;
	}
	scsi_target_unit.nr_mode_pages = 0;
	scsi_target_unit.free_pages_pos = 0;
}

static uint16_t scsi_mode_vpd_size(uint8_t page_code)
{
	return 0;
}

static void scsi_mode_vpd_data(uint8_t page_code)
{
}

scsi_vpd_page_t scsi_mode_vpd = {
	scsi_mode_vpd_size,
	scsi_mode_vpd_data,
};

static void scsi_mode_vpd_init(void)
{
	scsi_register_vpd_page(SCSI_VPD_MODE_PAGE_POLICY, &scsi_mode_vpd);
}
#else
#define scsi_mode_select6_parse(cdb, len)	scsi_invalid_cmd_opcode()
#define scsi_mode_select6_aval()
#define scsi_mode_select6_cmpl()

#define scsi_mode_sense6_parse(cdb, len)	scsi_invalid_cmd_opcode()
#define scsi_mode_sense6_aval()
#define scsi_mode_sense6_cmpl()

#define scsi_mode_init()
#define scsi_mode_vpd_init()
#endif

/*=========================================================================
 * INQUIRY command
 *=======================================================================*/
#define scsi_inquiry_vendor_size(evpd, page_code)	0
#define scsi_inquiry_vendor_data(evpd, page_code)	SCSI_INQUIRY_RESERVED
#define scsi_inquiry_write_ascii(string, size)		\
	scsi_in_write_ascii(string, size, SCSI_INQUIRY_SPACE)

/* ASCII information VPD page */
#ifdef CONFIG_SCSI_VPD_ASCII
uint8_t scsi_nr_asciis = 0;
text_char_t * data scsi_ascii_infos[NR_SCSI_ASCIIS];

#define scsi_ascii_info_vendor_size(index)		\
	scsi_inquiry_vendor_size(true, index + SCSI_VPD_ASCII_INFO_BEGIN)
#define scsi_ascii_info_vendor_data(index)		\
	scsi_inquiry_vendor_data(true, index + SCSI_VPD_ASCII_INFO_BEGIN)

static uint16_t scsi_ascii_vpd_size(uint8_t page_code)
{
	uint8_t index = page_code - SCSI_VPD_ASCII_INFO_BEGIN;
	if (index >= scsi_nr_asciis)
		return 0;
	return text_strlen(scsi_ascii_infos[index]+2) +
	       scsi_ascii_info_vendor_size(index);
}

static void scsi_ascii_vpd_data(uint8_t page_code)
{
	uint8_t i;
	text_char_t *ascii_info;
	uint8_t index = page_code - SCSI_VPD_ASCII_INFO_BEGIN;
	uint8_t length = scsi_ascii_vpd_size(page_code);

	scsi_def_writeb(length);
	ascii_info = scsi_ascii_infos[index];
	for (i = 1; i < length; i++) {
		scsi_def_writeb(ascii_info[i]);
	/* Vendor specific information here after */
	scsi_ascii_info_vendor_data(index);
}

scsi_vpd_page_t scsi_ascii_vpd = {
	scsi_ascii_vpd_size,
	scsi_ascii_vpd_data,
};

static uint8_t scsi_register_ascii_info(text_char_t *ascii_info)
{
	uint8_t index;
	BUG_ON(scsi_nr_asciis >= NR_SCSI_ASCIIS);
	index = scsi_nr_asciis;
	scsi_ascii_infos[index] = ascii_info;
	scsi_register_vpd_page(index + SCSI_VPD_ASCII_INFO_BEGIN, &scsi_ascii_vpd);
	scsi_nr_asciis++;
	return index;
}
#endif

#ifdef CONFIG_SCSI_VPD_SN
#define SCSI_SERIAL_LENGTH		6

static uint16_t scsi_serial_vpd_size(uint8_t page_code)
{
	return SCSI_SERIAL_LENGTH;
}

static void scsi_serial_vpd_data(uint8_t page_code)
{
	scsi_def_writew(DEV_VENDOR_ID);
	scsi_def_writew(system_product_id());
	scsi_def_writew(DEV_SERIAL_NO);
}

scsi_vpd_page_t scsi_serial_vpd = {
	scsi_serial_vpd_size,
	scsi_serial_vpd_data,
};

static void scsi_serial_vpd_init(void)
{
	scsi_register_vpd_page(SCSI_VPD_UNIT_SERIAL_NUMBER, &scsi_serial_vpd);
}
#else
#define scsi_serial_vpd_init()
#endif

#ifdef CONFIG_SCSI_INQUIRY_EX
static uint16_t scsi_extend_vpd_size(uint8_t page_code)
{
	return SCSI_INQUIRY_EXT_LENGTH;
}

static void scsi_extend_vpd_data(uint8_t page_code)
{
	uint8_t i;

	scsi_def_writeb(scsi_device_specific_inquiry(page_code));
	scsi_def_writeb((uint8_t)(scsi_device_specific_inquiry(page_code) |
				  SCSI_TARGET_PRIOR_SUP |
				  SCSI_INQUIRY_EXT_HEADSUP |
				  SCSI_INQUIRY_EXT_ORDSUP |
				  SCSI_INQUIRY_EXT_SIMPSUP));
	scsi_def_writeb(scsi_device_specific_inquiry(page_code));
	for (i = 3; i < SCSI_INQUIRY_EXT_LENGTH; i++)
		scsi_def_writeb(SCSI_INQUIRY_RESERVED);
}

scsi_vpd_page_t scsi_extend_vpd = {
	scsi_extend_vpd_size,
	scsi_extend_vpd_data,
};

static void scsi_extend_vpd_init(void)
{
	scsi_register_vpd_page(SCSI_VPD_EXTENDED_INQUIRY_DATA, &scsi_extend_vpd);
}
#else
#define scsi_extend_vpd_init()
#endif

/* supported descriptors VPD page */
static uint16_t scsi_supp_vpd_size(uint8_t page_code)
{
	return scsi_nr_vpds;
}

static void scsi_supp_vpd_data(uint8_t page_code)
{
	uint8_t last_code, min_code, code;
	uint8_t i, j;

	last_code = min_code = SCSI_VPD_SUPPORTED_VPD_PAGES;
	scsi_def_writeb(last_code);
	for (i = 1; i < scsi_nr_vpds; i++) {
		for (j = 0; j < scsi_nr_vpds; j++) {
			code = scsi_vpd_codes[j];
			if ((code > last_code) &&
			    (min_code == last_code || code < min_code)) {
				min_code = code;
			}
		}
		BUG_ON(min_code <= last_code);
		scsi_def_writeb(min_code);
		last_code = min_code;
	}
}

scsi_vpd_page_t scsi_supp_vpd = {
	scsi_supp_vpd_size,
	scsi_supp_vpd_data,
};

/* device identification descriptors VPD page */
#define SCSI_IDENT_HEAD_SIZE		4

static void scsi_ident_head_data(boolean piv, uint8_t code_set,
				 uint8_t assoc, uint8_t type,
				 uint16_t length)
{
	scsi_transport_t *xprt = scsi_target_xprt;

	scsi_def_writeb((uint8_t)((piv ? xprt->proto : 0x00) | code_set));
	scsi_def_writeb((uint8_t)((piv ? 0x80 : 0x00) | assoc | type));
	scsi_def_writew((uint16_t)(length - SCSI_IDENT_HEAD_SIZE));
}

/* NAA format descriptor */
#define SCSI_IDENT_NAA_MIN		4
/* TODO: real IEEE registered company identification */
#define SCSI_IEEE_COMP_ID		(uint32_t)DEV_VENDOR_ID

static uint8_t scsi_ident_naa_size(void)
{
	return SCSI_IDENT_HEAD_SIZE +
	       SCSI_IDENT_NAA_MIN + sizeof (uint8_t);
}

static void scsi_ident_naa_data(boolean piv, uint8_t assoc, uint8_t id)
{
	scsi_ident_head_data(piv,
			     SCSI_CODE_SET_BINARY,
			     assoc,
			     SCSI_IDENT_TYPE_NAA,
			     scsi_ident_naa_size());
	scsi_def_writeb(MAKEBYTE(HIHALF(HIBYTE(HIWORD(SCSI_IEEE_COMP_ID))),
				 SCSI_NAA_IEEE_REG));
	scsi_def_writeb(MAKEBYTE(HIHALF(LOBYTE(HIWORD(SCSI_IEEE_COMP_ID))),
				 LOHALF(HIBYTE(HIWORD(SCSI_IEEE_COMP_ID)))));
	scsi_def_writeb(MAKEBYTE(HIHALF(HIBYTE(LOWORD(SCSI_IEEE_COMP_ID))),
				 LOHALF(LOBYTE(HIWORD(SCSI_IEEE_COMP_ID)))));
	scsi_def_writeb(MAKEBYTE(HIHALF(LOBYTE(LOWORD(SCSI_IEEE_COMP_ID))),
				 LOHALF(HIBYTE(LOWORD(SCSI_IEEE_COMP_ID)))));
	scsi_def_writeb(MAKEBYTE(HIHALF(id),
				 LOHALF(LOBYTE(LOWORD(SCSI_IEEE_COMP_ID)))));
	scsi_def_writeb(MAKEBYTE(0, LOHALF(scsi_lun)));
}

#ifdef CONFIG_SCSI_SCSI_RPID
static uint16_t scsi_ident_rpid_size(void)
{
	return SCSI_IDENT_HEAD_SIZE + 4;
}

static void scsi_ident_rpid_data(void)
{
	scsi_ident_head_data(true,
			     SCSI_CODE_SET_BINARY,
			     SCSI_ASSOC_TARGET_PORT,
			     SCSI_IDENT_TYPE_REL_PID,
			     scsi_ident_rpid_size());
	scsi_def_writew(0);
	scsi_def_writew(scsi_target_port.rpid);
}
#else
#define scsi_ident_rpid_size()		0
#define scsi_ident_rpid_data()
#endif

static uint8_t scsi_ident_vid_size(void)
{
	return SCSI_IDENT_HEAD_SIZE + 8 + 16 + 10;
}

static void scsi_ident_vid_data(void)
{
	scsi_ident_head_data(false,
			     SCSI_CODE_SET_ASCII,
			     SCSI_ASSOC_LOGICAL_UNIT,
			     SCSI_IDENT_TYPE_T10_VID,
			     scsi_ident_vid_size());
	scsi_inquiry_write_ascii(system_vendor_name, 8);
	scsi_inquiry_write_ascii(system_device_name, 16);
	/* vendor identifier */
	scsi_def_writew(DEV_VENDOR_ID);
	/* product identifier */
	scsi_def_writew(system_product_id());
	/* device identifier */
	scsi_def_writel(system_device_id);
	/* product serial no */
	scsi_def_writew(DEV_SERIAL_NO);
}

#define scsi_ident_lun_size()	scsi_ident_naa_size()
#define scsi_ident_lun_data()	scsi_ident_naa_data(false, SCSI_ASSOC_LOGICAL_UNIT, scsi_lun)

#define scsi_ident_pid_size()	scsi_ident_naa_size()
#define scsi_ident_pid_data()	scsi_ident_naa_data(true, SCSI_ASSOC_TARGET_PORT, scsi_pid)

#define scsi_ident_dev_size()	scsi_ident_naa_size()
#define scsi_ident_dev_data()	scsi_ident_naa_data(true, SCSI_ASSOC_TARGET_DEVICE, 0)

static uint16_t scsi_ident_vpd_size(uint8_t page_code)
{
	uint16_t length = 0;
	
	length += scsi_ident_vid_size();
	/* logical unit name requirement */
	length += scsi_ident_lun_size();
	/* relative port identifier */
	length += scsi_ident_rpid_size();
	/* target port identifier */
	length += scsi_ident_pid_size();
	/* target device identifier */
	length += scsi_ident_dev_size();
	return length;
}

static void scsi_ident_vpd_data(uint8_t page_code)
{
	scsi_ident_vid_data();
	/* logical unit name requirement */
	scsi_ident_lun_data();
	/* relative port identifier */
	scsi_ident_rpid_data();
	/* target port identifier */
	scsi_ident_pid_data();
	/* target device identifier */
	scsi_ident_dev_data();
}

scsi_vpd_page_t scsi_ident_vpd = {
	scsi_ident_vpd_size,
	scsi_ident_vpd_data,
};

static uint8_t scsi_inquiry_std_length(void)
{
	return SCSI_INQUIRY_MIN_LENGTH + scsi_inquiry_vendor_size(false, 0);
}

static void scsi_inquiry_cmpl(void)
{
	uint8_t i;

	BUG_ON(scsi_get_status() != SCSI_STATUS_GOOD);

	scsi_def_writeb(MAKEBYTE(scsi_target_device->type,
				 SCSI_PERIPH_QUAL_SPEC_CONN));
	if (scsi_inquiry_cmnd.evpd) {
		scsi_vpd_page_t *vpd;
		
		vpd = scsi_vpd_page_by_code(scsi_inquiry_cmnd.page_code);
		BUG_ON(!vpd || !vpd->page_data);
		scsi_def_writeb(scsi_inquiry_cmnd.page_code);
		scsi_def_writew((uint8_t)(scsi_inquiry_cmnd.length - 4));
		vpd->page_data(scsi_inquiry_cmnd.page_code);
	} else {
		scsi_def_writeb((uint8_t)(scsi_target_device->flags & SCSI_DEVICE_RMB ?
					  SCSI_TARGET_RMB : 0x00));
		scsi_def_writeb(SCSI_VERSION);
		scsi_def_writeb((uint8_t)(SCSI_TARGET_NORMACA |
					  SCSI_TARGET_HISUP |
					  SCSI_INQUIRY_RESP_DATA_FORMAT));
		scsi_def_writeb((uint8_t)(scsi_inquiry_cmnd.length - 4));
		scsi_def_writeb((uint8_t)(scsi_device_specific_inquiry(0x00) |
					  SCSI_TARGET_ACC |
					  SCSI_TARGET_TPGS |
					  SCSI_TARGET_3PC));
		scsi_def_writeb((uint8_t)(scsi_device_specific_inquiry(0x00) |
					  scsi_xprt_specific_inquiry(0x00) |
					  SCSI_TARGET_BQUE |
					  scsi_inquiry_vendor_data(false, 0) |
					  SCSI_TARGET_MULTIP));
		scsi_def_writeb((uint8_t)(scsi_xprt_specific_inquiry(0x00) |
					  SCSI_TARGET_LINKED |
					  SCSI_TARGET_CMDQUE |
					  scsi_inquiry_vendor_data(false, 0)));
		scsi_inquiry_write_ascii(system_vendor_name, 8);
		scsi_inquiry_write_ascii(system_device_name, 16);
		scsi_def_writel(SCSI_TARGET_PRODUCT_REV);
		/* Vendor Specific */
		for (i = 36; i < 56; i++) {
			scsi_def_writeb(scsi_inquiry_vendor_data(false, 0));
		}
		scsi_def_writeb(scsi_xprt_specific_inquiry(0x00));
		/* Reserved */
		scsi_def_writeb(SCSI_INQUIRY_RESERVED);
		/* VERSION DESCRIPTOR */
		scsi_def_writew(SCSI_TARGET_SAM);
		scsi_def_writew(scsi_target_xprt->phys_ver_desc);
		scsi_def_writew(scsi_target_xprt->xprt_ver_desc);
		scsi_def_writew(SCSI_TARGET_SPC);
		scsi_def_writew(scsi_target_device->ver_desc);
		/* TODO: unused version descriptor
		 * SES, SCC, transport protocol can be filled here
		 */
		for (i = 5; i < 8; i++) {
			scsi_def_writew(SCSI_INQUIRY_RESERVED);
		}
		/* Reserved */
		for (i = 74; i < SCSI_INQUIRY_MIN_LENGTH; i++)
			scsi_def_writeb(SCSI_INQUIRY_RESERVED);
		/* Vendor Specific */
		for (i = SCSI_INQUIRY_MIN_LENGTH;
		     i < scsi_inquiry_std_length(); i++) {
			scsi_def_writeb(scsi_inquiry_vendor_data(false, 0));
		}
	}

	scsi_command_done(SCSI_STATUS_GOOD);
}

static void scsi_inquiry_parse(uint8_t *cdb, uint8_t len)
{
	/* ALLOCATION LENGTH */
	scsi_current_cmnd.expect_length = scsi_cdb_word(cdb, 3);
	scsi_inquiry_cmnd.evpd = SCSI_CDB_BITS(cdb, SCSI_INQUIRY_EVPD);
	scsi_inquiry_cmnd.page_code =
		SCSI_CDB_BYTE(cdb, SCSI_INQUIRY_PAGE_CODE);
	scsi_implicit_head_of_queue();
	scsi_command_aval(SCSI_CMND_IN);
}

static void scsi_inquiry_aval(void)
{
	uint8_t page_code = scsi_inquiry_cmnd.page_code;

	if (scsi_inquiry_cmnd.evpd) {
		scsi_vpd_page_t *vpd = scsi_vpd_page_by_code(page_code);
		if (!vpd) {
			scsi_invalid_cdb_field();
			return;
		}
		BUG_ON(!vpd->page_size);
		scsi_inquiry_cmnd.length = 4 + vpd->page_size(page_code);
	} else {
		if (page_code != 0) {
			scsi_invalid_cdb_field();
			return;
		}
		scsi_inquiry_cmnd.length = scsi_inquiry_std_length();
	}
	scsi_command_cmpl();
}

scsi_vpd_page_t *scsi_vpd_page_by_code(uint8_t code)
{
	uint8_t i;
	for (i = 0; i < scsi_nr_vpds; i++) {
		if (scsi_vpd_codes[i] == code)
			return scsi_vpd_pages[i];
	}
	return NULL;
}

void scsi_register_vpd_page(uint8_t page_code, scsi_vpd_page_t *vpd)
{
	uint8_t index;
	BUG_ON(scsi_nr_vpds >= NR_SCSI_VPDS);
	index = scsi_nr_vpds;
	scsi_vpd_pages[index] = vpd;
	scsi_vpd_codes[index] = page_code;
	scsi_nr_vpds++;
}

static void scsi_inquiry_init(void)
{
	scsi_register_vpd_page(SCSI_VPD_SUPPORTED_VPD_PAGES, &scsi_supp_vpd);
	scsi_register_vpd_page(SCSI_VPD_DEVICE_IDENTIFICATION, &scsi_ident_vpd);
	scsi_serial_vpd_init();
	scsi_extend_vpd_init();
	scsi_mode_vpd_init();
}

/*=========================================================================
 * command handler
 *=======================================================================*/
static void scsi_cmnd_parse(uint8_t *cdb, uint8_t len)
{
	switch (cdb[0]) {
	case SCSI_CMND_REQUEST_SENSE:
		scsi_request_sense_parse(cdb, len);
		break;
	case SCSI_CMND_INQUIRY:
		scsi_inquiry_parse(cdb, len);
		break;
	case SCSI_CMND_TEST_UNIT_READY:
		scsi_test_unit_ready_parse(cdb, len);
		break;
	case SCSI_CMND_PREVENT_ALLOW_MEDIUM_REMOVAL:
		scsi_prevent_allow_medium_removal_parse(cdb, len);
		break;
	case SCSI_CMND_REPORT_LUNS:
		scsi_report_luns_parse(cdb, len);
		break;
	case SCSI_CMND_MODE_SELECT_6:
		scsi_mode_select6_parse(cdb, len);
		break;
	case SCSI_CMND_MODE_SENSE_6:
		scsi_mode_sense6_parse(cdb, len);
		break;
	default:
		scsi_device_specific_parse(cdb, len);
		break;
	}
}

static void scsi_cmnd_set_leave(void)
{
	if (&scsi_current_cmnd == scsi_recent_cmnd) {
		scsi_recent_cmnd = scsi_current_cmnd.next;
	} else {
		struct scsi_cmnd *prev = scsi_recent_cmnd;
		while (prev->next != &scsi_current_cmnd)
			prev = prev->next;
		prev->next = scsi_current_cmnd.next;
	}
	scsi_current_cmnd.next = NULL;
}

static void scsi_cmnd_set_enter(void)
{
	struct scsi_cmnd *last;
	last = scsi_cmnd_last();
	if (last)
		last->next = &scsi_current_cmnd;
	else
		scsi_recent_cmnd = &scsi_current_cmnd;
}

uint8_t scsi_xprt_specific_inquiry(uint8_t page_code)
{
	return scsi_target_xprt->inquiry(page_code,
					 (uint8_t)scsi_current_cmnd.data_length);
}

uint8_t scsi_device_specific_inquiry(uint8_t page_code)
{
	return scsi_target_device->inquiry(page_code,
					   (uint8_t)scsi_current_cmnd.data_length);
}

void scsi_device_specific_parse(uint8_t *cdb, uint8_t len)
{
	scsi_target_device->parse_cdb(cdb, len);
}

void scsi_device_specific_aval(void)
{
	scsi_target_device->cmnd_aval();
}

void scsi_device_specific_cmpl(void)
{
	scsi_target_device->cmnd_cmpl();
}

static void scsi_cmnd_cmpl(void)
{
	switch (scsi_current_cmnd.opcode) {
	case SCSI_CMND_REQUEST_SENSE:
		scsi_request_sense_cmpl();
		break;
	case SCSI_CMND_INQUIRY:
		scsi_inquiry_cmpl();
		break;
	case SCSI_CMND_TEST_UNIT_READY:
		scsi_test_unit_ready_cmpl();
		break;
	case SCSI_CMND_PREVENT_ALLOW_MEDIUM_REMOVAL:
		scsi_prevent_allow_medium_removal_cmpl();
		break;
	case SCSI_CMND_REPORT_LUNS:
		scsi_report_luns_cmpl();
		break;
	case SCSI_CMND_MODE_SELECT_6:
		scsi_mode_select6_cmpl();
		break;
	case SCSI_CMND_MODE_SENSE_6:
		scsi_mode_sense6_cmpl();
		break;
	default:
		scsi_device_specific_cmpl();
		break;
	}
}

void scsi_task_completed(void)
{
	if (!scsi_recent_cmnd) {
		scsi_task_set_leave();
	}
}

uint8_t scsi_command_status(scsi_crn_t crn)
{
	scsi_cmnd_select(crn);
	return scsi_get_status();
}

scsi_lbs_t scsi_command_expect(scsi_crn_t crn)
{
	scsi_cmnd_select(crn);
	return (scsi_lbs_t)scsi_current_cmnd.expect_length <<
	       scsi_current_cmnd.expect_granularity;
}

void scsi_command_complete(scsi_crn_t crn)
{
	scsi_cmnd_select(crn);

	/* unlink sense data descriptor */
	scsi_sense_data_select(scsi_current_cmnd.sdd);
	if (scsi_get_sense_key() != SCSI_SENSE_NO_SENSE) {
		scsi_sense_data_enter();
	} else {
		scsi_sense_data_exit();
	}

	if (scsi_current_cmnd.nexus != INVALID_SCSI_NEXUS) {
		scsi_nexus_select(scsi_current_cmnd.nexus);

		scsi_aca_cmpl();
		scsi_cmnd_set_leave();

		if (scsi_target_task.linked) {
			scsi_link_completed();
		} else {
			scsi_task_completed();
		}
	}
	__scsi_cmnd_init();
}

static void scsi_cmnd_aval(void)
{
	switch (scsi_current_cmnd.opcode) {
	case SCSI_CMND_REQUEST_SENSE:
		scsi_request_sense_aval();
		break;
	case SCSI_CMND_INQUIRY:
		scsi_inquiry_aval();
		break;
	case SCSI_CMND_TEST_UNIT_READY:
		scsi_test_unit_ready_aval();
		break;
	case SCSI_CMND_PREVENT_ALLOW_MEDIUM_REMOVAL:
		scsi_prevent_allow_medium_removal_aval();
		break;
	case SCSI_CMND_REPORT_LUNS:
		scsi_report_luns_aval();
		break;
	case SCSI_CMND_MODE_SELECT_6:
		scsi_mode_select6_aval();
		break;
	case SCSI_CMND_MODE_SENSE_6:
		scsi_mode_sense6_aval();
		break;
	default:
		scsi_device_specific_aval();
		break;
	}
}

boolean scsi_command_schedule(scsi_crn_t crn, boolean sync)
{
	scsi_cmnd_select(crn);
	BUG_ON(scsi_current_cmnd.state == SCSI_CMND_DONE);
	BUG_ON(scsi_current_cmnd.state == SCSI_CMND_BUSY);

	BUG_ON(scsi_current_cmnd.nexus == INVALID_SCSI_NEXUS);
	scsi_nexus_select(scsi_current_cmnd.nexus);

	if (scsi_target_task.state != SCSI_TASK_ENABLED)
		return false;

	if (scsi_current_cmnd.state == SCSI_CMND_AVAL)
		scsi_cmnd_aval();
	if (scsi_current_cmnd.state == SCSI_CMND_CMPL)
		scsi_cmnd_cmpl();
	if (scsi_current_cmnd.state != SCSI_CMND_DONE)
		return false;

	/* notifier callback */
	if (!sync) {
		scsi_transport_t *xprt = scsi_target_xprt;

		BUG_ON(scsi_pid >= scsi_nr_pids);
		BUG_ON(!xprt);
		BUG_ON(!xprt->cmnd_cmpl);
		xprt->cmnd_cmpl();
	}
	return true;
}

static void scsi_task_route(scsi_lun_t lun,
			    uint8_t attr, uint8_t ctrl,
			    uint8_t *cdb, uint8_t len)
{
	if (SCSI_CONTROL_LINK(ctrl)) {
		scsi_link_valid(attr, ctrl);
		return;
	} else {
		uint8_t opcode = cdb[0];

		/* SAM-3 5.9.4 Incorrect logical unit selection */
		if (lun >= scsi_nr_luns) {
			if (opcode != SCSI_CMND_REQUEST_SENSE &&
				opcode != SCSI_CMND_INQUIRY &&
				opcode != SCSI_CMND_REPORT_LUNS) {
				scsi_illegal_request(SCSI_ASC_LOGICAL_UNIT_NOT_SUPPORTED);
				return;
			}
		}
		scsi_unit_select(lun);

		if (!scsi_task_ready()) {
			scsi_lack_task_resource();
			return;
		}

		if (!scsi_aca_valid()) {
			scsi_command_done(SCSI_STATUS_ACA_ACTIVE);
			return;
		}
#if 0
		if (!scsi_ua_valid()) {
			scsi_command_done(SCSI_STATUS_ACA_ACTIVE);
			return;
		}
#endif
		/* save task attributes */
		scsi_target_task.attr = attr;
		scsi_target_task.pid = scsi_pid;
		scsi_target_task.lun = scsi_lun;

		/* save cmnd attributes */
		scsi_current_cmnd.nexus = scsi_nexus_current();
		scsi_current_cmnd.state = SCSI_CMND_BUSY;
		scsi_current_cmnd.opcode = opcode;
		scsi_current_cmnd.control = ctrl;
		scsi_cmnd_parse(cdb, len);

		scsi_task_set_enter();
		scsi_cmnd_set_enter();
	}
}

void scsi_implicit_head_of_queue(void)
{
	scsi_target_task.attr = SCSI_TASK_HEAD_OF_QUEUE;
}

scsi_crn_t scsi_command_validate(uint8_t iid, uint8_t tid,
				 scsi_pid_t pid, scsi_lun_t lun,
				 uint8_t *cdb, uint8_t len, uint8_t attr)
{
	uint8_t opcode = cdb[0];

	scsi_port_select(pid);
	/* should store tid/iid in scsi_current_cmnd */
	scsi_target_port.iid = iid;
	scsi_target_port.tid = tid;

	if (!scsi_cmnd_ready() || !scsi_sense_data_ready()) {
		return INVALID_SCSI_CRN;
	}
	scsi_current_cmnd.state = SCSI_CMND_DONE;

	switch (SCSI_CDB_GROUP(opcode)) {
	case SCSI_CDB_GROUP_6:
		scsi_task_route(lun, attr, cdb[5], cdb, len);
		break;
	case SCSI_CDB_GROUP_10_1:
	case SCSI_CDB_GROUP_10_2:
		scsi_task_route(lun, attr, cdb[9], cdb, len);
		break;
	case SCSI_CDB_GROUP_16:
		scsi_task_route(lun, attr, cdb[15], cdb, len);
		break;
	case SCSI_CDB_GROUP_12:
		scsi_task_route(lun, attr, cdb[11], cdb, len);
		break;
	case SCSI_CDB_GROUP_VAR:
	default:
		scsi_invalid_cmd_opcode();
		break;
	}

	return scsi_crn;
}

/* TODO: asynchronous call */
#if 0
static void scsi_schedule_tasks(void)
{
	/* TODO: wakeup on any non-empty task sets */
	scsi_cmnds_sched()
}

static void scsi_execute_task(scsi_nexus_t nexus)
{
	struct scsi_cmnd *last_cmnd;

	scsi_nexus_select(nexus);

	if (scsi_target_task.state != SCSI_TASK_ENABLED)
		return;

	last_cmnd = scsi_recent_cmnd;

	while (last_cmnd) {
		(void)__scsi_cmnd_execute();
		last_cmnd = last_cmnd->next;
	}

	scsi_schedule_tasks();
}
#endif

/*=========================================================================
 * transport events
 *=======================================================================*/
void scsi_nexus_loss(scsi_nexus_t nexus)
{
}

void scsi_transport_reset(scsi_pid_t pid)
{
}

static void scsi_task_set_init(void)
{
	scsi_head_of_queue = NULL;
	scsi_target_task_set.ACA = 0;
	scsi_target_task_set.UA = 0;
}

static void scsi_unit_init(void)
{
	scsi_target_unit.linked_tag = INVALID_SCSI_TAG;
	scsi_senses_init();
	scsi_task_set_init();
	scsi_tasks_init();
	scsi_mode_init();
}

scsi_lun_t scsi_register_device(scsi_device_t *dev)
{
	scsi_lun_t lun;
	BUG_ON(scsi_nr_luns >= INVALID_SCSI_LUN);
	lun = scsi_nr_luns;
	scsi_devices[lun] = dev;
	scsi_nr_luns++;
	return lun;
}

scsi_pid_t scsi_register_transport(scsi_transport_t *xprt)
{
	scsi_pid_t pid;
	BUG_ON(scsi_nr_pids >= INVALID_SCSI_PID);
	pid = scsi_nr_pids;
	scsi_transports[pid] = xprt;
	scsi_nr_pids++;
	return pid;
}

void scsi_target_init(void)
{
	scsi_lun_t lun;

	scsi_inquiry_init();
	scsi_cmnds_init();

	for (lun = 0; lun < NR_SCSI_UNITS; lun++) {
		scsi_unit_select(lun);
		scsi_unit_init();
	}
}
