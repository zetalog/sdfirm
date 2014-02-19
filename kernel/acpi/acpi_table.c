/*
 * ZETALOG's Personal COPYRIGHT v2
 *
 * Copyright (c) 2014
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 2. Permission of reuse of souce code only granted to ZETALOG and the
 *    developer(s) in the companies ZETALOG worked and has redistributed this
 *    software to.  Permission of redistribution of source code only granted
 *    to ZETALOG.
 * 3. Permission of redistribution and/or reuse of binary fully granted
 *    to ZETALOG and the companies ZETALOG worked and has redistributed this
 *    software to.
 * 4. Any modification of this software in the redistributed companies need
 *    not be published to ZETALOG.
 * 5. All source code modifications linked/included this software and modified
 *    by ZETALOG are of ZETALOG's personal COPYRIGHT unless the above COPYRIGHT
 *    is no long disclaimed.
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
 * @(#)acpi_table.c: ACPI table management implementation
 * $Id: acpi_table.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "acpi_int.h"

/*
 * NOTE: Function Naming Rules
 *
 * 1. For functions that should be invoked without lock, no prefix is added.
 * 2. For functions that should be invoked with lock, "__" prefix is added.
 * 3. For functions that do not care about the invocation environment, no
 *    prefix is added if there is no naming conflict with the above 2 rules,
 *    or "____" prefix is added if there is a naming conflict in order to
 *    follow the above 2 rules.
 */

struct acpi_table_list {
	struct acpi_table_desc **tables;
	uint32_t use_table_count;
	uint32_t max_table_count;
	struct acpi_reference all_table_count;
	uint8_t flags;
};

struct acpi_table_array {
	struct list_head link;
	struct acpi_table_desc tables[1];
};

#define ACPI_ROOT_ORIGIN_UNKNOWN        (0)     /* ~ORIGIN_ALLOCATED */
#define ACPI_ROOT_ORIGIN_ALLOCATED      (1)
#define ACPI_ROOT_ALLOW_RESIZE          (2)

#define ACPI_TABLE_LIST_INCREMENT	4

#define acpi_foreach_uninstalled_ddb(ddb, start)		\
	for (ddb = start;					\
	     ddb < acpi_gbl_table_list.use_table_count; ddb++)	\
		if (__acpi_table_is_uninstalled(ddb))
#define acpi_foreach_installed_ddb(ddb, start)			\
	for (ddb = start;					\
	     ddb < acpi_gbl_table_list.use_table_count; ddb++)	\
		if (__acpi_table_is_installed(ddb))

static acpi_ddb_t __acpi_table_increment(acpi_ddb_t ddb);
static void __acpi_table_decrement(acpi_ddb_t ddb);
static acpi_status_t acpi_table_install_temporal(struct acpi_table_desc *table_desc,
						 acpi_addr_t address, acpi_table_flags_t flags);
static void acpi_table_uninstall_temporal(struct acpi_table_desc *table_desc);

static struct acpi_table_list acpi_gbl_table_list;
static boolean acpi_gbl_table_module_dead = false;
static boolean acpi_gbl_table_module_busy = false;
acpi_mutex_t acpi_gbl_table_mutex;
uint8_t acpi_gbl_integer_bit_width = 64;
static LIST_HEAD(acpi_gbl_table_arrays);

static void acpi_table_lock(void)
{
	(void)acpi_os_acquire_mutex(acpi_gbl_table_mutex, ACPI_WAIT_FOREVER);
}

static void acpi_table_unlock(void)
{
	acpi_os_release_mutex(acpi_gbl_table_mutex);
}

/*
 * NOTE: Indirect Table Pointer Manager
 *
 * Due to the following reasons:
 * 1. acpi_table_header is referenced using acpi_ddb_t which is the index
 *    to acpi_gbl_table_list.tables array.
 * 2. acpi_table_desc pointers dereferenced from acpi_gbl_table_list
 *    should not reallocated and copied as the user side will be running
 *    in parallel.
 * 3. while we need to reallocate acpi_ddb_t array when number of tables
 *    grows.can be reallocated when needed.
 *
 * The ACPI_TABLE_SOLVE_INDIRECT is implemented and must be used to
 * replace any code to dereference a pointer of acpi_table_desc from
 * acpi_gbl_table_list.  There should not be additional pieces of code
 * including "acpi_gbl_table_list.tables" execpt this macro and the
 * code to implement the alloc/free of the table list.
 */
#define ACPI_TABLE_SOLVE_INDIRECT(ddb)			acpi_gbl_table_list.tables[(ddb)]
#define __acpi_table_solve_indirect(ddb)		\
	(ddb < acpi_gbl_table_list.use_table_count ?	\
	 acpi_gbl_table_list.tables[(ddb)] : NULL)

static struct acpi_table_desc *acpi_table_solve_indirect(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc;

	acpi_table_lock();
	table_desc = __acpi_table_solve_indirect(ddb);
	acpi_table_unlock();

	return table_desc;
}

static acpi_status_t __acpi_table_list_resize(void)
{
	struct acpi_table_desc **new_table_ids, **old_table_ids, *table_desc;
	uint32_t new_count, new_id_count, old_id_count;
	struct acpi_table_array *new_array;
	uint32_t i;

	if (!(acpi_gbl_table_list.flags & ACPI_ROOT_ALLOW_RESIZE) &&
	    acpi_gbl_table_list.use_table_count < acpi_gbl_table_list.max_table_count)
		return AE_OK;
	if (!(acpi_gbl_table_list.flags & ACPI_ROOT_ALLOW_RESIZE))
		return AE_SUPPORT;
	if (acpi_gbl_table_list.flags & ACPI_ROOT_ORIGIN_ALLOCATED &&
	    acpi_gbl_table_list.use_table_count < acpi_gbl_table_list.max_table_count)
		return AE_OK;

again:
	old_id_count = acpi_gbl_table_list.use_table_count;
	if (acpi_gbl_table_list.flags & ACPI_ROOT_ORIGIN_ALLOCATED) {
		new_id_count = acpi_gbl_table_list.max_table_count + ACPI_TABLE_LIST_INCREMENT;
		new_count = ACPI_TABLE_LIST_INCREMENT;
	} else {
		new_id_count = acpi_gbl_table_list.use_table_count + ACPI_TABLE_LIST_INCREMENT;
		new_count = new_id_count;
	}

	acpi_table_unlock();
	new_array = acpi_os_allocate_zeroed(sizeof (struct acpi_table_array) +
					    new_count * sizeof (struct acpi_table_desc) +
					    new_id_count * sizeof (struct acpi_table_desc *));
	acpi_table_lock();
	if (!new_array)
		return AE_NO_MEMORY;

	if (acpi_gbl_table_list.use_table_count != old_id_count) {
		acpi_table_unlock();
		acpi_os_free(new_array);
		acpi_os_sleep(10);
		acpi_table_lock();
		goto again;
	}
	new_table_ids = ACPI_ADD_PTR(struct acpi_table_desc *, new_array,
				     sizeof (struct acpi_table_array) +
				     new_count * sizeof (struct acpi_table_desc));
	INIT_LIST_HEAD(&new_array->link);
	list_add(&new_array->link, &acpi_gbl_table_arrays);

	if (acpi_gbl_table_list.flags & ACPI_ROOT_ORIGIN_ALLOCATED) {
		old_table_ids = acpi_gbl_table_list.tables;
		for (i = 0; i < old_id_count; i++)
			new_table_ids[i] = old_table_ids[i];
		for (i = 0; i < ACPI_TABLE_LIST_INCREMENT; i++)
			new_table_ids[old_id_count+i] = &new_array->tables[i];
	} else {
		for (i = 0; i < old_id_count; i++) {
			table_desc = ACPI_TABLE_SOLVE_INDIRECT(i);
			/*
			 * NOTE: If acpi_gbl_table_list.tables[i] is being used,
			 *       crash can happen.
			 */
			BUG_ON(1 != acpi_reference_get(&table_desc->reference_count));
			memcpy(&new_array->tables[i], table_desc,
			       sizeof (struct acpi_table_desc));
		}
		old_table_ids = NULL;
		for (i = 0; i < new_count; i++)
			new_table_ids[i] = &new_array->tables[i];
	}

	acpi_gbl_table_list.tables = new_table_ids;
	acpi_gbl_table_list.max_table_count = new_id_count;
	acpi_gbl_table_list.flags |= ACPI_ROOT_ORIGIN_ALLOCATED;

	if (old_table_ids) {
		acpi_table_unlock();
		/*
		 * NOTE: The old_table_ids includes old_tables which
		 *       should not be freed.
		 * Please uncomment the following line if the design has
		 * been changed.
		 *
		 * acpi_os_free(old_table_ids);
		 */
		acpi_table_lock();
	}

	return AE_OK;
}

acpi_status_t acpi_reallocate_root_table(void)
{
	acpi_status_t status;

	acpi_table_lock();
	if (acpi_gbl_table_list.flags & ACPI_ROOT_ORIGIN_ALLOCATED) {
		status = AE_SUPPORT;
		goto err_lock;
	}

	acpi_gbl_table_list.flags |= ACPI_ROOT_ALLOW_RESIZE;
	status = __acpi_table_list_resize();

err_lock:
	acpi_table_unlock();
	return AE_OK;
}

static acpi_status_t __acpi_table_list_allocate(uint32_t initialial_table_count)
{
	acpi_status_t status;

	acpi_gbl_table_list.max_table_count = initialial_table_count;
	acpi_gbl_table_list.flags = ACPI_ROOT_ALLOW_RESIZE;
	status = __acpi_table_list_resize();

	return status;
}

/*
 * NOTE: BUSY/DEAD Locking Facility
 *
 * This facility is used to accelerate the process of
 * acpi_finalize_tables() as it can reduce period waited in this
 * function.
 * The DEAD flag is set only when BUSY flag is not set so that the code
 * locked with BUSY flag can safely access the installed tables, for
 * example, the code pieces iterating the acpi_gbl_table_list and are
 * triggered from a module external to ACPI.
 * The BUSY flag must also be locked for the code pieces that have
 * conflicts with the acpi_finalize_tables(), for example, the
 * installation process will add tables to the acpi_gbl_table_list thus
 * can cause longer wait period.
 * The DEAD flag must not be unset in order to break the BUSY lock
 * waiters, it should be unset by some module text segment protection
 * mechanism.
 * Unlocking and re-locking inside of DEAD/BUSY locks are simple
 * acpi_table_unlock()/acpi_table_lock().
 * Note that all DEAD/BUSY locked operations are serialized.
 */
static boolean acpi_table_is_module_busy(void)
{
	return acpi_gbl_table_module_busy;
}

static boolean acpi_table_is_module_dead(void)
{
	return acpi_gbl_table_module_dead;
}

static boolean acpi_table_lock_dead(void)
{
retry:
	acpi_table_lock();
	if (acpi_table_is_module_dead()) {
		acpi_table_unlock();
		return false;
	}
	if (acpi_table_is_module_busy()) {
		acpi_table_unlock();
		acpi_os_sleep(10);
		goto retry;
	}
	acpi_gbl_table_module_dead = true;
	return true;
}

static boolean acpi_table_lock_busy(void)
{
retry:
	acpi_table_lock();
	if (acpi_table_is_module_dead()) {
		acpi_table_unlock();
		return false;
	}
	if (acpi_table_is_module_busy()) {
		acpi_table_unlock();
		acpi_os_sleep(10);
		goto retry;
	}
	acpi_gbl_table_module_busy = true;
	return true;
}

static void acpi_table_unlock_busy(void)
{
	acpi_gbl_table_module_busy = false;
	acpi_table_unlock();
}

static void acpi_table_unlock_dead(void)
{
	acpi_gbl_table_module_dead = false;
	acpi_table_unlock();
}

/*
 * NOTE: INSTALLED/UNINSTALLED States
 *
 * Note that __acpi_table_is_installed(ddb) doesn't equal to
 * !__acpi_table_is_uninstalled(ddb).  There are cases that a table
 * descriptor is neither installed nor uninstalled.  In this state, there
 * wouldn't be any newly issued operations on such a table descriptor, it
 * is simply dying.
 */
static boolean __acpi_table_is_installed(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc &&
	    acpi_reference_get(&table_desc->reference_count) > 0 &&
	    !(table_desc->flags & ACPI_TABLE_IS_UNINSTALLING))
		return true;
	return false;
}

boolean __acpi_table_is_uninstalled(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc && !(table_desc->flags & ACPI_TABLE_IS_ACQUIRED))
		return true;
	return false;
}

boolean __acpi_table_can_validate(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc && __acpi_table_is_installed(ddb) &&
	    !table_desc->pointer)
		return true;
	return false;
}

boolean __acpi_table_is_validated(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc && __acpi_table_is_installed(ddb) &&
	    table_desc->pointer)
		return true;
	return false;
}

static boolean __acpi_table_is_loaded(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc && __acpi_table_is_validated(ddb) &&
	    table_desc->flags & ACPI_TABLE_IS_LOADED)
		return true;
	return false;
}

static boolean __acpi_table_is_unloaded(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc = __acpi_table_solve_indirect(ddb);

	if (table_desc && table_desc->flags & ACPI_TABLE_IS_LOADED)
		return false;
	return true;
}

void __acpi_table_set_loaded(acpi_ddb_t ddb, boolean is_loaded)
{
	struct acpi_table_desc *table_desc;

	table_desc = __acpi_table_solve_indirect(ddb);
	if (table_desc) {
		if (is_loaded) {
			acpi_dbg("[%4.4s %d] INC(LOADED)",
				 table_desc->signature, ddb);
			__acpi_table_increment(ddb);
			table_desc->flags |= ACPI_TABLE_IS_LOADED;
		} else {
			acpi_dbg("[%4.4s %d] DEC(LOADED)",
				 table_desc->signature, ddb);
			__acpi_table_decrement(ddb);
			table_desc->flags &= ~ACPI_TABLE_IS_LOADED;
		}
	}
}

/*
 * NOTE: Unlocked Table Notification
 *
 * ACPI table notification is implemented in the way that the callback
 * to listen such events is invoked without any lock held.  It ensures
 * that the pointer of the table descriptor will not be freed while being
 * accessed inside of the callback.  The code to implement the event
 * listener may invoke acpi_get_table()/acpi_table_indrement() to obtain a
 * reference of such table descriptors.  There are possiblities that the
 * acpi_get_table() failed due to a concurrent uninstalltion, so the
 * return value of acpi_get_table() must be checked.
 * The reversals to release the reference are acpi_put_table() and
 * acpi_table_decrement().
 */
static void __acpi_table_notify(struct acpi_table_desc *table_desc,
				acpi_ddb_t ddb, uint32_t event)
{
	acpi_dbg("[%4.4s %d] INC(NOTIFY)", table_desc->signature, ddb);
	__acpi_table_increment(ddb);
	acpi_table_unlock();
	acpi_event_table_notify(table_desc, ddb, event);
	acpi_table_lock();
	acpi_dbg("[%4.4s %d] DEC(NOTIFY)", table_desc->signature, ddb);
	__acpi_table_decrement(ddb);
}

void acpi_table_notify_existing(void)
{
	acpi_ddb_t ddb;
	struct acpi_table_desc *table_desc;

	if (!acpi_table_lock_busy())
		return;
	acpi_foreach_installed_ddb(ddb, 0) {
		table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
		acpi_dbg("[%4.4s %d] INC(NOTIFY_EXIST)", table_desc->signature, ddb);
		__acpi_table_increment(ddb);
		__acpi_table_notify(table_desc, ddb, ACPI_EVENT_TABLE_INSTALL);
		/* TODO: Ordered LOAD/UNLOAD event support */
		if (__acpi_table_is_loaded(ddb))
			__acpi_table_notify(table_desc, ddb,
					    ACPI_EVENT_TABLE_LOAD);
		acpi_dbg("[%4.4s %d] DEC(NOTIFY_EXIST)", table_desc->signature, ddb);
		__acpi_table_decrement(ddb);
	}
	acpi_table_unlock_busy();
}

boolean acpi_table_contains_aml(struct acpi_table_header *table)
{
	return ACPI_NAMECMP(ACPI_SIG_DSDT, table->signature) ||
	       ACPI_NAMECMP(ACPI_SIG_PSDT, table->signature) ||
	       ACPI_NAMECMP(ACPI_SIG_SSDT, table->signature);
}

uint32_t acpi_table_get_length(struct acpi_table_header *table)
{
	if (ACPI_RSDP_SIG_CMP(ACPI_CAST_RSDP(table)->signature))
		return ACPI_DECODE32(&(ACPI_CAST_RSDP(table)->length));
	else
		return ACPI_DECODE32(&(table->length));
}

/*=========================================================================
 * Non-RSDP table checksums
 *=======================================================================*/
static void __acpi_table_calc_checksum(struct acpi_table_header *table)
{
	ACPI_ENCODE8(&table->checksum, 0);
	ACPI_ENCODE8(&table->checksum,
		     acpi_checksum_calc(table, acpi_table_get_length(table)));
}

boolean acpi_table_has_header(acpi_name_t signature)
{
	/*
	 * NOTE: No Normal Header
	 *
	 * Note that S3PT and FACS do not have normal ACPI table headers.
	 */
	return (!ACPI_NAMECMP(ACPI_SIG_S3PT, signature) &&
		!ACPI_NAMECMP(ACPI_SIG_FACS, signature));
}

boolean __acpi_table_checksum_valid(struct acpi_table_header *table)
{
	if (!acpi_table_has_header(table->signature))
		return true;

	return acpi_checksum_calc(table, acpi_table_get_length(table)) == 0;
}

boolean acpi_table_checksum_valid(struct acpi_table_header *table)
{
	if (ACPI_RSDP_SIG_CMP(table->signature))
		return acpi_rsdp_checksum_valid(ACPI_CAST_RSDP(table));
	else
		return __acpi_table_checksum_valid(table);
}

void acpi_table_calc_checksum(struct acpi_table_header *table)
{
	if (ACPI_RSDP_SIG_CMP(table->signature))
		acpi_rsdp_calc_checksum(ACPI_CAST_RSDP(table));
	else
		__acpi_table_calc_checksum(table);
}

static acpi_status_t __acpi_table_list_acquire(acpi_ddb_t *ddb_handle,
					       acpi_name_t name)
{
	acpi_status_t status;
	acpi_ddb_t ddb;
	struct acpi_table_desc *table_desc;

	if (acpi_table_is_module_dead())
		return AE_NOT_FOUND;

	if (ACPI_NAMECMP(ACPI_SIG_DSDT, name)) {
		ddb = ACPI_DDB_HANDLE_DSDT;
		goto out_succ;
	}
	if (ACPI_NAMECMP(ACPI_SIG_FACS, name)) {
		ddb = ACPI_DDB_HANDLE_FACS;
		goto out_succ;
	}
	acpi_foreach_uninstalled_ddb(ddb, ACPI_DDB_HANDLE_NON_FIXED)
		goto out_succ;

	status = __acpi_table_list_resize();
	if (ACPI_FAILURE(status))
		return status;
	ddb = acpi_gbl_table_list.use_table_count;
	acpi_gbl_table_list.use_table_count++;

out_succ:
	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	if (table_desc->flags & ACPI_TABLE_STATE_MASK)
		return AE_NOT_FOUND;
	acpi_reference_inc(&acpi_gbl_table_list.all_table_count);
	acpi_dbg("[%4.4s %d] INC(all_table_count %d)", name, ddb,
		 acpi_reference_get(&acpi_gbl_table_list.all_table_count));
	*ddb_handle = ddb;
	BUG_ON(acpi_reference_get(&table_desc->reference_count));
	memset(table_desc, 0, sizeof (struct acpi_table_desc));
	table_desc->flags = ACPI_TABLE_IS_ACQUIRED;
	return AE_OK;
}

void __acpi_table_list_release(acpi_ddb_t ddb, acpi_name_t name)
{
	struct acpi_table_desc *table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);

	table_desc->flags &= ~ACPI_TABLE_IS_ACQUIRED;
	acpi_reference_dec(&acpi_gbl_table_list.all_table_count);
	acpi_dbg("[%4.4s %d] DEC(all_table_count %d)", name, ddb,
		 acpi_reference_get(&acpi_gbl_table_list.all_table_count));
}

static acpi_status_t acpi_table_acquire(struct acpi_table_header **table_ptr,
					acpi_addr_t address,
					uint32_t length,
					acpi_table_flags_t flags,
					boolean external_usage)
{
	struct acpi_table_header *table = NULL, *orig_table;

	switch (flags & ACPI_TABLE_ORIGIN_MASK) {
	case ACPI_TABLE_INTERNAL_PHYSICAL:
		table = acpi_os_map_memory(address, length);
		break;
	case ACPI_TABLE_EXTERNAL_VIRTUAL:
	case ACPI_TABLE_INTERNAL_VIRTUAL:
		orig_table = ACPI_CAST_PTR(struct acpi_table_header,
					   ACPI_PHYSADDR_TO_PTR(address));
		if (!external_usage)
			table = orig_table;
		else {
			table = acpi_os_allocate(length);
			if (table)
				memcpy(table, orig_table, length);
		}
		break;
	}
	if (!table)
		return AE_NO_MEMORY;

	*table_ptr = table;
	return AE_OK;
}

static void acpi_table_release(struct acpi_table_header *table,
			       uint32_t table_len,
			       acpi_table_flags_t table_flags,
			       boolean external_usage)
{
	switch (table_flags & ACPI_TABLE_ORIGIN_MASK) {
	case ACPI_TABLE_INTERNAL_PHYSICAL:
		acpi_os_unmap_memory(table, table_len);
		break;
	case ACPI_TABLE_EXTERNAL_VIRTUAL:
	case ACPI_TABLE_INTERNAL_VIRTUAL:
		if (external_usage)
			acpi_os_free(table);
	default:
		break;
	}
}

/*
 * NOTE: INSTALLED/VALIDATED/INVALIDATED/UNINSTALLED States
 *
 * During OSPM early boot stages, the memory mapping mechanism should not
 * be used in the late boot stages.  So this is ensured by the INSTALLED
 * but not VALIDATED state during early boot stages.
 * INSTALLED: .address is valid
 * VALIDATED: .pointer is valid as long as the .address
 * INVALIDATED: .pointer is not valid
 * UNINSTALLED: .address is not valid as long as .pointer
 */
static acpi_status_t ____acpi_table_validate(struct acpi_table_desc *table_desc)
{
	acpi_status_t status = AE_OK;

	if (!table_desc->pointer)
		status = acpi_table_acquire(&table_desc->pointer,
					    table_desc->address,
					    table_desc->length,
					    table_desc->flags, false);

	return status;
}

static void ____acpi_table_invalidate(struct acpi_table_desc *table_desc)
{
	if (!table_desc->pointer)
		return;

	acpi_table_release(table_desc->pointer, table_desc->length,
			   table_desc->flags, false);
	table_desc->pointer = NULL;
}

static void __acpi_table_invalidate(struct acpi_table_desc *table_desc)
{
	uint32_t length;
	acpi_table_flags_t flags;
	struct acpi_table_header *table;

	if (!table_desc->pointer)
		return;

	length = table_desc->length;
	flags = table_desc->flags;
	table = table_desc->pointer;
	table_desc->pointer = NULL;

	acpi_table_unlock();
	acpi_table_release(table, length, table_desc->flags, false);
	acpi_table_lock();
}

acpi_status_t acpi_validate_table(acpi_ddb_t ddb)
{
	acpi_status_t status;
	struct acpi_table_desc *table_desc;
	uint32_t length;
	acpi_table_flags_t flags;
	acpi_addr_t address;
	struct acpi_table_header *table;

	acpi_table_lock();

	if (!__acpi_table_can_validate(ddb)) {
		status = AE_NOT_FOUND;
		goto err_lock;
	}

	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	acpi_dbg("[%4.4s %d] INC(VALIDATE)", table_desc->signature, ddb);
	__acpi_table_increment(ddb);

	length = table_desc->length;
	flags = table_desc->flags;
	address = table_desc->address;

	acpi_table_unlock();
	status = acpi_table_acquire(&table, address,
				    length, flags, false);
	acpi_table_lock();

	if (ACPI_SUCCESS(status)) {
		if (__acpi_table_can_validate(ddb))
			table_desc->pointer = table;
		else if (table) {
			acpi_table_unlock();
			acpi_table_release(table, length, flags, false);
			acpi_table_lock();
		}
	}

	if (__acpi_table_is_validated(ddb))
		status = AE_OK;
	else
		status = AE_NOT_FOUND;

	acpi_dbg("[%4.4s %d] DEC(VALIDATE)", table_desc->signature, ddb);
	__acpi_table_decrement(ddb);

err_lock:
	acpi_table_unlock();
	return status;
}

static acpi_status_t acpi_table_verify(struct acpi_table_desc *table_desc,
				       acpi_tag_t signature)
{
	acpi_status_t status;

	status = ____acpi_table_validate(table_desc);
	if (ACPI_FAILURE(status))
		return status;
	if (!ACPI_NAMECMP(signature, ACPI_NULL_NAME) &&
	    !ACPI_NAMECMP(signature, table_desc->signature))
		return AE_BAD_SIGNATURE;
	if (!__acpi_table_checksum_valid(table_desc->pointer))
		return AE_BAD_CHECKSUM;

	return AE_OK;
}

acpi_status_t acpi_table_increment_validated(acpi_ddb_t ddb,
					     struct acpi_table_header **out_table)
{
	struct acpi_table_desc *table_desc;
	acpi_status_t status = AE_OK;

	if (!out_table)
		return AE_BAD_PARAMETER;
	*out_table = NULL;
	acpi_table_lock();
	if (!__acpi_table_is_validated(ddb)) {
		status = AE_ALREADY_EXISTS;
		goto err_lock;
	}
	__acpi_table_increment(ddb);
	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	*out_table = table_desc->pointer;

err_lock:
	acpi_table_unlock();
	return status;
}

static void ____acpi_table_install(struct acpi_table_desc *table_desc,
				   acpi_addr_t address, acpi_table_flags_t flags,
				   struct acpi_table_header *table_header)
{
	table_desc->address = address;
	table_desc->length = acpi_table_get_length(table_header);
	table_desc->flags |= flags;
	ACPI_NAMECPY(ACPI_NAME2TAG(table_header->signature), table_desc->signature);
	if (acpi_table_has_header(table_header->signature)) {
		ACPI_OEMCPY(table_header->oem_id,
			    table_desc->oem_id, ACPI_OEM_ID_SIZE);
		ACPI_OEMCPY(table_header->oem_table_id,
			    table_desc->oem_table_id, ACPI_OEM_ID_SIZE);
		table_desc->revision = table_header->revision;
	}
}

static void ____acpi_table_uninstall(struct acpi_table_desc *table_desc)
{
	void *address = NULL;

	if (!table_desc->address)
		return;

	____acpi_table_invalidate(table_desc);
	if ((table_desc->flags & ACPI_TABLE_ORIGIN_MASK) == ACPI_TABLE_INTERNAL_VIRTUAL)
		address = ACPI_CAST_PTR(void, table_desc->address);
	table_desc->address = ACPI_PTR_TO_PHYSADDR(NULL);
	table_desc->length = 0;
	table_desc->flags &= ~ACPI_TABLE_ORIGIN_MASK;

	if (address)
		acpi_os_free(address);
}

static void __acpi_table_uninstall(struct acpi_table_desc *table_desc)
{
	void *address = NULL;

	if (!table_desc->address)
		return;

	__acpi_table_invalidate(table_desc);
	if ((table_desc->flags & ACPI_TABLE_ORIGIN_MASK) == ACPI_TABLE_INTERNAL_VIRTUAL)
		address = ACPI_CAST_PTR(void, table_desc->address);
	table_desc->address = ACPI_PTR_TO_PHYSADDR(NULL);
	table_desc->length = 0;
	table_desc->flags &= ~ACPI_TABLE_ORIGIN_MASK;

	if (address) {
		acpi_table_unlock();
		acpi_os_free(address);
		acpi_table_lock();
	}
}

static void acpi_table_override(struct acpi_table_desc *old_table_desc)
{
	acpi_status_t status;
	char *override_type;
	struct acpi_table_desc new_table_desc;
	acpi_addr_t address;
	acpi_table_flags_t flags;

	status = acpi_os_table_override(old_table_desc->pointer,
					&address, &flags);
	if (ACPI_SUCCESS(status) && address) {
		acpi_table_install_temporal(&new_table_desc, address, flags);
		override_type = "Logical";
		goto out_succ;
	}

	return;

out_succ:
	status = acpi_table_verify(&new_table_desc, ACPI_TAG_NULL);
	if (ACPI_FAILURE(status))
		return;

	____acpi_table_uninstall(old_table_desc);
	____acpi_table_install(old_table_desc, new_table_desc.address,
			       new_table_desc.flags, new_table_desc.pointer);
	____acpi_table_validate(old_table_desc);
	acpi_table_uninstall_temporal(&new_table_desc);
}

static void acpi_determine_integer_width(uint8_t revision)
{
	if (revision < 2)
		acpi_gbl_integer_bit_width = 32;
	else
		acpi_gbl_integer_bit_width = 64;
}

static void __acpi_table_install_and_override(struct acpi_table_desc *new_table_desc,
					      acpi_ddb_t ddb, boolean override)
{
	struct acpi_table_desc *table_desc;

	BUG_ON(ddb >= acpi_gbl_table_list.use_table_count);

	if (override)
		acpi_table_override(new_table_desc);

	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	____acpi_table_install(table_desc,
			       new_table_desc->address, new_table_desc->flags,
			       new_table_desc->pointer);
	/* Acquire the MANAGED reference */
	acpi_dbg("[%4.4s %d] INC(MANAGED)", table_desc->signature, ddb);
	BUG_ON(acpi_reference_get(&table_desc->reference_count) != 0);
	acpi_reference_set(&table_desc->reference_count, 1);
	__acpi_table_notify(table_desc, ddb, ACPI_EVENT_TABLE_INSTALL);

	if (ddb == ACPI_DDB_HANDLE_DSDT)
		acpi_determine_integer_width(ACPI_DECODE8(&new_table_desc->pointer->revision));
}

boolean acpi_table_is_same(struct acpi_table_desc *table_desc, acpi_tag_t sig,
			   char *oem_id, char *oem_table_id)
{
	if (ACPI_NAMECMP(ACPI_SIG_DSDT, table_desc->signature) ||
	    ACPI_NAMECMP(ACPI_SIG_FADT, table_desc->signature) ||
	    !acpi_table_has_header(table_desc->signature))
		return ACPI_NAMECMP(sig, table_desc->signature) ? true: false;

	return ((ACPI_NAMECMP(sig, table_desc->signature) &&
		 ACPI_OEMCMP(oem_id, table_desc->oem_id, ACPI_OEM_ID_SIZE)&&
		 ACPI_OEMCMP(oem_table_id, table_desc->oem_table_id, ACPI_OEM_TABLE_ID_SIZE)) ?
		true : false);
}

static acpi_status_t __acpi_uninstall_table(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc;

	if (!__acpi_table_is_installed(ddb))
		return AE_NOT_FOUND;

	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	table_desc->flags |= ACPI_TABLE_IS_UNINSTALLING;

	if (!__acpi_table_is_unloaded(ddb)) {
		__acpi_table_notify(table_desc, ddb,
				    ACPI_EVENT_TABLE_UNLOAD);
		/* acpi_table_unparse(ddb); */
		__acpi_table_set_loaded(ddb, false);
	}

	__acpi_table_notify(table_desc, ddb, ACPI_EVENT_TABLE_UNINSTALL);

	/* Release the MANAGED reference */
	acpi_dbg("[%4.4s %d] DEC(MANAGED)", table_desc->signature, ddb);
	__acpi_table_decrement(ddb);

	return AE_OK;
}

acpi_status_t acpi_install_table(acpi_addr_t address, acpi_tag_t signature,
				 acpi_table_flags_t flags,
				 boolean override, boolean versioning,
				 acpi_ddb_t *ddb_handle)
{
	acpi_ddb_t ddb;
	acpi_status_t status;
	struct acpi_table_desc new_table_desc;
	struct acpi_table_desc *table_desc;

	if (!address) {
		acpi_err("[0x%X]: Null physical address for ACPI table", signature);
		return AE_NO_MEMORY;
	}

	status = acpi_table_install_temporal(&new_table_desc, address, flags);
	if (ACPI_FAILURE(status)) {
		acpi_err("[0x%X]: Could not acquire table length at %p", signature,
			 ACPI_CAST_PTR(void, address));
		return status;
	}

	status = acpi_table_verify(&new_table_desc, signature);
	if (ACPI_FAILURE(status))
		return status;

	acpi_table_lock();

	acpi_foreach_installed_ddb(ddb, 0) {
		table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);

		if (!acpi_table_is_same(table_desc,
					ACPI_NAME2TAG(new_table_desc.signature),
					new_table_desc.oem_id,
					new_table_desc.oem_table_id))
			continue;
		if (versioning && new_table_desc.revision <= table_desc->revision) {
			status = AE_ALREADY_EXISTS;
			goto err_lock;
		}

		status = __acpi_uninstall_table(ddb);
		if (ACPI_FAILURE(status))
			goto err_lock;

		/* Wait until uninstall completes, for FACS and DSDT reloading */
		while (table_desc->flags & ACPI_TABLE_IS_UNINSTALLING) {
			acpi_table_unlock();
			acpi_os_sleep(10);
			acpi_table_lock();
		}

		break;
	}

	status = __acpi_table_list_acquire(&ddb, new_table_desc.signature);
	if (ACPI_FAILURE(status))
		goto err_lock;

	*ddb_handle = ddb;
	__acpi_table_install_and_override(&new_table_desc, ddb, override);

err_lock:
	acpi_table_unlock();
	acpi_table_uninstall_temporal(&new_table_desc);
	return status;
}

void acpi_uninstall_table(acpi_ddb_t ddb)
{
	acpi_table_lock();
	__acpi_uninstall_table(ddb);
	acpi_table_unlock();
}

/*
 * NOTE: Install/Uninstall Temporal Table
 *
 * The temporal function only used during the table installation period,
 * where the .address member is valid and .pointer member is not touched.
 * This is used to temporary store a table descriptor that is not managed
 * by acpi_gbl_table_list.  This file uses new_table_desc/old_table_desc
 * to indicate such temporal table descriptors.
 */
static void acpi_table_uninstall_temporal(struct acpi_table_desc *table_desc)
{
	____acpi_table_invalidate(table_desc);
}

static acpi_status_t acpi_table_install_temporal(struct acpi_table_desc *table_desc,
						 acpi_addr_t address, acpi_table_flags_t flags)
{
	struct acpi_table_header *table_header;

	switch (flags & ACPI_TABLE_ORIGIN_MASK) {
	case ACPI_TABLE_INTERNAL_PHYSICAL:
		table_header = acpi_os_map_memory(address, sizeof (struct acpi_table_header));
		if (!table_header)
			return AE_NO_MEMORY;
		memset(table_desc, 0, sizeof (struct acpi_table_desc));
		____acpi_table_install(table_desc, address, flags, table_header);
		acpi_os_unmap_memory(table_header, sizeof (struct acpi_table_header));
		return AE_OK;
	case ACPI_TABLE_INTERNAL_VIRTUAL:
	case ACPI_TABLE_EXTERNAL_VIRTUAL:
		table_header = ACPI_CAST_PTR(struct acpi_table_header, address);
		memset(table_desc, 0, sizeof (struct acpi_table_desc));
		____acpi_table_install(table_desc, address, flags, table_header);
		return AE_OK;
	}

	return AE_NO_MEMORY;
}

acpi_ddb_t ____acpi_table_increment(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc;
	int count;

	BUG_ON(ddb >= acpi_gbl_table_list.use_table_count);
	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	count = acpi_reference_test_and_inc(&table_desc->reference_count);
	if (table_desc->flags & ACPI_TABLE_IS_ACQUIRED && count) {
		acpi_dbg("[%4.4s %d] acpi_table_increment %d",
			 table_desc->signature, ddb, count);
		return ddb;
	}

	return ACPI_DDB_HANDLE_INVALID;
}

acpi_ddb_t __acpi_table_increment(acpi_ddb_t ddb)
{
	BUG_ON(ddb >= acpi_gbl_table_list.use_table_count);
	ddb = ____acpi_table_increment(ddb);
	BUG_ON(ddb == ACPI_DDB_HANDLE_INVALID);

	return ddb;
}

acpi_ddb_t acpi_table_increment(acpi_ddb_t ddb)
{
	acpi_table_lock();
	ddb = ____acpi_table_increment(ddb);
	acpi_table_unlock();

	return ddb;
}

void __acpi_table_decrement(acpi_ddb_t ddb)
{
	struct acpi_table_desc *table_desc;
	int count;

	BUG_ON(ddb >= acpi_gbl_table_list.use_table_count);
	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	count = acpi_reference_dec_and_test(&table_desc->reference_count);
	acpi_dbg("[%4.4s %d] acpi_table_decrement %d",
		 table_desc->signature, ddb, count);
	if (count == 0) {
		__acpi_table_uninstall(table_desc);
		__acpi_table_list_release(ddb, table_desc->signature);
		table_desc->flags &= ~ACPI_TABLE_IS_UNINSTALLING;
		BUG_ON(table_desc->flags & ACPI_TABLE_STATE_MASK);
	}
}

void acpi_table_decrement(acpi_ddb_t ddb)
{
	acpi_table_lock();
	__acpi_table_decrement(ddb);
	acpi_table_unlock();
}

acpi_status_t acpi_parse_table(struct acpi_table_header *table,
			       struct acpi_namespace_node *start_node)
{
	acpi_status_t status;
	uint32_t aml_length;
	uint8_t *aml_start;

	BUG_ON(table->length < sizeof (struct acpi_table_header));

	aml_start = (uint8_t *)table + sizeof (struct acpi_table_header);
	aml_length = table->length - sizeof (struct acpi_table_header);

	status = acpi_parse_aml(aml_start, aml_length, start_node);
	if (ACPI_FAILURE(status))
		goto err_ref;

err_ref:
	return status;
}

static acpi_status_t acpi_load_table(acpi_ddb_t ddb, struct acpi_namespace_node *node)
{
	acpi_status_t status = AE_OK;
	struct acpi_table_desc *table_desc;
	struct acpi_table_header *table;

	if (ACPI_FAILURE(acpi_validate_table(ddb)))
		return AE_NOT_FOUND;
	if (ACPI_FAILURE(acpi_table_increment_validated(ddb, &table)))
		return AE_NOT_FOUND;

	acpi_table_lock();
	if (__acpi_table_is_loaded(ddb)) {
		status = AE_OK;
		goto err_ref;
	}

	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	table = table_desc->pointer;

#if 0
	/* Invoking the parser */
	status = acpi_parse_table(table, node);
	if (ACPI_SUCCESS(status)) {
		__acpi_table_set_loaded(ddb, true);
		__acpi_table_notify(table_desc, ddb, ACPI_EVENT_TABLE_LOAD);
	}
#endif

err_ref:
	acpi_table_unlock();
	acpi_table_decrement(ddb);
	return status;
}

acpi_status_t acpi_install_and_load_table(struct acpi_table_header *table,
					  acpi_table_flags_t flags,
					  boolean versioning,
					  acpi_ddb_t *ddb_handle)
{
	acpi_status_t status;
	acpi_ddb_t ddb;

	if (!table || !ddb_handle)
		return AE_BAD_PARAMETER;

	status = acpi_install_table(ACPI_PTR_TO_PHYSADDR(table), ACPI_TAG_NULL,
				    flags, false, versioning, &ddb);
	if (ACPI_FAILURE(status))
		return status;

	status = acpi_load_table(ddb, acpi_gbl_root_node);
	if (ACPI_FAILURE(status))
		return status;

	*ddb_handle = ddb;
	return AE_OK;
}

static acpi_status_t __acpi_get_table(acpi_ddb_t ddb,
				      struct acpi_table *out_table)
{
	acpi_status_t status;
	struct acpi_table_desc *table_desc;
	acpi_addr_t address;

	if (!__acpi_table_is_installed(ddb))
		return AE_NOT_FOUND;

	table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
	acpi_dbg("[%4.4s %d] INC(GET)", table_desc->signature, ddb);
	__acpi_table_increment(ddb);

	out_table->length = table_desc->length;
	out_table->flags = table_desc->flags;
	address = table_desc->address;

	acpi_table_unlock();
	status = acpi_table_acquire(&out_table->pointer, address,
				    out_table->length, out_table->flags,
				    true);
	acpi_table_lock();

	if (!__acpi_table_is_installed(ddb)) {
		status = AE_NOT_FOUND;
		acpi_table_unlock();
		acpi_table_release(out_table->pointer,
				   out_table->length, out_table->flags,
				   true);
		acpi_table_lock();
		out_table->pointer = NULL;
	}

	acpi_dbg("[%4.4s %d] DEC(GET)", table_desc->signature, ddb);
	__acpi_table_decrement(ddb);
	return status;
}

acpi_status_t acpi_get_table(acpi_ddb_t ddb, struct acpi_table *out_table)
{
	acpi_status_t status;

	if (!out_table)
		return AE_BAD_PARAMETER;
	out_table->pointer = NULL;

	acpi_table_lock();
	status = __acpi_get_table(ddb, out_table);
	acpi_table_unlock();
	return status;
}

void acpi_put_table(struct acpi_table *table)
{
	acpi_dbg("[%4.4s %d] DEC(PUT)",
		 table->pointer->signature, ACPI_DDB_HANDLE_INVALID);
	acpi_table_release(table->pointer,
			   table->length, table->flags, true);
}

acpi_status_t acpi_get_table_by_inst(acpi_tag_t sig, uint32_t instance,
				     struct acpi_table *out_table)
{
	acpi_status_t status = AE_NOT_FOUND;
	acpi_ddb_t ddb;
	uint32_t j = 0;
	struct acpi_table_desc *table_desc;

	if (ACPI_NAMECMP(sig,  ACPI_NULL_NAME) || !out_table)
		return AE_BAD_PARAMETER;
	out_table->pointer = NULL;

	acpi_table_lock();
	acpi_foreach_installed_ddb(ddb, 0) {
		table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);

		if (!ACPI_NAMECMP(sig, table_desc->signature))
			continue;
		if (++j < instance)
			continue;

		status = __acpi_get_table(ddb, out_table);
		break;
	}
	acpi_table_unlock();

	return status;
}

acpi_status_t acpi_get_table_by_name(acpi_tag_t sig, char *oem_id, char *oem_table_id,
				     struct acpi_table *out_table)
{
	acpi_status_t status = AE_NOT_FOUND;
	acpi_ddb_t ddb;
	struct acpi_table_desc *table_desc;

	if (ACPI_NAMECMP(sig,  ACPI_NULL_NAME) ||
	    !oem_id || !oem_table_id || !out_table)
		return AE_BAD_PARAMETER;
	out_table->pointer = NULL;

	acpi_table_lock();
	acpi_foreach_installed_ddb(ddb, 0) {
		table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);

		if (!acpi_table_is_same(table_desc, sig, oem_id, oem_table_id))
			continue;

		status = __acpi_get_table(ddb, out_table);
		break;
	}
	acpi_table_unlock();

	return status;
}

void acpi_load_tables(void)
{
	acpi_ddb_t ddb;
	struct acpi_table_desc *table_desc;

	acpi_table_lock();

	acpi_foreach_installed_ddb(ddb, 0) {
		table_desc = ACPI_TABLE_SOLVE_INDIRECT(ddb);
		if ((!ACPI_NAMECMP(ACPI_SIG_DSDT,
				   table_desc->signature) &&
		     !ACPI_NAMECMP(ACPI_SIG_SSDT,
				   table_desc->signature) &&
		     !ACPI_NAMECMP(ACPI_SIG_PSDT,
				   table_desc->signature)))
			continue;

		acpi_table_unlock();
		(void)acpi_load_table(ddb, acpi_gbl_root_node);
		acpi_table_lock();
	}

	acpi_table_unlock();
}

acpi_status_t acpi_initialize_tables(struct acpi_table_desc **initial_table_array,
				     uint32_t initial_table_count,
				     boolean allow_resize)
{
	acpi_addr_t rsdp_address;
	acpi_status_t status;
	uint32_t i;

	status = acpi_os_create_mutex(&acpi_gbl_table_mutex);
	if (ACPI_FAILURE(status))
		return status;

	rsdp_address = acpi_os_get_root_pointer();
	if (!rsdp_address)
		return AE_NOT_FOUND;

	acpi_table_lock();
	if (!initial_table_array) {
		status = __acpi_table_list_allocate(initial_table_count);
		if (status)
			return status;
	} else {
		for (i = 0; i < initial_table_count; i++)
			memset(initial_table_array[i], 0, sizeof (struct acpi_table_desc));
		acpi_gbl_table_list.tables = initial_table_array;
		acpi_gbl_table_list.max_table_count = initial_table_count;
		acpi_gbl_table_list.flags = ACPI_ROOT_ORIGIN_UNKNOWN;
		if (allow_resize)
			acpi_gbl_table_list.flags |= ACPI_ROOT_ALLOW_RESIZE;
	}
	acpi_reference_set(&acpi_gbl_table_list.all_table_count, 0);
	acpi_gbl_table_list.use_table_count = ACPI_DDB_HANDLE_NON_FIXED;
	acpi_table_unlock();

	return acpi_rsdp_parse(rsdp_address);
}

void acpi_finalize_tables(void)
{
	acpi_ddb_t ddb;
	struct acpi_table_array *pos, *array;

	if (!acpi_table_lock_dead())
		return;
again:
	acpi_foreach_installed_ddb(ddb, 0) {
		__acpi_uninstall_table(ddb);
	}

	acpi_table_unlock();
	acpi_os_sleep(1000);
	acpi_table_lock();
	if (acpi_reference_get(&acpi_gbl_table_list.all_table_count) != 0)
		goto again;

	if (acpi_gbl_table_list.flags & ACPI_ROOT_ORIGIN_ALLOCATED) {
		list_for_each_entry_safe(struct acpi_table_array, array, pos, &acpi_gbl_table_arrays, link) {
			list_del(&array->link);
			acpi_table_unlock();
			acpi_os_free(array);
			acpi_table_lock();
		}
	}
	acpi_gbl_table_list.tables = NULL;
	acpi_gbl_table_list.flags = 0;
	acpi_gbl_table_list.use_table_count = 0;
	acpi_gbl_table_list.max_table_count = 0;
	acpi_table_unlock();
}
