/****************************************************************************/
/*                           the diy toolsuite                              */
/*                                                                          */
/* Jade Alglave, University College London, UK.                             */
/* Luc Maranget, INRIA Paris-Rocquencourt, France.                          */
/*                                                                          */
/* Copyright 2015-present Institut National de Recherche en Informatique et */
/* en Automatique and the authors. All rights reserved.                     */
/*                                                                          */
/* This software is governed by the CeCILL-B license under French law and   */
/* abiding by the rules of distribution of free software. You can use,      */
/* modify and/ or redistribute the software under the terms of the CeCILL-B */
/* license as circulated by CEA, CNRS and INRIA at the following URL        */
/* "http://www.cecill.info". We also give a copy in LICENSE.txt.            */
/****************************************************************************/
#include <target/litmus.h>

int gcd(int a, int b)
{
	int tmp;

	for (; ; ) {
		if (a == 0)
			return b;
		tmp = a;
		a = b % a;
		b = tmp;
	}
}

int find_string(char *t[], int sz, char *s)
{
	int k;

	for (k = 0; k < sz; k++) {
		if (strcmp(t[k], s) == 0)
			return k;
	}
	return -1;
}

void *malloc_check(size_t sz, const char *name)
{
	void *p;

	if (sz == 0) return NULL;
	p = malloc(sz);
	if (!p) {
		if (!errno) errno = ENOMEM;
		BUG();
	}
#ifdef CONFIG_TEST_LITMUS_DEBUG
	printf("M(%s): %016llx - %016llx\n", name, p, sz);
#endif
	return p;
}

void free_check(void *p, const char *name)
{
#ifdef CONFIG_TEST_LITMUS_DEBUG
	printf("F(%s): %016llx\n", name, p);
#endif
	if (p)
		free(p);
}

void pp_ints(bool force_info, int *p, int n)
{
	int k;

	if (n > 0) {
		if (force_info)
			printf("%i", p[0]);
		else
			litmus_log("%i", p[0]);
		for (k = 1; k < n; k++) {
			if (force_info)
				printf(",%i", p[k]);
			else
				litmus_log(",%i", p[k]);
		}
	}
}

/************/
/* CPU sets */
/************/

cpus_t *cpus_create(int sz, const char *name)
{
	cpus_t *r = malloc_check(sizeof(*r) + sz*sizeof(r->cpu[0]), name);
	r->sz = sz;
	return r;
}

void cpus_free(cpus_t *p, const char *name)
{
	free_check(p, name);
}

void cpus_dump(bool force_info, cpus_t *p)
{
	pp_ints(force_info, p->cpu, p->sz);
}

void cpus_dump_test(int *p, int sz, cpus_t *cm, int nprocs)
{
	int k, i;

	for (k = 0; k < sz; k += nprocs) {
		litmus_log("[");
		pp_ints(false, &p[k], nprocs);
		litmus_log("] {");
		if (nprocs > 0) {
			litmus_log("%i", cm->cpu[p[k]]);
			for (i = 1; i < nprocs; i++) {
				litmus_log(",%i", cm->cpu[p[k+i]]);
			}
		}
		litmus_log("}\n");
	}
}

cpus_t *coremap_seq(int navail, int nways, const char *name)
{
	cpus_t *r = cpus_create(navail, name);
	int ncores = navail / nways;
	int i = 0;
	int c, k;

	for (c = 0; c < ncores; c++) {
		for (k = 0; k < nways; k++) {
			r->cpu[i++] = c;
		}
	}
	return r;
}

typedef struct {
	int ncores;
	cpus_t *core[0];
} mapcore_t;

static void mapcore_free(mapcore_t *p)
{
	int c;

	for (c = 0; c < p->ncores; c++)
		cpus_free(p->core[c], "mapcore");
	free_check(p, "mapcore");
}

static int get_ncores(cpus_t *cm)
{
	int r = 0;
	int k;

	for (k = 0; k < cm->sz; k++) {
		if (cm->cpu[k] > r)
			r = cm->cpu[k];
	}
	return r+1;
}

static cpus_t *get_core_procs(cpus_t *cm, cpus_t *p, int c)
{
	int sz = 0;
	int k, i;
	cpus_t *r;
	int proc;

	for (k = 0; k < p->sz; k++) {
		if (cm->cpu[p->cpu[k]] == c)
		       sz++;
	}
	r = cpus_create(sz, "mapcore");
	i = 0;
	for (k = 0; k < p->sz; k++) {
		proc = p->cpu[k];
		if (cm->cpu[proc] == c)
			r->cpu[i++] = proc;
	}
	return r;
}

static mapcore_t *inverse_procs(cpus_t *cm, cpus_t *p)
{
	int c;
	int ncores = get_ncores(cm);
	mapcore_t *r;

	r = malloc_check(sizeof(*r) + ncores*sizeof(r->core[0]), "mapcore");
	r->ncores = ncores;
	for (c = 0; c < ncores; c++)
		r->core[c] = get_core_procs(cm, p, c);
	return r;
}

static int get_node_sz(int *p)
{
	int r = 0;
	while (*p++ >= 0) r++;
	return r;
}

static int get_n(int **p)
{
	int r = 0;
	while (*p) {
		r += get_node_sz(*p);
		p++;
	}
	return r;
}

static int ok_one_color(int *cm, int *d, int *a, int n, int p, int c)
{
	int k;
	int op, oc;

	for (k = 0; k < n; k++) {
		op = a[k];
		if (op >= 0) {
			if (d[n*p+k]) {
				oc = cm[op];
				if (oc == c) {
					return 0;
				}
			}
		}
	}
	return 1;
}

static int ok_color(int *cm, int *d, int *a, int n, int *q, int c)
{
	for (; *q >= 0; q++) {
		if (!ok_one_color(cm, d, a, n, *q, c))
			return 0;
	}
	return 1;
}

static int find_color_diff(int prev, st_t *st, int *cm, mapcore_t *mc,
			   int *d, int *a,int n, int *q)
{
	int sz = get_node_sz(q);
	int k0 = prev >= 0 && rand_bit(st) ? prev : rand_k(st,mc->ncores);
	int k = k0;

	do {
		cpus_t *p = mc->core[k];
		if (p->sz >= sz && ok_color(cm, d, a, n, q, k))
		       return k;
		k++;
		k %= mc->ncores;
	} while (k != k0);
	return -1;
}

static int find_one_proc(int prev, st_t *st, int *cm, mapcore_t *mc,
			 int *d, int *a, int n, int p)
{
	int found = -1;
	int k0 = prev >= 0 && rand_bit(st) ? prev : rand_k(st,mc->ncores);
	int k = k0;

	do {
		cpus_t *pk = mc->core[k];
		if (pk->sz > 0) {
			if (found < 0) found = k;
			if (ok_one_color(cm, d, a, n, p, k))
				return k;
		}
		k++;
		k %= mc->ncores;
	} while (k != k0);
	if (found < 0)
		litmus_fatal("Cannot allocate threads");
	return found;
}

void custom_affinity(st_t *st, cpus_t *cm, int **color, int *diff,
		     cpus_t *aff_cpus, int n_exe, int *r)
{
	mapcore_t *mc = inverse_procs(cm, aff_cpus);
	int n = get_n(color);
	/* Diff relation as matrix */
	int d[n*n];
	int k, c, x, y, i;
	int *a;
	int prev_core = -1;
	int *qq, **q;
	cpus_t *p;

	qq = diff;
	for (k = 0; k < n*n; k++) d[k] = 0;
	while (*qq >= 0) {
		x = *qq++, y = *qq++;
		d[n*x+y] = d[n*y+x] = 1;
	}

	for (k = 0; k < n_exe; k++) {
		a = &r[k*n];
		prev_core = -1;
		for (i = 0; i < n; i++) a[i] = -1;
		for (q = color; *q; q++) {
			c = find_color_diff(prev_core, st, aff_cpus->cpu,
					    mc, d, a, n, *q);
			if (c >= 0) {
				p = mc->core[c];
				for (qq = *q; *qq>= 0; qq++) {
					p->sz--;
					a[*qq] = p->cpu[p->sz];
				}
				prev_core = c;
			} else {
				for (qq = *q; *qq >= 0; qq++) {
					c = find_one_proc(prev_core, st,
						aff_cpus->cpu, mc, d, a,
						n, *qq);
					p = mc->core[c];
					p->sz--;
					a[*qq] = p->cpu[p->sz];
					prev_core = c;
				}
			}
		}
	}
	mapcore_free(mc);
}

/**********/
/* ramdom */
/**********/

/* Simple generator
 * http://en.wikipedia.org/wiki/Linear_congruential_generator
 */

/* From ocaml sources: (globroot.c)
 * Linear congruence with modulus = 2^32, multiplier = 69069
 * (Knuth vol 2 p. 106, line 15 of table 1), additive = 25173.
 *
 * Knuth (vol 2 p. 13) shows that the least significant bits are
 * "less random" than the most significant bits with a modulus of 2^m.
 * We just swap half words, enough?
 */
static const uint32_t a = 69069;
static const uint32_t c = 25173;

inline static uint32_t unlocked_rand(st_t *st)
{
	uint32_t low, high;
	uint32_t r = a * *st + c;

	*st = r;
	/* Swap high & low bits */
	low = r & 0xffff;
	high = r >> 16;
	r = high | (low << 16);
	return r;
}

int rand_bit(st_t *st)
{
	uint32_t r = unlocked_rand(st);
	r &= 1;
	return r; 
}

static const uint32_t r_max = UINT32_MAX;

uint32_t rand_k(uint32_t *st, uint32_t k)
{
	uint32_t r, v;
	do {
		r = unlocked_rand(st);
		v = r % k;
	} while (r - v > r_max - k + 1);
	return v;
}

/********/
/* outs */
/********/

static outs_t *alloc_outs(intmax_t k)
{
	outs_t *r = malloc_check(sizeof(*r), "outs");
	r->k = k;
	r->c = 0;
	r->show = 0;
	r->next = r->down = NULL;
	return r;
}

void free_outs(outs_t *p)
{
	if (p == NULL) return;
	free_outs(p->next);
	free_outs(p->down);
	free_check(p, "outs");
}

/* Worth writing as a loop, since called many times */
static outs_t *loop_add_outcome_outs(outs_t *p, intmax_t *k, int i,
				     count_t c, int show)
{
	outs_t *r = p;
	outs_t **q;

	if (p == NULL || k[i] < p->k) {
		r = alloc_outs(k[i]);
		r->next = p;
		p = r;
	}
	for (; ; ) {
		if (k[i] > p->k) {
			q = &(p->next);
			p = p->next;
		} else if (i <= 0) {
			p->c += c;
			p->show = show || p->show;
			return r;
		} else {
			i--;
			q = &(p->down);
			p = p->down;
		}
		if (p == NULL || k[i] < p->k) {
			outs_t *a = alloc_outs(k[i]);
			a->next = p;
			p = a;
			*q = a;
		}
	}
}

outs_t *add_outcome_outs(outs_t *p, intmax_t *k, int sz, count_t c, int show)
{
	return loop_add_outcome_outs(p, k, sz-1, c, show);
}

count_t sum_outs(outs_t *p)
{
	count_t r = 0;

	for (; p; p = p->next) {
		r += p->c;
		r += sum_outs(p->down);
	}
	return r;
}

int finals_outs(outs_t *p)
{
	int r = 0;

	for (; p; p = p->next) {
		if (p->c > 0) r++;
		r += finals_outs(p->down);
	}
	return r;
}

void dump_outs(dump_outcome *dout, outs_t *p,
	       intmax_t *buff, int sz)
{
	for (; p; p = p->next) {
		buff[sz-1] = p->k;
		if (p->c > 0) {
			dout(buff,p->c,p->show);
		} else if (p->down) {
			dump_outs(dout, p->down, buff, sz-1);
		}
	}
}

/* merge p and q into p */
static outs_t *do_merge_outs(outs_t *p, outs_t *q)
{
	if (q == NULL) { // Nothing to add
		return p;
	}
	if (p == NULL || q->k < p->k) { // Need a cell
		outs_t *r = alloc_outs(q->k);
		r->next = p;
		p = r;
	}
	if (p->k == q->k) {
		p->c += q->c;
		p->show = p->show || q->show;
		p->down = do_merge_outs(p->down,q->down);
		p->next = do_merge_outs(p->next,q->next);
	} else {
		p->next = do_merge_outs(p->next,q);
	}
	return p;
}

outs_t *merge_outs(outs_t *p, outs_t *q, int sz)
{
	return do_merge_outs(p,q);
}

/***********************/
/* Prefetch directives */
/***********************/

static prfone_t *get_name_slot(prfproc_t *p, char *name)
{
	int nvars = p->nvars;
	prfone_t *q = p->t;
	int _v;

	for (_v = 0; _v < nvars; _v++) {
		if (strcmp(name,q[_v].name) == 0)
			return &q[_v];
	}
	return NULL; /* Name not found */
}

void set_prefetch(prfdirs_t *p, prfdir_t d)
{
	prfproc_t *q = p->t;
	int _p, _v;

	for (_p = 0; _p < p->nthreads; _p++) {
		int nvars = q[_p].nvars;
		prfone_t *r = q[_p].t;

		for (_v = 0; _v < nvars; _v++) {
			r[_v].dir = d;
		}
	}
}

int parse_prefetch(char *p, prfdirs_t *r)
{
	if (!*p) return 1;
	for (; ; ) {
		char *q;
		int proc = (int)strtoul(p, &q, 10);
		char *p0;
		prfone_t *loc_slot;
		prfdir_t dir;
		char c;

		if (proc < 0 || proc >= r->nthreads ||
		    *p == '\0' || *q != ':')
			return 0;
		p = q + 1;
		p0 = p;
		while (*p != '=') {
			if (*p == '\0')
				return 0;
			p++;
		}
		*p = '\0';
		loc_slot = get_name_slot(&r->t[proc], p0);
		if (loc_slot == NULL) {
			litmus_log("Proc %i does not access variable %s\n",
				 proc, p0);
			*p = '=';
			return 0;
		}
		*p = '=';
		c = *++p;
		dir = none;
		switch (c) {
		case 'F':
			dir = flush;
			break;
		case 'T':
			dir = touch;
			break;
		case 'W':
			dir = touch_store;
			break;
		}
		loc_slot->dir = dir;
		c = *++p;
		if (c == '\0')
			return 1;
		else if (c == ',')
			p++;
		else
			return 0;
	}
}

/*****************/
/* Random things */
/*****************/

void perm_prefix_ints(unsigned *st, int *_t, int m, int n)
{
	int k, j, x;

	for (k = 0; k < m; k++) {
		j = k + rand_k(st, n - k);
		x = _t[k]; _t[k] = _t[j]; _t[j] = x;
	}
}

void perm_funs(unsigned *st, f_t *fun[], int n)
{
	int k, j;
	f_t *t;

	for (k = 0; k < n - 1; k++) {
		j = k + rand_k(st, n - k);
		t = fun[j];
		fun[j] = fun[k]; fun[k] = t;
	}
}

#if 0
void perm_threads(unsigned *st, pthread_t thread[], int n)
{
	int k;
	int j;
	pthread_t t;

	for (k = 0; k < n-1; k++) {
		j = k + rand_k(st, n - k);
		t = thread[j];
		thread[j] = thread[k]; thread[k] = t;
	}
}
#endif

/*************************/
/* Concurrency utilities */
/*************************/

/* phread based mutex */
pm_t *pm_create(void)
{
	pm_t *p = malloc_check(sizeof(*p), "pm");
	spin_lock_init(p);
	return p;
}

void pm_free(pm_t *p)
{
	free_check(p, "pm");
}

void pm_lock(pm_t *m)
{
	spin_lock(m);
}

void pm_unlock(pm_t *m)
{
	spin_unlock(m);
}

/* pthread based barrier, usable for nproc threads */
pb_t *pb_create(int nprocs)
{
	pb_t *p = malloc_check(sizeof(*p), "pb");
	p->cond = pm_create();
	p->count = p->nprocs = nprocs;
	p->turn = 0;
	return p;
}

void pb_free(pb_t *p)
{
	pm_free(p->cond);
	free_check(p, "pb");
}

/* The following code should protect us against spurious wake ups */
void pb_wait(pb_t *p)
{
	int t;

	pm_lock(p->cond);
	t = p->turn;
	--p->count;
	if (p->count == 0) {
		p->count = p->nprocs;
		p->turn = !t;
		pm_unlock(p->cond);
		smp_wmb();
		pm_lock(p->cond);
	} else {
		do {
			pm_unlock(p->cond);
			smp_rmb();
			pm_lock(p->cond);
		} while (p->turn == t);
	}
	pm_unlock(p->cond);
}

/****************/
/* Command line */
/****************/

static void ints_dump(ints_t *p)
{
	int k;

	if (p->sz > 0) {
		printf("%i:%i", 0, p->t[0]);
		for (k = 1; k < p->sz; k++) {
			printf(",%i:%i", k, p->t[k]);
		}
	}
}

static void prefetch_dump(prfdirs_t *p)
{
	prfproc_t *q = p->t;
	int some = 0;
	int _p, _v;

	for (_p = 0; _p < p->nthreads; _p++) {
		int nvars = q[_p].nvars;
		prfone_t *r = q[_p].t;

		for (_v = 0; _v < nvars; _v++) {
			prfdir_t dir = r[_v].dir;
			if (dir != none) {
				__unused char c = 'I';
				if (dir == flush) c = 'F';
				else if (dir == touch) c = 'T';
				else if (dir == touch_store) c = 'W';
				if (some) {
					litmus_log(",");
				} else {
					some = 1;
				}
				printf("%i:%s=%c", _p, r[_v].name, c);
			}
		}
	}
}

static void usage(char *prog, cmd_t *d)
{
	printf("usage: %s (options)*\n", prog);
	printf("  -v      be verbose\n");
	printf("  -q      be quiet\n");
	printf("  -a <n>  run maximal number of tests for n available processors (default %i)\n",
	       d->avail);
	printf("  -n <n>  run n tests concurrently\n");
	printf("  -r <n>  perform n runs (default %i)\n", d->max_run);
	printf("  -fr <f> multiply run number per f\n");
	printf("  -s <n>  outcomes per run (default %i)\n",
	       d->size_of_test);
	if (d->stride > 0) {
		printf("  -st <n> stride (default %i)\n", d->stride);
	}
	printf("  -fs <f> multiply outcomes per f\n");
	printf("  -f <f>  multiply outcomes per f, divide run number by f\n");
	if (d->aff_mode != aff_none) {
		printf("  -i <n>  increment for allocating logical processors, -i 0 disables affinity mode");
		if (d->aff_mode == aff_incr) {
			printf(" (default %i)\n", d->aff_incr);
		} else {
			printf("\n");
		}
		printf("  -p <ns> specify logical processors (default '");
		cpus_dump(true, d->aff_cpus);
		printf("')\n");
		printf("  +ra     randomise affinity%s\n",
		       d->aff_mode == aff_random ? " (default)" : "");
		if (d->aff_custom_enabled) {
			printf("  +ca     enable custom affinity%s\n",
			       d->aff_mode == aff_custom ? " (default)" : "");
		} else {
			printf("  +ca     alias for +ra\n");
		}
		if (d->aff_scan_enabled) {
			printf("  +sa     enable scanning affinity%s\n",
			       d->aff_mode == aff_scan ?  " (default)" : "");
			printf("  +ta <topo> set topology affinity\n");
		} else {
			printf("  +sa     alias for +ra\n");
		}
	}
	if (d->shuffle >= 0) {
		printf("  +rm     randomise memory accesses%s\n",
		       d->shuffle ? " (default)" : "");
		printf("  -rm     do not randomise memory accesses%s\n",
		       !d->shuffle ? " (default)" : "");
	}
	if (d->speedcheck >= 0) {
		printf("  +sc     stop as soon as possible%s\n",
		       d->speedcheck ? " (default)" : "");
		printf("  -sc     run test completly%s\n",
		       !d->speedcheck ? " (default)" : "");
	}
	if (!d->fix) {
		printf("  +fix    fix thread launch order\n");
	}
	if (d->delta_tb) {
		printf("  -tb <list> set timebase delays, default '");
		ints_dump(d->delta_tb);
		printf("'\n");
		printf("    List syntax is comma separated proc:delay\n");
		printf("  -ta <n>    set all timebase delays\n");
	}
	if (d->verbose_barrier >= 0) {
		printf("  +vb     show iteration timings%s\n",
		       d->verbose_barrier ? " (default)" : "");
		printf("  -vb     do not show iteration timings%s\n",
		       !d->verbose_barrier ? " (default)" : "");
	}
	if (d->prefetch) {
		printf("  -pra (I|F|T|W) set all prefetch\n");
		printf("  -prf <list> set prefetch, default '");
		prefetch_dump(d->prefetch);
		printf("'\n");
		printf("    List syntax is comma separated proc:name=(I|F|T|W)\n");
	}
	if (d->static_prefetch >= 0) {
		printf("  -prs <n> prefetch probability is 1/n, -prs 0 disables feature, default %i\n",
		       d->static_prefetch);
	}
	if (d->max_loop > 0) {
		printf("  -l <n>  measure time by running assembly in a loop of size <n> (default %i)\n",
		       d->max_loop);
	}
	if (d->prelude > 0) {
		printf("  -vp     no verbose prelude\n");
	}
	if (d->sync_n > 0) {
		printf("  -k <n>  undocumented (default %i)\n", d->sync_n);
	}
}

static long my_add(long x, long y)
{
	long r = x + y;

	if (r < x || r < y) {
		errno = ERANGE;
		return -ERANGE;
	}
	return r;
}

static long my_pow10(int p, long x)
{
	long r = x;

	for (; p > 0; p--) {
		long y2, y4, y8;

		y2 = my_add(r, r);
		if (y2 < 0)
			return -ERANGE;
		y4 = my_add(y2, y2);
		if (y4 < 0)
			return -ERANGE;
		y8 = my_add(y4, y4);
		if (y8 < 0)
			return -ERANGE;
		r = my_add(y8, y2);
	}
	if (r >= INT_MAX || r <= 0) {
		errno = ERANGE;
		return -ERANGE;
	}
	return r;
}

static int do_argint(char *p, char **q)
{
	long r = strtoul(p, q, 10);

	if (errno == ERANGE) {
		return -ERANGE;
	}
	if (**q == 'k' || **q == 'K') {
		r = my_pow10(3, r);
		if (r < 0)
			return -ERANGE;
	       	*q += 1;
	} else if (**q == 'm' || **q == 'M') {
		r = my_pow10(6, r);
		if (r < 0)
			return -ERANGE;
		*q += 1;
	}
	return (int)r;
}

static int argint(char *prog, char *p, cmd_t *d)
{
	char *q;
	long r = do_argint(p, &q);

	if (r < 0)
		return -ERANGE;
	if (*p == '\0' || *q != '\0') {
		usage(prog, d);
		return -EINVAL;
	}
	return (int)r;
}

static cpus_t *argcpus(char *prog, char *p0, cmd_t *d, const char *name)
{
	int sz = 0;
	char *p;
	cpus_t *r;
	int k;

	p = p0;
	for (; ; ) {
		char *q;
		int x = (int)strtoul(p, &q, 10);
		if (x < 0 || *p == '\0' || (*q != '\0' && *q != ',')) {
			usage(prog, d);
			return NULL;
		}
		sz++;
		if (*q == '\0')
			break;
		p = q + 1;
	}
	r = cpus_create(sz, name);
	p = p0;
	for (k = 0; k < sz; k++) {
		char *q;
		r->cpu[k] = (int)strtoul(p, &q, 10);
		p = q + 1;
	}
	return r;
}

static int argints(char *prog, cmd_t *d, char *p, ints_t *r)
{
	while (*p) {
		char *q;
		int idx = (int)strtoul(p, &q, 10);
		int v;

		if (idx < 0 || idx >= r->sz || *p == '\0' || *q != ':') {
			usage(prog, d);
			return -EINVAL;
		}
		p = q + 1;
		v = do_argint(p, &q);
		if (v < 0)
			return -ERANGE;
		if (*p == '\0' || (*q != '\0' && *q != ',')) {
			usage(prog, d);
			return -EINVAL;
		}
		r->t[idx] = v;
		if (*q == '\0') {
			p = q;
		} else {
			p = q + 1;
		}
	}
	return 0;
}

static void argoneprefetch(char *prog, cmd_t *d, char *p, prfdirs_t *r)
{
	prfdir_t dir = none;

	switch (*p) {
	case 'F':
		dir = flush;
		break;
	case 'T':
		dir = touch;
		break;
	case 'W':
		dir = touch_store;
		break;
	}
	set_prefetch(r, dir);
}

static int argprefetch(char *prog, cmd_t *d, char *p, prfdirs_t *r)
{
	if (!parse_prefetch(p, r)) {
		usage(prog, d);
		return -EINVAL;
	}
	return 0;
}

static double argdouble(char *prog, char *p, cmd_t *d)
{
	char *q;
	double r = strtod(p, &q);

	if (*p == '\0' || *q != '\0') {
		usage(prog,d);
		return (double)(-EINVAL);
	}
	return r;
}

int parse_cmd(int argc, char **argv, cmd_t *d, cmd_t *p)
{
	char *prog = argv[0];

	/* Options */
	for (; ; ) {
		char fst;

		--argc; ++argv;
		if (!*argv)
			break;
		fst = **argv;
		if (fst != '-' && fst != '+')
			break;
		if (strcmp(*argv, "-q") == 0)
			p->verbose = 0;
		else if (strcmp(*argv, "-v") == 0)
			p->verbose++;
		else if (strcmp(*argv, "-r") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			p->max_run = argint(prog, argv[0], d);
			if (p->max_run < 1)
				p->max_run = 1;
		} else if (strcmp(*argv, "-fr") == 0) {
			double f;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			f = argdouble(prog, argv[0], d);
			if (f < 0)
				f = 1.0;
			p->max_run *= f;
		} else if (strcmp(*argv, "-s") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			p->size_of_test = argint(prog, argv[0], d);
			if (p->size_of_test < 1)
				p->size_of_test = 1;
		} else if (d->stride > 0 && strcmp(*argv,"-st") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			p->stride = argint(prog, argv[0], d);
			if (p->stride < 0)
				p->stride = 0;
		} else if (strcmp(*argv, "-fs") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			p->size_of_test *= argdouble(prog, argv[0], d);
		} else if (strcmp(*argv, "-f") == 0) {
			double f;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			f = argdouble(prog, argv[0], d);
			if (f < 0)
				f = 1.0;
			p->size_of_test *= f;
			p->max_run /= f;
		} else if (strcmp(*argv, "-n") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			p->n_exe = argint(prog, argv[0], d);
			if (p->n_exe < 1)
				p->n_exe = 1;
		} else if (strcmp(*argv, "-a") == 0) {
			int a;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			a = argint(prog, argv[0], d);
			p->avail = a < 0 ? 0 : a;
		} else if (d->sync_n > 0 && strcmp(*argv, "-k") == 0) {
			int a;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			a = argint(prog, argv[0], d);
			p->sync_n = a < 0 ? 0 : a;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "-i") == 0) {
			int i;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			i = argint(prog,argv[0], d);
			p->aff_mode = aff_incr;
			p->aff_incr = i < 0 ? 0 : i;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "-p") == 0) {
			cpus_t *cpus;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			cpus = argcpus(prog, argv[0], d, "cmd->aff_cpus");
			if (!cpus)
				return 2;
			p->aff_cpus = cpus;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "+ra") == 0) {
			p->aff_mode = aff_random;
		} else if (d->aff_custom_enabled &&
			   strcmp(*argv, "+ca") == 0) {
			p->aff_mode = aff_custom;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "+ca") == 0) {
			p->aff_mode = aff_random;
		} else if (d->aff_scan_enabled &&
			   strcmp(*argv, "+sa") == 0) {
			p->aff_mode = aff_scan;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "+sa") == 0) {
			p->aff_mode = aff_random;
		} else if (d->aff_scan_enabled &&
			   strcmp(*argv, "+ta") == 0) {
			p->aff_mode = aff_topo;
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			p->aff_topo = argv[0];
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "+sa") == 0) {
			p->aff_mode = aff_random;
		} else if (d->shuffle >= 0 &&
			   strcmp(*argv, "+rm") == 0) {
			p->shuffle = 1;
		} else if (d->shuffle >= 0 &&
			   strcmp(*argv, "-rm") == 0) {
			p->shuffle = 0;
		} else if (d->speedcheck >= 0 &&
			   strcmp(*argv, "+sc") == 0) {
			p->speedcheck = 1;
		} else if (d->speedcheck >= 0 &&
			   strcmp(*argv, "-sc") == 0) {
			p->speedcheck = 0;
		} else if (!d->fix && strcmp(*argv, "+fix") == 0) {
			p->fix = 1;
		} else if (d->verbose_barrier >= 0 &&
			   strcmp(*argv, "+vb") == 0) {
			p->verbose_barrier++;
		} else if (d->verbose_barrier >= 0 &&
			   strcmp(*argv, "-vb") == 0) {
			p->verbose_barrier = 0;
		} else if (d->prelude > 0 &&
			   strcmp(*argv, "-vp") == 0) {
			p->prelude = 0;
		} else if (d->delta_tb && strcmp(*argv, "-tb") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			if (argints(prog, d, argv[0], p->delta_tb) != 0)
				return 2;
		} else if (d->delta_tb && strcmp(*argv,"-ta") == 0) {
			int da;
			int k;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			da = argint(prog, argv[0], d);
			for (k = 0; k < p->delta_tb->sz; k++)
				p->delta_tb->t[k] = da;
		} else if (d->prefetch && strcmp(*argv, "-prf") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			if (argprefetch(prog, d, argv[0], p->prefetch) != 0)
				return 2;
		} else if (d->prefetch && strcmp(*argv,"-pra") == 0) {
			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			argoneprefetch(prog, d, argv[0], p->prefetch);
		} else if (d->static_prefetch >= 0 &&
			   strcmp(*argv, "-prs") == 0) {
			int prs;

			--argc; ++argv;
			if (!*argv) {
				usage(prog, d);
				return 2;
			}
			prs = argint(prog, argv[0], d);
			p->static_prefetch = prs >= 0 ? prs : 0;
		} else if (d->max_loop > 0 && strcmp(*argv, "-l") == 0) {
			int i;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			i = argint(prog, argv[0], d);
			p->max_loop = i < 1 ? 1 : i;
		} else {
			usage(prog, d);
			return 2;
		}
	}

	/* Argument */
	if (argc == 0)
		return 0;
	usage(prog, d);
	return 2;
}

#ifdef CPUS_DEFINED
cpus_t *read_affinity(const char *name)
{
	int p, *q;
	cpus_t *r;

	r = cpus_create(NR_CPUS, name);
	for (p = 0, q = r->cpu; p < NR_CPUS; p++) {
		if (C(p) & CPU_ALL)
			*q++ = p;
	}
	return r;
}

/* Attempt to force processors wake up, on devices where unused procs
 * go to sleep...
 */
#ifdef FORCE_AFFINITY
cpus_t *read_force_affinity(int n_avail, int verbose, const char *name)
{
	cpus_t *r;

	r = read_affinity(name);
	if (n_avail <= r->sz)
		return r;
	if (verbose) {
		litmus_log("Read affinity: '");
		cpus_dump(false, r);
		litmus_log("'\n");
	}
	cpus_free(r, name);
	return NULL;
}
#endif
#endif

bh_t litmus_bh;
litmus_evt_t litmus_event;
litmus_sta_t litmus_state;

#ifdef CONFIG_TEST_LITMUS_DEBUG
static void litmus_dbg_evt(litmus_evt_t event)
{
	switch (event) {
	case LITMUS_EVT_OPEN:
		printf("E: OPEN\n");
		break;
	case LITMUS_EVT_CLOSE:
		printf("E: CLOSE\n");
		break;
	case LITMUS_EVT_RUN_START:
		printf("E: RUN START\n");
		break;
	case LITMUS_EVT_RUN_STOP:
		printf("E: RUN STOP\n");
		break;
	default:
		printf("E: UNKNOWN\n");
		break;
	}
}

static void litmus_dbg_sta(litmus_sta_t state)
{
	switch (state) {
	case LITMUS_STA_IDLE:
		printf("S: IDLE\n");
		break;
	case LITMUS_STA_BUSY:
		printf("S: BUSY\n");
		break;
	default:
		printf("S: UNKNOWN\n");
		break;
	}
}
#else
#define litmus_dbg_evt(event)		do { } while (0)
#define litmus_dbg_sta(state)		do { } while (0)
#endif

void litmus_raise(litmus_evt_t event)
{
	if (!(litmus_event & event)) {
		litmus_event |= event;
		litmus_dbg_evt(event);
		bh_resume(litmus_bh);
	}
}

void litmus_enter(litmus_sta_t state)
{
	if (litmus_state != state) {
		litmus_state = state;
		litmus_dbg_sta(state);
	}
}

#ifdef CONFIG_TEST_LITMUS_DEBUG
#define litmus_invoke(step)			\
	do {					\
		printf(__stringify(step) "\n");	\
		step();				\
	} while (0)
#else
#define litmus_invoke(step)			\
	do {					\
		step();				\
	} while (0)
#endif

static void litmus_handler(uint8_t __event)
{
	uint32_t event = litmus_event;

	litmus_event = 0;
	switch (litmus_state) {
	case LITMUS_STA_IDLE:
		if (event & LITMUS_EVT_OPEN) {
			litmus_invoke(litmus_start);
			litmus_enter(LITMUS_STA_BUSY);
			litmus_raise(LITMUS_EVT_RUN_START);
		}
		break;
	case LITMUS_STA_BUSY:
		if (event & LITMUS_EVT_RUN_START) {
			litmus_invoke(litmus_run_start);
			litmus_raise(LITMUS_EVT_RUN_STOP);
		}
		if (event & LITMUS_EVT_RUN_STOP) {
			litmus_invoke(litmus_run_stop);
			if (litmus_closed())
				litmus_raise(LITMUS_EVT_CLOSE);
			else
				litmus_raise(LITMUS_EVT_RUN_START);
		}
		if (event & LITMUS_EVT_CLOSE) {
			litmus_invoke(litmus_stop);
			litmus_enter(LITMUS_STA_IDLE);
		}
		break;
	}
}

void litmus_launch(void)
{
	litmus_raise(LITMUS_EVT_OPEN);
	do {
		bh_sync();
	} while (litmus_state != LITMUS_STA_IDLE);
}

void litmus_exec(const char *test)
{
	struct cpu_exec_test *fn;

	fn = bench_test_find(test);
	if (!fn) {
		litmus_fatal(test);
		litmus_run_stop();
		litmus_stop();
		litmus_enter(LITMUS_STA_IDLE);
		return;
	}
	bench_simple(CPU_ALL, fn, true);
}

void litmus_init(void)
{
	litmus_event = 0;
	litmus_state = LITMUS_STA_IDLE;
	litmus_bh = bh_register_handler(litmus_handler);
}
