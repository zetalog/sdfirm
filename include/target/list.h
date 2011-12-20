/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2005
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@hzcnc.com
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
 * @(#)list.h: simple doubly linked list implementation
 * $Id: list.h,v 1.8 2011-10-27 07:59:36 zhenglv Exp $
 */

#ifndef __LIST_H_INCLUDE__
#define __LIST_H_INCLUDE__

#include <target/types.h>

/*
 * Copied from LINUX source codes.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head {
	struct list_head *next, *prev;
};

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  ((void *)0x00100100)
#define LIST_POISON2  ((void *)0x00200200)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name)						\
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(node)					\
	do {							\
		(node)->next = node;				\
		(node)->prev = node;				\
	} while (0)

/**
 * list_add - add a new entry
 * @entry: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
#define list_add(entry, head)					\
	do {							\
		(head)->next->prev = entry;			\
		(entry)->next = (head)->next;			\
		(entry)->prev = head; (head)->next = entry;	\
	} while (0)

/**
 * list_add_tail - add a new entry
 * @entry: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
#define list_add_tail(entry, head)				\
	do {							\
		(head)->prev->next = entry;			\
		(entry)->prev = (head)->prev;			\
		(head)->prev = entry; (entry)->next = head;	\
	} while (0)

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define __list_del(node, entry)					\
	do {							\
		(entry)->prev = node; (node)->next = entry;	\
	} while (0)

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this,
 * the entry is in an undefined state.
 */
#define list_del(entry)						\
	do {							\
		(entry)->next->prev = (entry)->prev;		\
		(entry)->prev->next = (entry)->next;		\
		(entry)->next = LIST_POISON1;			\
		(entry)->prev = LIST_POISON2;			\
	} while (0)

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
#define list_del_init(entry)					\
	do {							\
		(entry)->next->prev = (entry)->prev;		\
		(entry)->prev->next = (entry)->next;		\
		INIT_LIST_HEAD(entry);				\
	} while (0)

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
#define list_move(list, head)					\
	do {							\
		struct list_head * tmp = list;			\
		__list_del((list)->prev, (list)->next);		\
		list_add(tmp, head);				\
	} while (0)

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
#define list_move_tail(list, head)				\
	do {							\
		struct list_head * tmp = list;			\
		__list_del((list)->prev, (list)->next);		\
		list_add_tail(tmp, head);			\
	} while (0)

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
#define list_empty(head)					\
	((head)->next == (head))

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
#define list_splice(list, head)					\
	do {							\
		if (!list_empty(list)) {			\
			struct list_head *first = (list)->next;	\
			struct list_head *last = (list)->prev;	\
			struct list_head *at = (head)->next;	\
			first->prev = head;			\
			(head)->next = first;			\
			last->next = at; at->prev = last;	\
		}						\
	} while (0)

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
#define list_splice_init(list, head)				\
	do {							\
		if (!list_empty(list)) {			\
			struct list_head *first = (list)->next;	\
			struct list_head *last = (list)->prev;	\
			struct list_head *at = (head)->next;	\
			first->prev = head;			\
			(head)->next = first;			\
			last->next = at; at->prev = last;	\
			INIT_LIST_HEAD(list);			\
		}						\
	} while (0)

/**
 * list_entry - get the struct for this entry
 * @node: the &list pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define list_entry(node, type, member)				\
	((type *)((caddr_t)(node) - (size_t)&((type *)0)->member))

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member)			\
	list_entry((ptr)->next, type, member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head)				\
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head)				\
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head)			\
	for (pos = (head)->next, n = pos->next; pos != (head);	\
		pos = n, n = pos->next)

#ifdef NDEBUG
/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(type, pos, head, member)		\
	for (pos = list_entry((head)->next, type, member);	\
	     &pos->member != (head); 				\
	     pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(type, pos, n, head, member)	\
	for (pos = list_entry((head)->next, type, member),	\
	     n = list_entry(pos->member.next, type, member);	\
	     &pos->member != (head); 				\
	     pos = n, n = list_entry(n->member.next, type, member))
#else
/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(type, pos, head, member)		\
	for (pos = ((head) == (head)->next ? NULL :		\
	     list_entry((head)->next, type, member));		\
	     pos && &pos->member != (head); 			\
	     pos = ((head) == pos->member.next ? NULL :		\
	     list_entry(pos->member.next, type, member)))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(type, pos, n, head, member)	\
	for (pos = ((head) == (head)->next ? NULL :		\
	     list_entry((head)->next, type, member)),		\
	     n = ((!pos || (head) == pos->member.next) ? NULL :	\
	     list_entry(pos->member.next, type, member));	\
	     pos && &pos->member != (head); 			\
	     pos = n,						\
	     n = ((!n || (head) == n->member.next) ? NULL :	\
	     list_entry(n->member.next, type, member)))
#endif

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#define HLIST_HEAD_INIT { NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

#define INIT_HLIST_NODE(h)		\
	do {				\
		(h)->next = NULL;	\
		(h)->pprev = NULL;	\
	} while (0)

#define hlist_unhashed(h)	(!(h)->pprev)
#define hlist_empty(h)		((h)->first)

#define __hlist_del(n)						\
	do {							\
		struct hlist_node *next = (n)->next;		\
		struct hlist_node **pprev = (n)->pprev;		\
		*pprev = next;					\
		if (next)					\
			next->pprev = pprev;			\
	} while (0)

#define hlist_del(n)						\
	do {							\
		__hlist_del(n);					\
		(n)->next = LIST_POISON1;			\
		(n)->pprev = LIST_POISON2;			\
	} while (0)

#define hlist_del_init(n)					\
	do {							\
		if (!hlist_unhashed(n)) {			\
			__hlist_del(n);				\
			INIT_HLIST_NODE(n);			\
		}						\
	} while (0)

#define hlist_add_head(n, h)					\
	do {							\
		struct hlist_node *first = (h)->first;		\
		(n)->next = first;				\
		if (first)					\
			first->pprev = &(n)->next;		\
		(h)->first = (n);				\
		(n)->pprev = &(h)->first;			\
	} while (0)

/* next must be != NULL */
#define hlist_add_before(n, next)				\
	do {							\
		(n)->pprev = (next)->pprev;			\
		(n)->next = (next);				\
		(next)->pprev = &(n)->next;			\
		*((n)->pprev) = (n);				\
	} while (0)

#define hlist_add_after(n, next)				\
	do {							\
		(next)->next = (n)->next;			\
		(n)->next = (next);				\
		(next)->pprev = &(n)->next;			\
		if ((next)->next)				\
			(next)->next->pprev  = &(next)->next;	\
	} while (0)

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_for_each(pos, head)				\
	for (pos = (head)->first; pos;				\
	     pos = pos->next)

#define hlist_for_each_safe(pos, n, head)			\
	for (pos = (head)->first; pos && (n = pos->next, 1);	\
	     pos = n)

/**
 * hlist_for_each_entry	- iterate over list of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(type, tpos, pos, head, member)	\
	for (pos = (head)->first;				\
	     pos &&						\
	     (tpos = hlist_entry(pos, type, member), 1);	\
	     pos = pos->next)

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(type, tpos, pos, member)	\
	for (pos = (pos)->next;					\
	     pos &&						\
	     (tpos = hlist_entry(pos, type, member), 1);	\
	     pos = pos->next)

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(type, tpos, pos, member)	\
	for (; pos &&						\
	     (tpos = hlist_entry(pos, type, member), 1);	\
	     pos = pos->next)

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(type, tpos, pos, n, head, member)	\
	for (pos = (head)->first;					\
	     pos && (n = pos->next, 1) &&				\
	     (tpos = hlist_entry(pos, type, member), 1);		\
	     pos = n)

#endif /* __LIST_H_INCLUDE__ */
