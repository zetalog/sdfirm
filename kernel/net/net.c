#include <target/net.h>
#include <target/heap.h>

void skb_over_panic(struct sk_buff *skb, int sz, char *here)
{
#if 0
	 "SKB: over panic, "
	 "text=%s, len=%d, put=%d, head=%p, "
	 "data=%p, tail=%#lx, end=%#lx, dev=%s",
	 here, skb->len, sz, skb->head, skb->data,
	 skb->tail,
	 skb->end,
	 skb->dev ? skb->dev->ifname : "<NULL>"
#endif
	BUG();
}

void skb_under_panic(struct sk_buff *skb, int sz, char *here)
{
#if 0
	 "SKB: under panic, "
	 "text=%s, len=%d, put=%d, head=%p, "
	 "data=%p, tail=%#lx, end=%#lx, dev=%s",
	 here, skb->len, sz, skb->head, skb->data,
	 skb->tail,
	 skb->end,
	 skb->dev ? skb->dev->ifname : "<NULL>"
#endif
	BUG();
}

void skb_truesize_bug(struct sk_buff *skb)
{
#if 0
	 "SKB: invalid truesize, size=%u, "
	 "len=%u, sizeof(sk_buff)=%Zd",
	 skb->truesize, skb->len, sizeof(struct sk_buff)
#endif
}

struct sk_buff *alloc_skb(sk_size_t size)
{
	struct skb_shared_info *shinfo;
	struct sk_buff *skb;
	sk_size_t skb_len = sizeof (struct sk_buff);
	sk_data_t data;

	/* get the HEAD */
	skb = (struct sk_buff *)heap_alloc((heap_size_t)skb_len);
	if (!skb)
		goto out;

	data = malloc(size + sizeof (struct skb_shared_info));
	if (!data)
		goto nodata;

	/* see comment in sk_buff definition, just before the 'tail' member */
	memory_set((caddr_t)skb, 0,
		   (heap_size_t)(offsetof(struct sk_buff, sk_tail)));
	skb->truesize = size + sizeof(struct sk_buff);
	atomic_set(&skb->users, 1);
	skb->sk_head = data;
	skb->sk_data = data;
	skb_reset_tail_pointer(skb);
	skb->sk_end = skb->sk_tail + size;
	/* make sure we initialize shinfo sequentially */
	shinfo = skb_shinfo(skb);
	atomic_set(&shinfo->dataref, 1);
out:
	return skb;
nodata:
	free(skb);
	return NULL;
}

static void skb_release_data(struct sk_buff *skb)
{
	if (!skb->cloned ||
	    !atomic_sub_return(skb->nohdr ? (1 << SKB_DATAREF_SHIFT) + 1 : 1,
			       &skb_shinfo(skb)->dataref)) {
	}
}

void free_skb(struct sk_buff *skb)
{
	if (!skb)
		return;
	if (atomic_read(&skb->users) == 1)
		;
	else if (!atomic_dec_and_test(&skb->users))
		return;
	/* dst_release(skb->dst); */
	if (skb->destructor) {
		skb->destructor(skb);
	}

	skb_release_data(skb);
	heap_free((caddr_t)skb);
}

struct sk_buff *skb_clone(struct sk_buff *skb)
{
	struct sk_buff *n;
	int skb_len = sizeof(struct sk_buff);

	n = skb + 1;
	n = (struct sk_buff *)heap_alloc((heap_size_t)skb_len);
	if (!n)
		return NULL;

#define C(x) n->x = skb->x

	n->next = n->prev = NULL;
	n->sk = NULL;
	/* C(tstamp); */
	C(dev);
	/* C(dst); */
	/* dst_clone(skb->dst); */
	C(len);
	C(data_len);
	C(local_df);
	n->cloned = 1;
	n->hdr_len = (uint16_t)(skb->nohdr ? skb_headroom(skb) : skb->hdr_len);
	n->nohdr = 0;
	n->destructor = NULL;
	C(truesize);
	atomic_set(&n->users, 1);
	C(sk_head);
	C(sk_data);
	C(sk_tail);
	C(sk_end);

	atomic_inc(&(skb_shinfo(skb)->dataref));
	skb->cloned = 1;

	return n;
}

struct sk_buff *skb_peek_tail(struct sk_buff_head *list)
{
	struct sk_buff *res = ((struct sk_buff *)list)->prev;
	if (res == (struct sk_buff *)list)
		res = NULL;
	return res;
}

struct sk_buff *skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *next, *prev, *result;

	prev = (struct sk_buff *) list;
	next = prev->next;
	result = NULL;
	if (next != prev) {
		result	     = next;
		next	     = next->next;
		list->qlen--;
		next->prev   = prev;
		prev->next   = next;
		result->next = result->prev = NULL;
	}
	return result;
}

struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek_tail(list);
	if (skb)
		skb_unlink(skb, list);
	return skb;
}

void skb_queue_purge(struct sk_buff_head *list)
{
	struct sk_buff *skb;
	while ((skb = skb_dequeue(list)) != NULL)
		free_skb(skb);
}

void skb_queue_after(struct sk_buff_head *list,
		     struct sk_buff *prev,
		     struct sk_buff *newsk)
{
	struct sk_buff *next;
	list->qlen++;

	next = prev->next;
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
}

void skb_queue_head(struct sk_buff_head *list,
		    struct sk_buff *newsk)
{
	skb_queue_after(list, (struct sk_buff *)list, newsk);
}

void skb_queue_tail(struct sk_buff_head *list,
		    struct sk_buff *newsk)
{
	struct sk_buff *prev, *next;

	list->qlen++;
	next = (struct sk_buff *)list;
	prev = next->prev;
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
}

void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	next->prev = prev;
	prev->next = next;
}

void skb_insert(struct sk_buff *newsk,
		struct sk_buff *prev, struct sk_buff *next,
		struct sk_buff_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}

void skb_append(struct sk_buff *old, struct sk_buff *newsk,
		struct sk_buff_head *list)
{
	skb_insert(newsk, old, old->next, list);
}

void net_init(void)
{
}
