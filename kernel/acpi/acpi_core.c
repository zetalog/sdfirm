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
 * @(#)acpi_core.c: ACPI locking and synchronization implementation
 * $Id: acpi_core.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "acpi_int.h"

static acpi_spinlock_t acpi_gbl_reference_lock;
boolean acpi_gbl_early_stage = true;

/*=========================================================================
 * Reference counting
 *=======================================================================*/
static void acpi_reference_update(struct acpi_reference *reference,
				  int action, int *new_count)
{
	acpi_cpuflags_t flags;

	if (!reference)
		return;

	flags = acpi_os_acquire_lock(acpi_gbl_reference_lock);
	switch (action) {
	case REF_INCREMENT_FORCE:
		if (new_count)
			*new_count = reference->count;
		reference->count++;
		break;
	case REF_INCREMENT:
		if (new_count)
			*new_count = reference->count;
		if (reference->count > 0)
			reference->count++;
		break;
	case REF_DECREMENT:
		reference->count--;
		if (new_count)
			*new_count = reference->count;
		break;
	default:
		return;
	}
	acpi_os_release_lock(acpi_gbl_reference_lock, flags);
}

void acpi_reference_inc(struct acpi_reference *reference)
{
	acpi_reference_update(reference, REF_INCREMENT_FORCE, NULL);
}

void acpi_reference_dec(struct acpi_reference *reference)
{
	acpi_reference_update(reference, REF_DECREMENT, NULL);
}

int acpi_reference_test_and_inc(struct acpi_reference *reference)
{
	int count;
	acpi_reference_update(reference, REF_INCREMENT, &count);
	return count;
}

int acpi_reference_dec_and_test(struct acpi_reference *reference)
{
	int count;
	acpi_reference_update(reference, REF_DECREMENT, &count);
	return count;
}

void acpi_reference_set(struct acpi_reference *reference, int count)
{
	acpi_cpuflags_t flags;

	if (!reference)
		return;

	flags = acpi_os_acquire_lock(acpi_gbl_reference_lock);
	reference->count = count;
	acpi_os_release_lock(acpi_gbl_reference_lock, flags);
}

int acpi_reference_get(struct acpi_reference *reference)
{
	acpi_cpuflags_t flags;
	int count;

	if (!reference)
		return 0;

	flags = acpi_os_acquire_lock(acpi_gbl_reference_lock);
	count = reference->count;
	acpi_os_release_lock(acpi_gbl_reference_lock, flags);

	return count;
}

/*=========================================================================
 * Object abstraction
 *=======================================================================*/
static void __acpi_object_init(struct acpi_object *object,
			       uint8_t type, acpi_release_cb release)
{
	object->descriptor_type = type;
	acpi_reference_set(&object->reference_count, 1);
	object->release = release;
}

struct acpi_object *acpi_object_open(uint8_t type,
					    acpi_size_t size,
					    acpi_release_cb release)
{
	struct acpi_object *object;

	object = acpi_os_allocate_zeroed(size);
	if (object)
		__acpi_object_init(object, type, release);

	return object;
}

void acpi_object_close(struct acpi_object *object)
{
	if (!object || acpi_object_is_closing(object))
		return;

	object->closing = true;
	if (!acpi_reference_dec_and_test(&object->reference_count)) {
		if (object->release)
			object->release(object);
		acpi_os_free(object);
	}
}

void acpi_object_get(struct acpi_object *object)
{
	if (object)
		acpi_reference_inc(&object->reference_count);
}

boolean acpi_object_is_closing(struct acpi_object *object)
{
	return object->closing ? true : false;
}

struct acpi_object *acpi_object_get_graceful(struct acpi_object *object)
{
	if (!object || acpi_object_is_closing(object))
		return NULL;
	acpi_object_get(object);
	return object;
}

void acpi_object_put(struct acpi_object *object)
{
	if (object) {
		if (!acpi_reference_dec_and_test(&object->reference_count)) {
			if (object->release)
				object->release(object);
			acpi_os_free(object);
		}
	}
}

/*=========================================================================
 * Stacked states
 *=======================================================================*/
void acpi_state_push(struct acpi_state **head, struct acpi_state *state)
{
	state->next = *head;
	*head = state;

	return;
}

struct acpi_state *acpi_state_pop(struct acpi_state **head)
{
	struct acpi_state *state;

	state = *head;
	if (state)
		*head = state->next;

	return state;
}

static void __acpi_state_init(struct acpi_state *state, uint8_t type,
			      acpi_release_cb release)
{
	state->object_type = type;
	state->next = NULL;
	state->release_state = release;
}

static void __acpi_state_exit(struct acpi_object *object)
{
	struct acpi_state *state =
		ACPI_CAST_PTR(struct acpi_state, object);

	if (state->release_state)
		state->release_state(object);
}

struct acpi_state *acpi_state_open(uint8_t type, acpi_size_t size,
				   acpi_release_cb release)
{
	struct acpi_object *object;
	struct acpi_state *state = NULL;

	object = acpi_object_open(ACPI_DESC_TYPE_STATE, size,
				  __acpi_state_exit);
	state = ACPI_CAST_PTR(struct acpi_state, object);
	if (state)
		__acpi_state_init(state, type, release);

	return state;
}

void acpi_state_close(struct acpi_state *state)
{
	acpi_object_close(ACPI_CAST_PTR(struct acpi_object, state));
}

acpi_status_t acpi_initialize_subsystem(void)
{
	acpi_status_t status;
	
	status = acpi_os_create_lock(&acpi_gbl_reference_lock);
	if (ACPI_FAILURE(status))
		return status;
	status = acpi_initialize_events();
	if (ACPI_FAILURE(status))
		return status;

	return AE_OK;
}

/*=========================================================================
 * Stacked scopes
 *=======================================================================*/
static void __acpi_scope_init(struct acpi_scope *scope,
			      struct acpi_namespace_node *node)
{
	scope->node = acpi_node_get(node, "scope");
}

static void __acpi_scope_exit(struct acpi_object *object)
{
	struct acpi_scope *scope =
		ACPI_CAST_PTR(struct acpi_scope, object);

	if (scope->node) {
		acpi_node_put(scope->node, "scope");
		scope->node = NULL;
	}
}

struct acpi_scope *acpi_scope_open(struct acpi_namespace_node *node)
{
	struct acpi_state *state;
	struct acpi_scope *scope = NULL;

	state = acpi_state_open(ACPI_STATE_SCOPE,
				sizeof (struct acpi_scope),
				__acpi_scope_exit);
	scope = ACPI_CAST_PTR(struct acpi_scope, state);
	if (scope)
		__acpi_scope_init(scope, node);

	return scope;
}

void acpi_scope_close(struct acpi_scope *scope)
{
	acpi_state_close(ACPI_CAST_PTR(struct acpi_state, scope));
}

struct acpi_scope *acpi_scope_init(struct acpi_namespace_node *node)
{
	return acpi_scope_open(node);
}

void acpi_scope_exit(struct acpi_scope *scope)
{
	while (scope)
		acpi_scope_pop(&scope);
}

acpi_status_t acpi_scope_push(struct acpi_scope **curr_scope,
			      struct acpi_namespace_node *node)
{
	struct acpi_scope *next_state;

	next_state = acpi_scope_open(node);
	if (!next_state)
		return AE_NO_MEMORY;

	acpi_state_push(ACPI_CAST_PTR(struct acpi_state *, curr_scope),
			ACPI_CAST_PTR(struct acpi_state, next_state));
	BUG_ON(next_state != *curr_scope);
	return AE_OK;
}

void acpi_scope_pop(struct acpi_scope **curr_scope)
{
	struct acpi_state *last_state;

	last_state = acpi_state_pop(ACPI_CAST_PTR(struct acpi_state *,
				    curr_scope));
	if (last_state)
		acpi_scope_close(ACPI_CAST_PTR(struct acpi_scope, last_state));
}
