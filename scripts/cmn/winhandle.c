#include <assert.h>
#include <windows.h>
#include <host/winhandle.h>

/* grapped from PuTTY for win32 serial console support */

#define snew(type)		\
	((type *) malloc(sizeof(type)))
#define snewn(n, type)		\
	((type *) malloc((n) * sizeof(type)))
#define sresize(x, n, type)	\
	((type *) realloc((x), (n) * sizeof(type)))
#define sfree(x)		\
	(free((x)))

struct bufchain_granule;
typedef struct bufchain_tag {
	struct bufchain_granule *head, *tail;
	/* current amount of buffered data */
	int buffersize;
} bufchain;

#define BUFFER_GRANULE  512

struct bufchain_granule {
	struct bufchain_granule *next;
	int buflen, bufpos;
	char buf[BUFFER_GRANULE];
};

void bufchain_init(bufchain *ch)
{
	ch->head = ch->tail = NULL;
	ch->buffersize = 0;
}

void bufchain_clear(bufchain *ch)
{
	struct bufchain_granule *b;
	while (ch->head) {
		b = ch->head;
		ch->head = ch->head->next;
		sfree(b);
	}
	ch->tail = NULL;
	ch->buffersize = 0;
}

int bufchain_size(bufchain *ch)
{
	return ch->buffersize;
}

void bufchain_add(bufchain *ch, const void *data, int len)
{
	const char *buf = (const char *)data;
	
	if (len == 0) return;
	
	ch->buffersize += len;
	
	if (ch->tail && ch->tail->buflen < BUFFER_GRANULE) {
		int copylen = min(len, BUFFER_GRANULE - ch->tail->buflen);
		memcpy(ch->tail->buf + ch->tail->buflen, buf, copylen);
		buf += copylen;
		len -= copylen;
		ch->tail->buflen += copylen;
	}
	while (len > 0) {
		int grainlen = min(len, BUFFER_GRANULE);
		struct bufchain_granule *newbuf;
		newbuf = snew(struct bufchain_granule);
		newbuf->bufpos = 0;
		newbuf->buflen = grainlen;
		memcpy(newbuf->buf, buf, grainlen);
		buf += grainlen;
		len -= grainlen;
		if (ch->tail)
			ch->tail->next = newbuf;
		else
			ch->head = ch->tail = newbuf;
		newbuf->next = NULL;
		ch->tail = newbuf;
	}
}

void bufchain_consume(bufchain *ch, int len)
{
	struct bufchain_granule *tmp;
	
	assert(ch->buffersize >= len);
	while (len > 0) {
		int remlen = len;
		assert(ch->head != NULL);
		if (remlen >= ch->head->buflen - ch->head->bufpos) {
			remlen = ch->head->buflen - ch->head->bufpos;
			tmp = ch->head;
			ch->head = tmp->next;
			sfree(tmp);
			if (!ch->head)
				ch->tail = NULL;
		} else
			ch->head->bufpos += remlen;
		ch->buffersize -= remlen;
		len -= remlen;
	}
}

void bufchain_prefix(bufchain *ch, void **data, int *len)
{
	*len = ch->head->buflen - ch->head->bufpos;
	*data = ch->head->buf + ch->head->bufpos;
}

void bufchain_fetch(bufchain *ch, void *data, int len)
{
	struct bufchain_granule *tmp;
	char *data_c = (char *)data;
	
	tmp = ch->head;
	
	assert(ch->buffersize >= len);
	while (len > 0) {
		int remlen = len;
		
		assert(tmp != NULL);
		if (remlen >= tmp->buflen - tmp->bufpos)
			remlen = tmp->buflen - tmp->bufpos;
		memcpy(data_c, tmp->buf + tmp->bufpos, remlen);
		
		tmp = tmp->next;
		len -= remlen;
		data_c += remlen;
	}
}

/*
 * Find an element e in a sorted 2-3-4 tree t. Returns NULL if not
 * found. e is always passed as the first argument to cmp, so cmp
 * can be an asymmetric function if desired. cmp can also be passed
 * as NULL, in which case the compare function from the tree proper
 * will be used.
 * 
 * Three of these functions are special cases of findrelpos234. The
 * non-`pos' variants lack the `index' parameter: if the parameter
 * is present and non-NULL, it must point to an integer variable
 * which will be filled with the numeric index of the returned
 * element.
 * 
 * The non-`rel' variants lack the `relation' parameter. This
 * parameter allows you to specify what relation the element you
 * provide has to the element you're looking for. This parameter
 * can be:
 * 
 *   REL234_EQ     - find only an element that compares equal to e
 *   REL234_LT     - find the greatest element that compares < e
 *   REL234_LE     - find the greatest element that compares <= e
 *   REL234_GT     - find the smallest element that compares > e
 *   REL234_GE     - find the smallest element that compares >= e
 * 
 * Non-`rel' variants assume REL234_EQ.
 * 
 * If `rel' is REL234_GT or REL234_LT, the `e' parameter may be
 * NULL. In this case, REL234_GT will return the smallest element
 * in the tree, and REL234_LT will return the greatest. This gives
 * an alternative means of iterating over a sorted tree, instead of
 * using index234:
 * 
 *   // to loop forwards
 *   for (p = NULL; (p = findrel234(tree, p, NULL, REL234_GT)) != NULL ;)
 *       consume(p);
 * 
 *   // to loop backwards
 *   for (p = NULL; (p = findrel234(tree, p, NULL, REL234_LT)) != NULL ;)
 *       consume(p);
 */
enum {
	REL234_EQ, REL234_LT, REL234_LE, REL234_GT, REL234_GE
};

#ifdef TEST
#define LOG(x) (printf x)
#else
#define LOG(x)
#endif

typedef struct node234_Tag node234;
typedef struct tree234_Tag tree234;

typedef int (*cmpfn234) (void *, void *);

struct tree234_Tag {
	node234 *root;
	cmpfn234 cmp;
};

struct node234_Tag {
	node234 *parent;
	node234 *kids[4];
	int counts[4];
	void *elems[3];
};

/*
 * Create a 2-3-4 tree.
 */
tree234 *newtree234(cmpfn234 cmp)
{
	tree234 *ret = snew(tree234);
	LOG(("created tree %p\n", ret));
	ret->root = NULL;
	ret->cmp = cmp;
	return ret;
}

/*
 * Free a 2-3-4 tree (not including freeing the elements).
 */
static void freenode234(node234 * n)
{
	if (!n)
		return;
	freenode234(n->kids[0]);
	freenode234(n->kids[1]);
	freenode234(n->kids[2]);
	freenode234(n->kids[3]);
	sfree(n);
}

void freetree234(tree234 * t)
{
	freenode234(t->root);
	sfree(t);
}

/*
 * Internal function to count a node.
 */
static int countnode234(node234 * n)
{
	int count = 0;
	int i;
	if (!n)
		return 0;
	for (i = 0; i < 4; i++)
		count += n->counts[i];
	for (i = 0; i < 3; i++) {
		if (n->elems[i])
			count++;
	}
	return count;
}

/*
 * Count the elements in a tree.
 */
int count234(tree234 * t)
{
	if (t->root)
		return countnode234(t->root);
	else
		return 0;
}

/*
 * Add an element e to a 2-3-4 tree t. Returns e on success, or if
 * an existing element compares equal, returns that.
 */
static void *add234_internal(tree234 * t, void *e, int index)
{
	node234 *n, **np, *left, *right;
	void *orig_e = e;
	int c, lcount, rcount;
	
	LOG(("adding node %p to tree %p\n", e, t));
	if (t->root == NULL) {
		t->root = snew(node234);
		t->root->elems[1] = t->root->elems[2] = NULL;
		t->root->kids[0] = t->root->kids[1] = NULL;
		t->root->kids[2] = t->root->kids[3] = NULL;
		t->root->counts[0] = t->root->counts[1] = 0;
		t->root->counts[2] = t->root->counts[3] = 0;
		t->root->parent = NULL;
		t->root->elems[0] = e;
		LOG(("  created root %p\n", t->root));
		return orig_e;
	}
	
	n = NULL; /* placate gcc; will always be set below since t->root != NULL */
	np = &t->root;
	while (*np) {
		int childnum;
		n = *np;
		LOG(("  node %p: %p/%d [%p] %p/%d [%p] %p/%d [%p] %p/%d\n",
		    n,
		    n->kids[0], n->counts[0], n->elems[0],
		    n->kids[1], n->counts[1], n->elems[1],
		    n->kids[2], n->counts[2], n->elems[2],
		    n->kids[3], n->counts[3]));
		if (index >= 0) {
			if (!n->kids[0]) {
				/*
				 * Leaf node. We want to insert at kid
				 * position equal to the index:
				 * 
				 *   0 A 1 B 2 C 3
				 */
				childnum = index;
			} else {
				/*
				* Internal node. We always descend through
				* it (add always starts at the bottom,
				* never in the middle).
				*/
				do {
					/* this is a do ... while (0) to
					 * allow `break'
					 */
					if (index <= n->counts[0]) {
						childnum = 0;
						break;
					}
					index -= n->counts[0] + 1;
					if (index <= n->counts[1]) {
						childnum = 1;
						break;
					}
					index -= n->counts[1] + 1;
					if (index <= n->counts[2]) {
						childnum = 2;
						break;
					}
					index -= n->counts[2] + 1;
					if (index <= n->counts[3]) {
						childnum = 3;
						break;
					}
					return NULL;       /* error: index out of range */
				} while (0);
			}
		} else {
			if ((c = t->cmp(e, n->elems[0])) < 0)
				childnum = 0;
			else if (c == 0)
				return n->elems[0];    /* already exists */
			else if (n->elems[1] == NULL
				|| (c = t->cmp(e, n->elems[1])) < 0) childnum = 1;
			else if (c == 0)
				return n->elems[1];    /* already exists */
			else if (n->elems[2] == NULL
				|| (c = t->cmp(e, n->elems[2])) < 0) childnum = 2;
			else if (c == 0)
				return n->elems[2];    /* already exists */
			else
				childnum = 3;
		}
		np = &n->kids[childnum];
		LOG(("  moving to child %d (%p)\n", childnum, *np));
	}
	
	/*
	 * We need to insert the new element in n at position np.
	 */
	left = NULL;
	lcount = 0;
	right = NULL;
	rcount = 0;
	while (n) {
		LOG(("  at %p: %p/%d [%p] %p/%d [%p] %p/%d [%p] %p/%d\n",
		    n,
		    n->kids[0], n->counts[0], n->elems[0],
		    n->kids[1], n->counts[1], n->elems[1],
		    n->kids[2], n->counts[2], n->elems[2],
		    n->kids[3], n->counts[3]));
		LOG(("  need to insert %p/%d [%p] %p/%d at position %d\n",
		    left, lcount, e, right, rcount, np - n->kids));
		if (n->elems[1] == NULL) {
			/*
			 * Insert in a 2-node; simple.
			 */
			if (np == &n->kids[0]) {
				LOG(("  inserting on left of 2-node\n"));
				n->kids[2] = n->kids[1];
				n->counts[2] = n->counts[1];
				n->elems[1] = n->elems[0];
				n->kids[1] = right;
				n->counts[1] = rcount;
				n->elems[0] = e;
				n->kids[0] = left;
				n->counts[0] = lcount;
			} else {
				/* np == &n->kids[1] */
				LOG(("  inserting on right of 2-node\n"));
				n->kids[2] = right;
				n->counts[2] = rcount;
				n->elems[1] = e;
				n->kids[1] = left;
				n->counts[1] = lcount;
			}
			if (n->kids[0])
				n->kids[0]->parent = n;
			if (n->kids[1])
				n->kids[1]->parent = n;
			if (n->kids[2])
				n->kids[2]->parent = n;
			LOG(("  done\n"));
			break;
		} else if (n->elems[2] == NULL) {
			/*
			 * Insert in a 3-node; simple.
			 */
			if (np == &n->kids[0]) {
				LOG(("  inserting on left of 3-node\n"));
				n->kids[3] = n->kids[2];
				n->counts[3] = n->counts[2];
				n->elems[2] = n->elems[1];
				n->kids[2] = n->kids[1];
				n->counts[2] = n->counts[1];
				n->elems[1] = n->elems[0];
				n->kids[1] = right;
				n->counts[1] = rcount;
				n->elems[0] = e;
				n->kids[0] = left;
				n->counts[0] = lcount;
			} else if (np == &n->kids[1]) {
				LOG(("  inserting in middle of 3-node\n"));
				n->kids[3] = n->kids[2];
				n->counts[3] = n->counts[2];
				n->elems[2] = n->elems[1];
				n->kids[2] = right;
				n->counts[2] = rcount;
				n->elems[1] = e;
				n->kids[1] = left;
				n->counts[1] = lcount;
			} else {
				/* np == &n->kids[2] */
				LOG(("  inserting on right of 3-node\n"));
				n->kids[3] = right;
				n->counts[3] = rcount;
				n->elems[2] = e;
				n->kids[2] = left;
				n->counts[2] = lcount;
			}
			if (n->kids[0])
				n->kids[0]->parent = n;
			if (n->kids[1])
				n->kids[1]->parent = n;
			if (n->kids[2])
				n->kids[2]->parent = n;
			if (n->kids[3])
				n->kids[3]->parent = n;
			LOG(("  done\n"));
			break;
		} else {
			node234 *m = snew(node234);
			m->parent = n->parent;
			LOG(("  splitting a 4-node; created new node %p\n", m));
			/*
			 * Insert in a 4-node; split into a 2-node and a
			 * 3-node, and move focus up a level.
			 * 
			 * I don't think it matters which way round we
			 * put the 2 and the 3. For simplicity, we'll put
			 * the 3 first always.
			 */
			if (np == &n->kids[0]) {
				m->kids[0] = left;
				m->counts[0] = lcount;
				m->elems[0] = e;
				m->kids[1] = right;
				m->counts[1] = rcount;
				m->elems[1] = n->elems[0];
				m->kids[2] = n->kids[1];
				m->counts[2] = n->counts[1];
				e = n->elems[1];
				n->kids[0] = n->kids[2];
				n->counts[0] = n->counts[2];
				n->elems[0] = n->elems[2];
				n->kids[1] = n->kids[3];
				n->counts[1] = n->counts[3];
			} else if (np == &n->kids[1]) {
				m->kids[0] = n->kids[0];
				m->counts[0] = n->counts[0];
				m->elems[0] = n->elems[0];
				m->kids[1] = left;
				m->counts[1] = lcount;
				m->elems[1] = e;
				m->kids[2] = right;
				m->counts[2] = rcount;
				e = n->elems[1];
				n->kids[0] = n->kids[2];
				n->counts[0] = n->counts[2];
				n->elems[0] = n->elems[2];
				n->kids[1] = n->kids[3];
				n->counts[1] = n->counts[3];
			} else if (np == &n->kids[2]) {
				m->kids[0] = n->kids[0];
				m->counts[0] = n->counts[0];
				m->elems[0] = n->elems[0];
				m->kids[1] = n->kids[1];
				m->counts[1] = n->counts[1];
				m->elems[1] = n->elems[1];
				m->kids[2] = left;
				m->counts[2] = lcount;
				/* e = e; */
				n->kids[0] = right;
				n->counts[0] = rcount;
				n->elems[0] = n->elems[2];
				n->kids[1] = n->kids[3];
				n->counts[1] = n->counts[3];
			} else {
				/* np == &n->kids[3] */
				m->kids[0] = n->kids[0];
				m->counts[0] = n->counts[0];
				m->elems[0] = n->elems[0];
				m->kids[1] = n->kids[1];
				m->counts[1] = n->counts[1];
				m->elems[1] = n->elems[1];
				m->kids[2] = n->kids[2];
				m->counts[2] = n->counts[2];
				n->kids[0] = left;
				n->counts[0] = lcount;
				n->elems[0] = e;
				n->kids[1] = right;
				n->counts[1] = rcount;
				e = n->elems[2];
			}
			m->kids[3] = n->kids[3] = n->kids[2] = NULL;
			m->counts[3] = n->counts[3] = n->counts[2] = 0;
			m->elems[2] = n->elems[2] = n->elems[1] = NULL;
			if (m->kids[0])
				m->kids[0]->parent = m;
			if (m->kids[1])
				m->kids[1]->parent = m;
			if (m->kids[2])
				m->kids[2]->parent = m;
			if (n->kids[0])
				n->kids[0]->parent = n;
			if (n->kids[1])
				n->kids[1]->parent = n;
			LOG(("  left (%p): %p/%d [%p] %p/%d [%p] %p/%d\n", m,
			    m->kids[0], m->counts[0], m->elems[0],
			    m->kids[1], m->counts[1], m->elems[1],
			    m->kids[2], m->counts[2]));
			LOG(("  right (%p): %p/%d [%p] %p/%d\n", n,
			    n->kids[0], n->counts[0], n->elems[0],
			    n->kids[1], n->counts[1]));
			left = m;
			lcount = countnode234(left);
			right = n;
			rcount = countnode234(right);
		}
		if (n->parent)
			np = (n->parent->kids[0] == n ? &n->parent->kids[0] :
		n->parent->kids[1] == n ? &n->parent->kids[1] :
		n->parent->kids[2] == n ? &n->parent->kids[2] :
		&n->parent->kids[3]);
		n = n->parent;
    }
    
    /*
     * If we've come out of here by `break', n will still be
     * non-NULL and all we need to do is go back up the tree
     * updating counts. If we've come here because n is NULL, we
     * need to create a new root for the tree because the old one
     * has just split into two.
     */
    if (n) {
	    while (n->parent) {
		    int count = countnode234(n);
		    int childnum;
		    childnum = (n->parent->kids[0] == n ? 0 :
		    n->parent->kids[1] == n ? 1 :
		    n->parent->kids[2] == n ? 2 : 3);
		    n->parent->counts[childnum] = count;
		    n = n->parent;
	    }
    } else {
	    LOG(("  root is overloaded, split into two\n"));
	    t->root = snew(node234);
	    t->root->kids[0] = left;
	    t->root->counts[0] = lcount;
	    t->root->elems[0] = e;
	    t->root->kids[1] = right;
	    t->root->counts[1] = rcount;
	    t->root->elems[1] = NULL;
	    t->root->kids[2] = NULL;
	    t->root->counts[2] = 0;
	    t->root->elems[2] = NULL;
	    t->root->kids[3] = NULL;
	    t->root->counts[3] = 0;
	    t->root->parent = NULL;
	    if (t->root->kids[0])
		    t->root->kids[0]->parent = t->root;
	    if (t->root->kids[1])
		    t->root->kids[1]->parent = t->root;
		LOG(("  new root is %p/%d [%p] %p/%d\n",
		    t->root->kids[0], t->root->counts[0],
		    t->root->elems[0], t->root->kids[1], t->root->counts[1]));
	}
    
	return orig_e;
}

void *add234(tree234 * t, void *e)
{
	if (!t->cmp)		       /* tree is unsorted */
		return NULL;
	
	return add234_internal(t, e, -1);
}

void *addpos234(tree234 * t, void *e, int index)
{
	if (index < 0 ||		       /* index out of range */
	    t->cmp)			       /* tree is sorted */
		return NULL;		       /* return failure */
	
	return add234_internal(t, e, index);	/* this checks the upper bound */
}

/*
 * Look up the element at a given numeric index in a 2-3-4 tree.
 * Returns NULL if the index is out of range.
 */
void *index234(tree234 * t, int index)
{
	node234 *n;
	
	if (!t->root)
		return NULL;		       /* tree is empty */
	
	if (index < 0 || index >= countnode234(t->root))
		return NULL;		       /* out of range */
	
	n = t->root;
	
	while (n) {
		if (index < n->counts[0])
			n = n->kids[0];
		else if (index -= n->counts[0] + 1, index < 0)
			return n->elems[0];
		else if (index < n->counts[1])
			n = n->kids[1];
		else if (index -= n->counts[1] + 1, index < 0)
			return n->elems[1];
		else if (index < n->counts[2])
			n = n->kids[2];
		else if (index -= n->counts[2] + 1, index < 0)
			return n->elems[2];
		else
			n = n->kids[3];
	}
	
	/* We shouldn't ever get here. I wonder how we did. */
	return NULL;
}

/*
 * Find an element e in a sorted 2-3-4 tree t. Returns NULL if not
 * found. e is always passed as the first argument to cmp, so cmp
 * can be an asymmetric function if desired. cmp can also be passed
 * as NULL, in which case the compare function from the tree proper
 * will be used.
 */
void *findrelpos234(tree234 * t, void *e, cmpfn234 cmp,
		    int relation, int *index)
{
	node234 *n;
	void *ret;
	int c;
	int idx, ecount, kcount, cmpret;
	
	if (t->root == NULL)
		return NULL;
	
	if (cmp == NULL)
		cmp = t->cmp;
	
	n = t->root;
	/*
	 * Attempt to find the element itself.
	 */
	idx = 0;
	ecount = -1;
	/*
	 * Prepare a fake `cmp' result if e is NULL.
	 */
	cmpret = 0;
	if (e == NULL) {
		assert(relation == REL234_LT || relation == REL234_GT);
		if (relation == REL234_LT)
			cmpret = +1;	       /* e is a max: always greater */
		else if (relation == REL234_GT)
			cmpret = -1;	       /* e is a min: always smaller */
	}
	while (1) {
		for (kcount = 0; kcount < 4; kcount++) {
			if (kcount >= 3 || n->elems[kcount] == NULL ||
				(c = cmpret ? cmpret : cmp(e, n->elems[kcount])) < 0) {
				break;
			}
			if (n->kids[kcount])
				idx += n->counts[kcount];
			if (c == 0) {
				ecount = kcount;
				break;
			}
			idx++;
		}
		if (ecount >= 0)
			break;
		if (n->kids[kcount])
			n = n->kids[kcount];
		else
			break;
	}
	
	if (ecount >= 0) {
		/*
		 * We have found the element we're looking for. It's
		 * n->elems[ecount], at tree index idx. If our search
		 * relation is EQ, LE or GE we can now go home.
		 */
		if (relation != REL234_LT && relation != REL234_GT) {
			if (index)
				*index = idx;
			return n->elems[ecount];
		}
		
		/*
		 * Otherwise, we'll do an indexed lookup for the previous
		 * or next element. (It would be perfectly possible to
		 * implement these search types in a non-counted tree by
		 * going back up from where we are, but far more fiddly.)
		 */
		if (relation == REL234_LT)
			idx--;
		else
			idx++;
	} else {
		/*
		 * We've found our way to the bottom of the tree and we
		 * know where we would insert this node if we wanted to:
		 * we'd put it in in place of the (empty) subtree
		 * n->kids[kcount], and it would have index idx
		 * 
		 * But the actual element isn't there. So if our search
		 * relation is EQ, we're doomed.
		 */
		if (relation == REL234_EQ)
			return NULL;
		
		/*
		 * Otherwise, we must do an index lookup for index idx-1
		 * (if we're going left - LE or LT) or index idx (if we're
		 * going right - GE or GT).
		 */
		if (relation == REL234_LT || relation == REL234_LE) {
			idx--;
		}
	}
	
	/*
	 * We know the index of the element we want; just call index234
	 * to do the rest. This will return NULL if the index is out of
	 * bounds, which is exactly what we want.
	 */
	ret = index234(t, idx);
	if (ret && index)
		*index = idx;
	return ret;
}

void *find234(tree234 * t, void *e, cmpfn234 cmp)
{
	return findrelpos234(t, e, cmp, REL234_EQ, NULL);
}

void *findrel234(tree234 * t, void *e, cmpfn234 cmp, int relation)
{
	return findrelpos234(t, e, cmp, relation, NULL);
}

void *findpos234(tree234 * t, void *e, cmpfn234 cmp, int *index)
{
	return findrelpos234(t, e, cmp, REL234_EQ, index);
}

/*
 * Delete an element e in a 2-3-4 tree. Does not free the element,
 * merely removes all links to it from the tree nodes.
 */
static void *delpos234_internal(tree234 * t, int index)
{
	node234 *n;
	void *retval;
	int ei = -1;
	
	retval = 0;
	
	n = t->root;
	LOG(("deleting item %d from tree %p\n", index, t));
	while (1) {
		while (n) {
			int ki;
			node234 *sub;
			
			LOG(("  node %p: %p/%d [%p] %p/%d [%p] %p/%d [%p] %p/%d index=%d\n",
			    n, n->kids[0], n->counts[0], n->elems[0], n->kids[1],
			    n->counts[1], n->elems[1], n->kids[2], n->counts[2],
			    n->elems[2], n->kids[3], n->counts[3], index));
			if (index < n->counts[0]) {
				ki = 0;
			} else if (index -= n->counts[0] + 1, index < 0) {
				ei = 0;
				break;
			} else if (index < n->counts[1]) {
				ki = 1;
			} else if (index -= n->counts[1] + 1, index < 0) {
				ei = 1;
				break;
			} else if (index < n->counts[2]) {
				ki = 2;
			} else if (index -= n->counts[2] + 1, index < 0) {
				ei = 2;
				break;
			} else {
				ki = 3;
			}
			/*
			 * Recurse down to subtree ki. If it has only one
			 * element, we have to do some transformation to
			 * start with.
			 */
			LOG(("  moving to subtree %d\n", ki));
			sub = n->kids[ki];
			if (!sub->elems[1]) {
				LOG(("  subtree has only one element!\n", ki));
				if (ki > 0 && n->kids[ki - 1]->elems[1]) {
					/*
					 * Case 3a, left-handed variant. Child ki has
					 * only one element, but child ki-1 has two or
					 * more. So we need to move a subtree from ki-1
					 * to ki.
					 * 
					 *                . C .                     . B .
					 *               /     \     ->            /     \
					 * [more] a A b B c   d D e      [more] a A b   c C d D e
					 */
					node234 *sib = n->kids[ki - 1];
					int lastelem = (sib->elems[2] ? 2 :
					sib->elems[1] ? 1 : 0);
					sub->kids[2] = sub->kids[1];
					sub->counts[2] = sub->counts[1];
					sub->elems[1] = sub->elems[0];
					sub->kids[1] = sub->kids[0];
					sub->counts[1] = sub->counts[0];
					sub->elems[0] = n->elems[ki - 1];
					sub->kids[0] = sib->kids[lastelem + 1];
					sub->counts[0] = sib->counts[lastelem + 1];
					if (sub->kids[0])
						sub->kids[0]->parent = sub;
					n->elems[ki - 1] = sib->elems[lastelem];
					sib->kids[lastelem + 1] = NULL;
					sib->counts[lastelem + 1] = 0;
					sib->elems[lastelem] = NULL;
					n->counts[ki] = countnode234(sub);
					LOG(("  case 3a left\n"));
					LOG(("  index and left subtree count before adjustment: %d, %d\n",
					    index, n->counts[ki - 1]));
					index += n->counts[ki - 1];
					n->counts[ki - 1] = countnode234(sib);
					index -= n->counts[ki - 1];
					LOG(("  index and left subtree count after adjustment: %d, %d\n",
					    index, n->counts[ki - 1]));
				} else if (ki < 3 && n->kids[ki + 1] &&
					   n->kids[ki + 1]->elems[1]) {
					/*
					 * Case 3a, right-handed variant. ki has only
					 * one element but ki+1 has two or more. Move a
					 * subtree from ki+1 to ki.
					 * 
					 *      . B .                             . C .
					 *     /     \                ->         /     \
					 *  a A b   c C d D e [more]      a A b B c   d D e [more]
					 */
					node234 *sib = n->kids[ki + 1];
					int j;
					sub->elems[1] = n->elems[ki];
					sub->kids[2] = sib->kids[0];
					sub->counts[2] = sib->counts[0];
					if (sub->kids[2])
						sub->kids[2]->parent = sub;
					n->elems[ki] = sib->elems[0];
					sib->kids[0] = sib->kids[1];
					sib->counts[0] = sib->counts[1];
					for (j = 0; j < 2 && sib->elems[j + 1]; j++) {
						sib->kids[j + 1] = sib->kids[j + 2];
						sib->counts[j + 1] = sib->counts[j + 2];
						sib->elems[j] = sib->elems[j + 1];
					}
					sib->kids[j + 1] = NULL;
					sib->counts[j + 1] = 0;
					sib->elems[j] = NULL;
					n->counts[ki] = countnode234(sub);
					n->counts[ki + 1] = countnode234(sib);
					LOG(("  case 3a right\n"));
				} else {
					/*
					 * Case 3b. ki has only one element, and has no
					 * neighbour with more than one. So pick a
					 * neighbour and merge it with ki, taking an
					 * element down from n to go in the middle.
					 *
					 *      . B .                .
					 *     /     \     ->        |
					 *  a A b   c C d      a A b B c C d
					 * 
					 * (Since at all points we have avoided
					 * descending to a node with only one element,
					 * we can be sure that n is not reduced to
					 * nothingness by this move, _unless_ it was
					 * the very first node, ie the root of the
					 * tree. In that case we remove the now-empty
					 * root and replace it with its single large
					 * child as shown.)
					 */
					node234 *sib;
					int j;
					
					if (ki > 0) {
						ki--;
						index += n->counts[ki] + 1;
					}
					sib = n->kids[ki];
					sub = n->kids[ki + 1];
					
					sub->kids[3] = sub->kids[1];
					sub->counts[3] = sub->counts[1];
					sub->elems[2] = sub->elems[0];
					sub->kids[2] = sub->kids[0];
					sub->counts[2] = sub->counts[0];
					sub->elems[1] = n->elems[ki];
					sub->kids[1] = sib->kids[1];
					sub->counts[1] = sib->counts[1];
					if (sub->kids[1])
						sub->kids[1]->parent = sub;
					sub->elems[0] = sib->elems[0];
					sub->kids[0] = sib->kids[0];
					sub->counts[0] = sib->counts[0];
					if (sub->kids[0])
						sub->kids[0]->parent = sub;
					
					n->counts[ki + 1] = countnode234(sub);
					
					sfree(sib);
					
					/*
					 * That's built the big node in
					 * sub. Now we need to remove the
					 * reference to sib in n.
					 */
					for (j = ki; j < 3 && n->kids[j + 1]; j++) {
						n->kids[j] = n->kids[j + 1];
						n->counts[j] = n->counts[j + 1];
						n->elems[j] = j < 2 ? n->elems[j + 1] : NULL;
					}
					n->kids[j] = NULL;
					n->counts[j] = 0;
					if (j < 3)
						n->elems[j] = NULL;
					LOG(("  case 3b ki=%d\n", ki));
					
					if (!n->elems[0]) {
						/*
						 * The root is empty and
						 * needs to be removed.
						 */
						LOG(("  shifting root!\n"));
						t->root = sub;
						sub->parent = NULL;
						sfree(n);
					}
				}
	    }
	    n = sub;
	}
	if (!retval)
		retval = n->elems[ei];
	
	if (ei == -1)
		return NULL;	       /* although this shouldn't happen */

       /*
        * Treat special case: this is the one remaining item in
        * the tree. n is the tree root (no parent), has one
        * element (no elems[1]), and has no kids (no kids[0]).
	*/
	if (!n->parent && !n->elems[1] && !n->kids[0]) {
		LOG(("  removed last element in tree\n"));
		sfree(n);
		t->root = NULL;
		return retval;
	}
	
	/*
	 * Now we have the element we want, as n->elems[ei], and we
	 * have also arranged for that element not to be the only
	 * one in its node. So...
	 */
	
	if (!n->kids[0] && n->elems[1]) {
		/*
		 * Case 1. n is a leaf node with more than one element,
		 * so it's _really easy_. Just delete the thing and
		 * we're done.
		 */
		int i;
		LOG(("  case 1\n"));
		for (i = ei; i < 2 && n->elems[i + 1]; i++)
			n->elems[i] = n->elems[i + 1];
		n->elems[i] = NULL;
		/*
		 * Having done that to the leaf node, we now go back up
		 * the tree fixing the counts.
		 */
		while (n->parent) {
			int childnum;
			childnum = (n->parent->kids[0] == n ? 0 :
			n->parent->kids[1] == n ? 1 :
			n->parent->kids[2] == n ? 2 : 3);
			n->parent->counts[childnum]--;
			n = n->parent;
		}
		return retval;	       /* finished! */
	} else if (n->kids[ei]->elems[1]) {
		/*
		 * Case 2a. n is an internal node, and the root of the
		 * subtree to the left of e has more than one element.
		 * So find the predecessor p to e (ie the largest node
		 * in that subtree), place it where e currently is, and
		 * then start the deletion process over again on the
		 * subtree with p as target.
		 */
		node234 *m = n->kids[ei];
		void *target;
		LOG(("  case 2a\n"));
		while (m->kids[0]) {
			m = (m->kids[3] ? m->kids[3] :
		m->kids[2] ? m->kids[2] :
		m->kids[1] ? m->kids[1] : m->kids[0]);
		}
		target = (m->elems[2] ? m->elems[2] :
		m->elems[1] ? m->elems[1] : m->elems[0]);
		n->elems[ei] = target;
		index = n->counts[ei] - 1;
		n = n->kids[ei];
	} else if (n->kids[ei + 1]->elems[1]) {
		/*
		 * Case 2b, symmetric to 2a but s/left/right/ and
		 * s/predecessor/successor/. (And s/largest/smallest/).
		 */
		node234 *m = n->kids[ei + 1];
		void *target;
		LOG(("  case 2b\n"));
		while (m->kids[0]) {
			m = m->kids[0];
		}
		target = m->elems[0];
		n->elems[ei] = target;
		n = n->kids[ei + 1];
		index = 0;
	} else {
		/*
		 * Case 2c. n is an internal node, and the subtrees to
		 * the left and right of e both have only one element.
		 * So combine the two subnodes into a single big node
		 * with their own elements on the left and right and e
		 * in the middle, then restart the deletion process on
		 * that subtree, with e still as target.
		 */
		node234 *a = n->kids[ei], *b = n->kids[ei + 1];
		int j;
		
		LOG(("  case 2c\n"));
		a->elems[1] = n->elems[ei];
		a->kids[2] = b->kids[0];
		a->counts[2] = b->counts[0];
		if (a->kids[2])
			a->kids[2]->parent = a;
		a->elems[2] = b->elems[0];
		a->kids[3] = b->kids[1];
		a->counts[3] = b->counts[1];
		if (a->kids[3])
			a->kids[3]->parent = a;
		sfree(b);
		n->counts[ei] = countnode234(a);
		/*
		 * That's built the big node in a, and destroyed b. Now
		 * remove the reference to b (and e) in n.
		 */
		for (j = ei; j < 2 && n->elems[j + 1]; j++) {
			n->elems[j] = n->elems[j + 1];
			n->kids[j + 1] = n->kids[j + 2];
			n->counts[j + 1] = n->counts[j + 2];
		}
		n->elems[j] = NULL;
		n->kids[j + 1] = NULL;
		n->counts[j + 1] = 0;
		/*
		 * It's possible, in this case, that we've just removed
		 * the only element in the root of the tree. If so,
		 * shift the root.
		 */
		if (n->elems[0] == NULL) {
			LOG(("  shifting root!\n"));
			t->root = a;
			a->parent = NULL;
			sfree(n);
		}
		/*
		 * Now go round the deletion process again, with n
		 * pointing at the new big node and e still the same.
		 */
		n = a;
		index = a->counts[0] + a->counts[1] + 1;
	}
    }
}

void *delpos234(tree234 * t, int index)
{
	if (index < 0 || index >= countnode234(t->root))
		return NULL;
	return delpos234_internal(t, index);
}

void *del234(tree234 * t, void *e)
{
	int index;
	if (!findrelpos234(t, e, NULL, REL234_EQ, &index))
		return NULL;	/* it wasn't in there anyway */
	return delpos234_internal(t, index);	/* it's there; delete it. */
}

/* ----------------------------------------------------------------------
 * Generic definitions.
 */

/*
 * Maximum amount of backlog we will allow to build up on an input
 * handle before we stop reading from it.
 */
#define MAX_BACKLOG 32768

struct handle_generic {
	/*
	 * Initial fields common to both handle_input and handle_output
	 * structures.
	 * 
	 * The three HANDLEs are set up at initialisation time and are
	 * thereafter read-only to both main thread and subthread.
	 * `moribund' is only used by the main thread; `done' is
	 * written by the main thread before signalling to the
	 * subthread. `defunct' and `busy' are used only by the main
	 * thread.
	 */
	HANDLE h;		/* the handle itself */
	HANDLE ev_to_main;	/* event used to signal main thread */
	HANDLE ev_from_main;	/* event used to signal back to us */
	int moribund;		/* are we going to kill this soon? */
	int done;		/* request subthread to terminate */
	int defunct;		/* has the subthread already gone? */
	int busy;		/* operation currently in progress? */
	void *privdata;		/* for client to remember who they are */
};

/* ----------------------------------------------------------------------
 * Input threads.
 */

/*
 * Data required by an input thread.
 */
struct handle_input {
	/* Copy of the handle_generic structure. */
	HANDLE h;		/* the handle itself */
	HANDLE ev_to_main;	/* event used to signal main thread */
	HANDLE ev_from_main;	/* event used to signal back to us */
	int moribund;		/* are we going to kill this soon? */
	int done;		/* request subthread to terminate */
	int defunct;		/* has the subthread already gone? */
	int busy;		/* operation currently in progress? */
	void *privdata;		/* for client to remember who they are */
	/* Data set at initialisation and then read-only. */
	int flags;
	/*
	 * Data set by the input thread before signalling ev_to_main,
	 * and read by the main thread after receiving that signal.
	 */
	char buffer[4096];	/* the data read from the handle */
	DWORD len;		/* how much data that was */
	int readerr;		/* lets us know about read errors */
	/*
	 * Callback function called by this module when data arrives on
	 * an input handle.
	 */
	handle_inputfn_t gotdata;
};

/*
 * The actual thread procedure for an input thread.
 */
static DWORD WINAPI handle_input_threadfunc(void *param)
{
	struct handle_input *ctx = (struct handle_input *) param;
	OVERLAPPED ovl, *povl;
	HANDLE oev;
	int readret, readlen;
	
	if (ctx->flags & HANDLE_FLAG_OVERLAPPED) {
		povl = &ovl;
		oev = CreateEvent(NULL, TRUE, FALSE, NULL);
	} else {
		povl = NULL;
	}
	
	if (ctx->flags & HANDLE_FLAG_UNITBUFFER)
		readlen = 1;
	else
		readlen = sizeof(ctx->buffer);
	
	while (1) {
		if (povl) {
			memset(povl, 0, sizeof(OVERLAPPED));
			povl->hEvent = oev;
		}
		readret = ReadFile(ctx->h, ctx->buffer,readlen, &ctx->len, povl);
		if (!readret)
			ctx->readerr = GetLastError();
		else
			ctx->readerr = 0;
		if (povl && !readret && ctx->readerr == ERROR_IO_PENDING) {
			WaitForSingleObject(povl->hEvent, INFINITE);
			readret = GetOverlappedResult(ctx->h, povl, &ctx->len, FALSE);
			if (!readret)
				ctx->readerr = GetLastError();
			else
				ctx->readerr = 0;
		}
		
		if (!readret) {
			/*
			 * Windows apparently sends ERROR_BROKEN_PIPE when a
			 * pipe we're reading from is closed normally from the
			 * writing end. This is ludicrous; if that situation
			 * isn't a natural EOF, _nothing_ is. So if we get that
			 * particular error, we pretend it's EOF.
			 */
			if (ctx->readerr == ERROR_BROKEN_PIPE)
				ctx->readerr = 0;
			ctx->len = 0;
		}
		
		if (readret && ctx->len == 0 &&
			(ctx->flags & HANDLE_FLAG_IGNOREEOF))
			continue;
		
		SetEvent(ctx->ev_to_main);
		
		if (!ctx->len)
			break;
		
		WaitForSingleObject(ctx->ev_from_main, INFINITE);
		if (ctx->done)
			break;		       /* main thread told us to shut down */
	}
	
	if (povl)
		CloseHandle(oev);
	
	return 0;
}

/*
 * This is called after a succcessful read, or from the
 * `unthrottle' function. It decides whether or not to begin a new
 * read operation.
 */
static void handle_throttle(struct handle_input *ctx, int backlog)
{
	if (ctx->defunct)
		return;
	
	/*
	 * If there's a read operation already in progress, do nothing:
	 * when that completes, we'll come back here and be in a
	 * position to make a better decision.
	 */
	if (ctx->busy)
		return;
	
	/*
	 * Otherwise, we must decide whether to start a new read based
	 * on the size of the backlog.
	 */
	if (backlog < MAX_BACKLOG) {
		SetEvent(ctx->ev_from_main);
		ctx->busy = TRUE;
	}
}

/* ----------------------------------------------------------------------
 * Output threads.
 */

/*
 * Data required by an output thread.
 */
struct handle_output {
	/*
	 * Copy of the handle_generic structure.
	 */
	HANDLE h;		/* the handle itself */
	HANDLE ev_to_main;	/* event used to signal main thread */
	HANDLE ev_from_main;	/* event used to signal back to us */
	int moribund;		/* are we going to kill this soon? */
	int done;		/* request subthread to terminate */
	int defunct;		/* has the subthread already gone? */
	int busy;		/* operation currently in progress? */
	void *privdata;		/* for client to remember who they are */
	/* Data set at initialisation and then read-only. */
	int flags;
	/*
	 * Data set by the main thread before signalling ev_from_main,
	 * and read by the input thread after receiving that signal.
	 */
	char *buffer;		/* the data to write */
	DWORD len;		/* how much data there is */
	/*
	 * Data set by the input thread before signalling ev_to_main,
	 * and read by the main thread after receiving that signal.
	 */
	DWORD lenwritten;	/* how much data we actually wrote */
	int writeerr;		/* return value from WriteFile */
	/*
	 * Data only ever read or written by the main thread.
	 */
	bufchain queued_data;	/* data still waiting to be written */
	/*
	 * Callback function called when the backlog in the bufchain
	 * drops.
	 */
	handle_outputfn_t sentdata;
};

static DWORD WINAPI handle_output_threadfunc(void *param)
{
	struct handle_output *ctx = (struct handle_output *) param;
	OVERLAPPED ovl, *povl;
	HANDLE oev;
	int writeret;
	
	if (ctx->flags & HANDLE_FLAG_OVERLAPPED) {
		povl = &ovl;
		oev = CreateEvent(NULL, TRUE, FALSE, NULL);
	} else {
		povl = NULL;
	}
	
	while (1) {
		WaitForSingleObject(ctx->ev_from_main, INFINITE);
		if (ctx->done) {
			SetEvent(ctx->ev_to_main);
			break;
		}
		if (povl) {
			memset(povl, 0, sizeof(OVERLAPPED));
			povl->hEvent = oev;
		}
		
		writeret = WriteFile(ctx->h, ctx->buffer, ctx->len,
			&ctx->lenwritten, povl);
		if (!writeret)
			ctx->writeerr = GetLastError();
		else
			ctx->writeerr = 0;
		if (povl && !writeret && GetLastError() == ERROR_IO_PENDING) {
			writeret = GetOverlappedResult(ctx->h, povl,
				&ctx->lenwritten, TRUE);
			if (!writeret)
				ctx->writeerr = GetLastError();
			else
				ctx->writeerr = 0;
		}
		
		SetEvent(ctx->ev_to_main);
		if (!writeret)
			break;
	}
	
	if (povl)
		CloseHandle(oev);
	
	return 0;
}

static void handle_try_output(struct handle_output *ctx)
{
	void *senddata;
	int sendlen;
	
	if (!ctx->busy && bufchain_size(&ctx->queued_data)) {
		bufchain_prefix(&ctx->queued_data, &senddata, &sendlen);
		ctx->buffer = senddata;
		ctx->len = sendlen;
		SetEvent(ctx->ev_from_main);
		ctx->busy = TRUE;
	}
}

/* ----------------------------------------------------------------------
 * Unified code handling both input and output threads.
 */

struct handle {
	int output;
	union {
		struct handle_generic g;
		struct handle_input i;
		struct handle_output o;
	} u;
};

static tree234 *handles_by_evtomain;

static int handle_cmp_evtomain(void *av, void *bv)
{
	struct handle *a = (struct handle *)av;
	struct handle *b = (struct handle *)bv;
	
	if ((unsigned)a->u.g.ev_to_main < (unsigned)b->u.g.ev_to_main)
		return -1;
	else if ((unsigned)a->u.g.ev_to_main > (unsigned)b->u.g.ev_to_main)
		return +1;
	else
		return 0;
}

static int handle_find_evtomain(void *av, void *bv)
{
	HANDLE *a = (HANDLE *)av;
	struct handle *b = (struct handle *)bv;
	
	if ((unsigned)*a < (unsigned)b->u.g.ev_to_main)
		return -1;
	else if ((unsigned)*a > (unsigned)b->u.g.ev_to_main)
		return +1;
	else
		return 0;
}

struct handle *handle_input_new(HANDLE handle, handle_inputfn_t gotdata,
				void *privdata, int flags)
{
	struct handle *h = snew(struct handle);
	DWORD in_threadid; /* required for Win9x */
	
	h->output = FALSE;
	h->u.i.h = handle;
	h->u.i.ev_to_main = CreateEvent(NULL, FALSE, FALSE, NULL);
	h->u.i.ev_from_main = CreateEvent(NULL, FALSE, FALSE, NULL);
	h->u.i.gotdata = gotdata;
	h->u.i.defunct = FALSE;
	h->u.i.moribund = FALSE;
	h->u.i.done = FALSE;
	h->u.i.privdata = privdata;
	h->u.i.flags = flags;
	
	if (!handles_by_evtomain)
		handles_by_evtomain = newtree234(handle_cmp_evtomain);
	add234(handles_by_evtomain, h);
	
	CreateThread(NULL, 0, handle_input_threadfunc,
		&h->u.i, 0, &in_threadid);
	h->u.i.busy = TRUE;
	
	return h;
}

struct handle *handle_output_new(HANDLE handle, handle_outputfn_t sentdata,
				 void *privdata, int flags)
{
	struct handle *h = snew(struct handle);
	DWORD out_threadid; /* required for Win9x */
	
	h->output = TRUE;
	h->u.o.h = handle;
	h->u.o.ev_to_main = CreateEvent(NULL, FALSE, FALSE, NULL);
	h->u.o.ev_from_main = CreateEvent(NULL, FALSE, FALSE, NULL);
	h->u.o.busy = FALSE;
	h->u.o.defunct = FALSE;
	h->u.o.moribund = FALSE;
	h->u.o.done = FALSE;
	h->u.o.privdata = privdata;
	bufchain_init(&h->u.o.queued_data);
	h->u.o.sentdata = sentdata;
	h->u.o.flags = flags;
	
	if (!handles_by_evtomain)
		handles_by_evtomain = newtree234(handle_cmp_evtomain);
	add234(handles_by_evtomain, h);
	
	CreateThread(NULL, 0, handle_output_threadfunc,
		&h->u.o, 0, &out_threadid);
	
	return h;
}

int handle_write(struct handle *h, const void *data, int len)
{
	assert(h->output);
	bufchain_add(&h->u.o.queued_data, data, len);
	handle_try_output(&h->u.o);
	return bufchain_size(&h->u.o.queued_data);
}

HANDLE *handle_get_events(int *nevents)
{
	HANDLE *ret;
	struct handle *h;
	int i, n, size;
	
	/*
	 * Go through our tree counting the handle objects currently
	 * engaged in useful activity.
	 */
	ret = NULL;
	n = size = 0;
	if (handles_by_evtomain) {
		for (i = 0; (h = index234(handles_by_evtomain, i)) != NULL; i++) {
			if (h->u.g.busy) {
				if (n >= size) {
					size += 32;
					ret = sresize(ret, size, HANDLE);
				}
				ret[n++] = h->u.g.ev_to_main;
			}
		}
	}
	
	*nevents = n;
	return ret;
}

static void handle_destroy(struct handle *h)
{
	if (h->output)
		bufchain_clear(&h->u.o.queued_data);
	CloseHandle(h->u.g.ev_from_main);
	CloseHandle(h->u.g.ev_to_main);
	del234(handles_by_evtomain, h);
	sfree(h);
}

void handle_free(struct handle *h)
{
	/*
	 * If the handle is currently busy, we cannot immediately free
	 * it. Instead we must wait until it's finished its current
	 * operation, because otherwise the subthread will write to
	 * invalid memory after we free its context from under it.
	 */
	assert(h && !h->u.g.moribund);
	if (h->u.g.busy) {
		/*
		 * Just set the moribund flag, which will be noticed next
		 * time an operation completes.
		 */
		h->u.g.moribund = TRUE;
	} else if (h->u.g.defunct) {
		/*
		 * There isn't even a subthread; we can go straight to
		 * handle_destroy.
		 */
		handle_destroy(h);
	} else {
		/*
		 * The subthread is alive but not busy, so we now signal
		 * it to die. Set the moribund flag to indicate that it
		 * will want destroying after that.
		 */
		h->u.g.moribund = TRUE;
		h->u.g.done = TRUE;
		h->u.g.busy = TRUE;
		SetEvent(h->u.g.ev_from_main);
	}
}

void handle_got_event(HANDLE event)
{
	struct handle *h;
	
	assert(handles_by_evtomain);
	h = find234(handles_by_evtomain, &event, handle_find_evtomain);
	if (!h) {
		/*
		 * This isn't an error condition. If two or more event
		 * objects were signalled during the same select
		 * operation, and processing of the first caused the
		 * second handle to be closed, then it will sometimes
		 * happen that we receive an event notification here for a
		 * handle which is already deceased. In that situation we
		 * simply do nothing.
		 */
		return;
	}
	
	if (h->u.g.moribund) {
		/*
		 * A moribund handle is already treated as dead from the
		 * external user's point of view, so do nothing with the
		 * actual event. Just signal the thread to die if
		 * necessary, or destroy the handle if not.
		 */
		if (h->u.g.done) {
			handle_destroy(h);
		} else {
			h->u.g.done = TRUE;
			h->u.g.busy = TRUE;
			SetEvent(h->u.g.ev_from_main);
		}
		return;
	}
	
	if (!h->output) {
		int backlog;
		
		h->u.i.busy = FALSE;
		
		/*
		 * A signal on an input handle means data has arrived.
		 */
		if (h->u.i.len == 0) {
			/*
			 * EOF, or (nearly equivalently) read error.
			 */
			h->u.i.gotdata(h, NULL, -h->u.i.readerr);
			h->u.i.defunct = TRUE;
		} else {
			backlog = h->u.i.gotdata(h, h->u.i.buffer, h->u.i.len);
			handle_throttle(&h->u.i, backlog);
		}
	} else {
		h->u.o.busy = FALSE;
		
		/*
		 * A signal on an output handle means we have completed a
		 * write. Call the callback to indicate that the output
		 * buffer size has decreased, or to indicate an error.
		 */
		if (h->u.o.writeerr) {
			/*
			 * Write error. Send a negative value to the
			 * callback, and mark the thread as defunct
			 * (because the output thread is terminating by
			 * now).
			 */
			h->u.o.sentdata(h, -h->u.o.writeerr);
			h->u.o.defunct = TRUE;
		} else {
			bufchain_consume(&h->u.o.queued_data, h->u.o.lenwritten);
			h->u.o.sentdata(h, bufchain_size(&h->u.o.queued_data));
			handle_try_output(&h->u.o);
		}
	}
}

void handle_unthrottle(struct handle *h, int backlog)
{
	assert(!h->output);
	handle_throttle(&h->u.i, backlog);
}

int handle_backlog(struct handle *h)
{
	assert(h->output);
	return bufchain_size(&h->u.o.queued_data);
}

void *handle_get_privdata(struct handle *h)
{
	return h->u.g.privdata;
}
