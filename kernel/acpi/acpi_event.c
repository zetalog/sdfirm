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
 * @(#)acpi_event.c: ACPI event management implementation
 * $Id: acpi_event.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "acpi_int.h"

typedef void (*acpi_event_cb)(void);

struct acpi_event {
	uint8_t flags;
#define ACPI_EVENT_GARBAGE	0x01
	uint8_t invokings;
	acpi_event_cb handler;
	void *context;
};

acpi_mutex_t acpi_gbl_event_mutex;
static struct acpi_event acpi_gbl_event_table = { 0, 0, NULL, NULL };
static struct acpi_event acpi_gbl_event_space = { 0, 0, NULL, NULL };

static void acpi_event_lock(void)
{
	(void)acpi_os_acquire_mutex(acpi_gbl_event_mutex, ACPI_WAIT_FOREVER);
}

static void acpi_event_unlock(void)
{
	acpi_os_release_mutex(acpi_gbl_event_mutex);
}

static acpi_status_t __acpi_event_register_handler(struct acpi_event *event,
						   void *handler, void *context)
{
	if (!handler)
		return AE_BAD_PARAMETER;
	if (event->handler)
		return AE_ALREADY_EXISTS;

	event->handler = handler;
	event->context = context;
	event->invokings = 0;
	event->flags = 0;

	return AE_OK;
}

static void __acpi_event_unregister_handler(struct acpi_event *event,
					    acpi_event_cb handler)
{
	if (!handler)
		return;

	if (handler != event->handler ||
	    event->flags & ACPI_EVENT_GARBAGE)
		return;
	event->invokings++;
	event->flags |= ACPI_EVENT_GARBAGE;
	acpi_event_unlock();

	while (event->invokings != 1)
		acpi_os_sleep(10);

	acpi_event_lock();
	event->handler = NULL;
	event->context = NULL;
	event->invokings--;
	event->flags = 0;
}

static acpi_event_cb __acpi_event_lock_notify(struct acpi_event *event)
{
	acpi_event_cb handler = NULL;

	acpi_event_lock();
	if (event->handler && !(event->flags & ACPI_EVENT_GARBAGE)) {
		event->invokings++;
		handler = event->handler;
	}
	acpi_event_unlock();

	return handler;
}

static void __acpi_event_unlock_notify(struct acpi_event *event)
{
	acpi_event_lock();
	event->invokings--;
	acpi_event_unlock();
}

void acpi_event_table_notify(struct acpi_table_desc *table_desc,
			     acpi_ddb_t ddb, uint32_t event)
{
	acpi_event_table_cb handler;

	handler = (acpi_event_table_cb)
		__acpi_event_lock_notify(&acpi_gbl_event_table);
	if (handler) {
		/* Invoking callback without any locks held */
		handler(table_desc, ddb, event,
			acpi_gbl_event_table.context);
		__acpi_event_unlock_notify(&acpi_gbl_event_table);
	}
}

acpi_status_t acpi_event_register_table_handler(acpi_event_table_cb handler,
						void *context)
{
	acpi_status_t status;

	acpi_event_lock();
	status = __acpi_event_register_handler(&acpi_gbl_event_table,
					       (acpi_event_cb)handler,
					       context);
	if (ACPI_FAILURE(status))
		goto err_lock;
	acpi_event_unlock();
	acpi_table_notify_existing();
	acpi_event_lock();

err_lock:
	acpi_event_unlock();
	return status;
}

void acpi_event_unregister_table_handler(acpi_event_table_cb handler)
{
	acpi_event_lock();
	__acpi_event_unregister_handler(&acpi_gbl_event_table,
					(acpi_event_cb)handler);
	acpi_event_unlock();
}

acpi_status_t acpi_event_register_space_handler(acpi_event_space_cb handler,
						void *context)
{
	acpi_status_t status;

	acpi_event_lock();
	status = __acpi_event_register_handler(&acpi_gbl_event_space,
					       (acpi_event_cb)handler,
					       context);
	if (ACPI_FAILURE(status))
		goto err_lock;
	acpi_event_unlock();
	/* acpi_space_notify_existing(); */
	acpi_event_lock();

err_lock:
	acpi_event_unlock();
	return status;
}

void acpi_event_unregister_space_handler(acpi_event_space_cb handler)
{
	acpi_event_lock();
	__acpi_event_unregister_handler(&acpi_gbl_event_space,
					(acpi_event_cb)handler);
	acpi_event_unlock();
}

void acpi_event_space_notify(struct acpi_namespace_node *node, uint32_t event)
{
	acpi_event_space_cb handler;

	handler = (acpi_event_space_cb)
		__acpi_event_lock_notify(&acpi_gbl_event_space);
	if (handler) {
		/* Invoking callback without any locks held */
		handler(node, event,
			acpi_gbl_event_space.context);
		__acpi_event_unlock_notify(&acpi_gbl_event_space);
	}
}

acpi_status_t acpi_initialize_events(void)
{
	acpi_status_t status;

	status = acpi_os_create_mutex(&acpi_gbl_event_mutex);
	if (ACPI_FAILURE(status))
		return status;

	return status;
}
