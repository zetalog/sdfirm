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

void fatal(char *msg)
{
	log_error("Failure: %s\n", msg);
	fprintf(stdout, "Failure: %s\n", msg);
	BUG();
}

void errexit(char *msg, int err)
{
	log_error("%s: %d\n", msg, err);
	BUG();
}

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

void *malloc_check(size_t sz)
{
	void *p;

	if (sz == 0) return NULL ;
	p = malloc(sz) ;
	if (!p) {
		if (!errno) errno = ENOMEM ;
		BUG();
	}
	return p ;
}

void pp_ints(FILE *fp, int *p, int n)
{
	int k;

	if (n > 0) {
		fprintf(fp, "%i", p[0]);
		for (k = 1; k < n; k++) {
			fprintf(fp, ",%i", p[k]);
		}
	}
}

void ints_dump(FILE *fp, ints_t *p)
{
	int k;

	if (p->sz > 0) {
		fprintf(fp, "%i:%i", 0, p->t[0]);
		for (k = 1; k < p->sz; k++) {
			fprintf(fp, ",%i:%i", k, p->t[k]);
		}
	}
}

/************/
/* CPU sets */
/************/

cpus_t *cpus_create(int sz)
{
	cpus_t *r = malloc_check(sizeof(*r)) ;
	r->sz = sz ;
	r->cpu = malloc_check(sizeof(r->cpu[0])*sz)  ;
	return r ;
}

void cpus_free(cpus_t *p)
{
	free(p->cpu);
	free(p);
}

void cpus_dump(FILE *fp, cpus_t *p)
{
	pp_ints(fp, p->cpu, p->sz);
}

void cpus_dump_test(FILE *fp, int *p, int sz, cpus_t *cm, int nprocs)
{
	int k, i;

	for (k = 0 ; k < sz ; k += nprocs) {
		fprintf(fp, "[");
		pp_ints(fp, &p[k], nprocs);
		fprintf(fp, "] {");
		if (nprocs > 0) {
			fprintf(fp, "%i", cm->cpu[p[k]]);
			for (i = 1 ; i < nprocs; i++) {
				fprintf(fp, ",%i", cm->cpu[p[k+i]]);
			}
		}
		fprintf(fp, "}\n");
	}
}

cpus_t *coremap_seq(int navail, int nways)
{
	cpus_t *r = cpus_create(navail);
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
	cpus_t **core;
} mapcore_t ;

static void mapcore_free(mapcore_t *p)
{
	int c;

	for (c = 0; c < p->ncores; c++)
		cpus_free(p->core[c]);
	free(p->core);
	free(p);
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

cpus_t *get_core_procs(cpus_t *cm, cpus_t *p,int c)
{
	int sz = 0;
	int k, i;
	cpus_t *r;
	int proc;

	for (k = 0; k < p->sz; k++) {
		if (cm->cpu[p->cpu[k]] == c)
		       sz++;
	}
	r = cpus_create(sz);
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
	int ncores = get_ncores(cm) ;
	mapcore_t *r = malloc_check(sizeof(*r));

	r->ncores = ncores;
	r->core = malloc_check(sizeof(r->core[0])*ncores);
	for (c = 0; c < ncores; c++) {
		r->core[c] = get_core_procs(cm,p,c);
	}
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
	} while (k != k0) ;
	if (found < 0)
		fatal("Cannot allocate threads");
	return found;
}

void custom_affinity(st_t *st, cpus_t *cm, int **color, int *diff,
		     cpus_t *aff_cpus, int n_exe, int *r)
{
	mapcore_t *mc = inverse_procs(cm,aff_cpus);
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
	outs_t *r = malloc_check(sizeof(*r));
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
	free(p);
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

void dump_outs(FILE *chan, dump_outcome *dout, outs_t *p,
	       intmax_t *buff, int sz)
{
	for (; p; p = p->next) {
		buff[sz-1] = p->k;
		if (p->c > 0) {
			dout(chan,buff,p->c,p->show);
		} else if (p->down) {
			dump_outs(chan, dout, p->down, buff, sz-1);
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

void prefetch_dump(FILE *fp, prfdirs_t *p)
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
					fprintf(fp, ",");
				} else {
					some = 1;
				}
				fprintf(fp, "%i:%s=%c", _p, r[_v].name, c);
			}
		}
	}
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
			return 0 ;
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
			log_error("Proc %i does not access variable %s\n",
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
		if (c == '\0') return 1;
		else if (c == ',') p++;
		else return 0;
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

void perm_cpus(unsigned *st, cpu_exec_cpu_t thread[], int n)
{
	int k;
	int j;
	cpu_exec_cpu_t t;

	for (k = 0 ; k < n-1 ; k++) {
		j = k + rand_k(st, n - k);
		t = thread[j];
		thread[j] = thread[k]; thread[k] = t;
	}
}

/*************************/
/* Concurrency utilities */
/*************************/

/* phread based mutex */
pm_t *pm_create(void)
{
	pm_t *p = malloc_check(sizeof(*p));
	spin_lock_init(p);
	return p;
}

void pm_free(pm_t *p)
{
	free(p);
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
	pb_t *p = malloc_check(sizeof(*p));
	p->cond = pm_create();
	p->count = p->nprocs = nprocs;
	p->turn = 0;
	return p;
}

void pb_free(pb_t *p)
{
	pm_free(p->cond);
	free(p);
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
		smp_wmb();
	} else {
		do {
			smp_rmb();
		} while (p->turn == t);
	}
	pm_unlock(p->cond);
}

/****************/
/* Command line */
/****************/

static void usage(char *prog, cmd_t *d)
{
	log_error("usage: %s (options)*\n", prog);
	log_error("  -v      be verbose\n");
	log_error("  -q      be quiet\n");
	log_error("  -a <n>  run maximal number of tests for n available processors (default %i)\n",
		  d->avail);
	log_error("  -n <n>  run n tests concurrently\n");
	log_error("  -r <n>  perform n runs (default %i)\n", d->max_run);
	log_error("  -fr <f> multiply run number per f\n");
	log_error("  -s <n>  outcomes per run (default %i)\n",
		  d->size_of_test);
	if (d->stride > 0) {
		log_error("  -st <n> stride (default %i)\n", d->stride);
	}
	log_error("  -fs <f> multiply outcomes per f\n") ;
	log_error("  -f <f>  multiply outcomes per f, divide run number by f\n") ;
	if (d->aff_mode != aff_none) {
		log_error("  -i <n>  increment for allocating logical processors, -i 0 disables affinity mode");
		if (d->aff_mode == aff_incr) {
			log_error(" (default %i)\n", d->aff_incr);
		} else {
			log_error("\n");
		}
		log_error("  -p <ns> specify logical processors (default '");
		cpus_dump(errlog, d->aff_cpus);
		log_error("')\n");
		log_error("  +ra     randomise affinity%s\n",
			  d->aff_mode == aff_random ? " (default)" : "");
		if (d->aff_custom_enabled) {
			log_error("  +ca     enable custom affinity%s\n",
				  d->aff_mode == aff_custom ?
				  " (default)" : "");
		} else {
			log_error("  +ca     alias for +ra\n");
		}
		if (d->aff_scan_enabled) {
			log_error("  +sa     enable scanning affinity%s\n",
				  d->aff_mode == aff_scan ?
				  " (default)" : "") ;
			log_error("  +ta <topo> set topology affinity\n") ;
		} else {
			log_error("  +sa     alias for +ra\n");
		}
	}
	if (d->shuffle >= 0) {
		log_error("  +rm     randomise memory accesses%s\n",
			  d->shuffle ? " (default)" : "");
		log_error("  -rm     do not randomise memory accesses%s\n",
			  !d->shuffle ? " (default)" : "");
	}
	if (d->speedcheck >= 0) {
		log_error("  +sc     stop as soon as possible%s\n",
			  d->speedcheck ? " (default)" : "");
		log_error("  -sc     run test completly%s\n",
			  !d->speedcheck ? " (default)" : "");
	}
	if (!d->fix) {
		log_error("  +fix    fix thread launch order\n");
	}
	if (d->delta_tb) {
		log_error("  -tb <list> set timebase delays, default '");
		ints_dump(errlog, d->delta_tb);
		log_error("'\n");
		log_error("    List syntax is comma separated proc:delay\n");
		log_error("  -ta <n>    set all timebase delays\n");
	}
	if (d->verbose_barrier >= 0) {
		log_error("  +vb     show iteration timings%s\n",
			  d->verbose_barrier ? " (default)" : "");
		log_error("  -vb     do not show iteration timings%s\n",
			  !d->verbose_barrier ? " (default)" : "");
	}
	if (d->prefetch) {
		log_error("  -pra (I|F|T|W) set all prefetch\n");
		log_error("  -prf <list> set prefetch, default '");
		prefetch_dump(errlog,d->prefetch);
		log_error("'\n");
		log_error("    List syntax is comma separated proc:name=(I|F|T|W)\n");
	}
	if (d->static_prefetch >= 0) {
		log_error("  -prs <n> prefetch probability is 1/n, -prs 0 disables feature, default %i\n",
			  d->static_prefetch);
	}
	if (d->max_loop > 0) {
		log_error("  -l <n>  measure time by running assembly in a loop of size <n> (default %i)\n",
			  d->max_loop);
	}
	if (d->prelude > 0) {
		log_error("  -vp     no verbose prelude\n");
	}
	if (d->sync_n > 0) {
		log_error("  -k <n>  undocumented (default %i)\n",
			  d->sync_n);
	}
	exit(2);
}

static long my_add (long x, long y)
{
	long r = x + y;

	if (r < x || r < y) {
		errno = ERANGE;
	       	fatal("overflow");
	}
	return r;
}

static long my_pow10(int p, long x)
{
	long r = x ;

	for (; p > 0; p--) {
		long y2 = my_add(r, r);
		long y4 = my_add(y2, y2);
		long y8 = my_add(y4, y4);
		r = my_add(y8, y2) ;
	}
	if (r >= INT_MAX || r <= 0) {
		errno = ERANGE;
		fatal("overflow");
	}
	return r;
}

static int do_argint(char *p, char **q)
{
	long r = strtoul(p, q, 10);

	if (errno == ERANGE) {
		fatal("overflow");
	}
	if (**q == 'k' || **q == 'K') {
		r = my_pow10(3, r);
	       	*q += 1;
	} else if (**q == 'm' || **q == 'M') {
		r = my_pow10(6, r);
		*q += 1;
	}
	return (int)r;
}

static int argint(char *prog, char *p, cmd_t *d)
{
	char *q ;
	long r = do_argint(p, &q);

	if (*p == '\0' || *q != '\0') {
		usage(prog, d);
	}
	return (int)r;
}

static cpus_t *argcpus(char *prog, char *p0, cmd_t *d)
{
	int sz = 0;
	char *p;
	cpus_t *r;
	int k;

	p = p0;
	for (; ; ) {
		char *q;
		int x = (int)strtoul(p, &q, 10);
		if (x < 0 || *p == '\0' || (*q != '\0' && *q != ','))
			usage(prog, d);
		sz++;
		if (*q == '\0') break;
		p = q + 1;
	}
	r = cpus_create(sz);
	p = p0;
	for (k = 0; k < sz; k++) {
		char *q;
		r->cpu[k] = (int)strtoul(p, &q, 10);
		p = q + 1;
	}
	return r;
}

static void argints(char *prog, cmd_t *d, char *p, ints_t *r)
{
	while (*p) {
		char *q ;
		int idx = (int)strtoul(p, &q, 10);
		int v;

		if (idx < 0 || idx >= r->sz || *p == '\0' || *q != ':')
			usage(prog, d);
		p = q + 1;
		v = do_argint(p, &q);
		if (*p == '\0' || (*q != '\0' && *q != ','))
			usage(prog, d);
		r->t[idx] = v;
		if (*q == '\0') {
			p = q;
		} else {
			p = q + 1;
		}
	}
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

static void argprefetch(char *prog, cmd_t *d, char *p, prfdirs_t *r)
{
	if (!parse_prefetch(p, r))
		usage(prog, d);
}

static double argdouble(char *prog,char *p,cmd_t *d)
{
	char *q ;
	double r = strtod(p, &q);

	if (*p == '\0' || *q != '\0') {
		usage(prog,d);
	}
	return r;
}

void parse_cmd(int argc, char **argv, cmd_t *d, cmd_t *p)
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
			if (!*argv)
				usage(prog, d);
			p->max_run = argint(prog, argv[0], d);
		} else if (strcmp(*argv, "-fr") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			p->max_run *= argdouble(prog, argv[0], d);
		} else if (strcmp(*argv, "-s") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			p->size_of_test = argint(prog, argv[0], d);
		} else if (d->stride > 0 && strcmp(*argv,"-st") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			p->stride = argint(prog, argv[0], d);
			if (p->stride <= 0)
				p->stride = 0;
		} else if (strcmp(*argv, "-fs") == 0) {
			--argc; ++argv;
			if (!*argv) usage(prog, d);
			p->size_of_test *= argdouble(prog,argv[0], d);
		} else if (strcmp(*argv, "-f") == 0) {
			double f;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			f = argdouble(prog, argv[0], d);
			p->size_of_test *= f;
			p->max_run /= f;
		} else if (strcmp(*argv, "-n") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			p->n_exe = argint(prog, argv[0], d);
			if (p->n_exe < 1)
				p->n_exe = 1;
		} else if (strcmp(*argv, "-a") == 0) {
			int a;

			--argc; ++argv;
			if (!*argv) usage(prog, d);
			a = argint(prog, argv[0], d);
			p->avail = a;
		} else if (d->sync_n > 0 && strcmp(*argv, "-k") == 0) {
			int a;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			a = argint(prog, argv[0], d);
			p->sync_n = a < 0 ? 0 : a;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "-i") == 0) {
			int i;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			i = argint(prog,argv[0], d);
			p->aff_mode = aff_incr;
			p->aff_incr = i < 0 ? 0 : i;
		} else if (d->aff_mode != aff_none &&
			   strcmp(*argv, "-p") == 0) {
			cpus_t *cpus;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			cpus = argcpus(prog, argv[0], d);
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
			if (!*argv)
				usage(prog, d);
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
			if (!*argv)
				usage(prog, d);
			argints(prog, d, argv[0], p->delta_tb);
		} else if (d->delta_tb && strcmp(*argv,"-ta") == 0) {
			int da;
			int k;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			da = argint(prog, argv[0], d);
			for (k = 0; k < p->delta_tb->sz; k++)
				p->delta_tb->t[k] = da;
		} else if (d->prefetch && strcmp(*argv, "-prf") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			argprefetch(prog, d, argv[0], p->prefetch);
		} else if (d->prefetch && strcmp(*argv,"-pra") == 0) {
			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			argoneprefetch(prog, d, argv[0], p->prefetch);
		} else if (d->static_prefetch >= 0 &&
			   strcmp(*argv, "-prs") == 0) {
			int prs;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			prs = argint(prog, argv[0], d);
			p->static_prefetch = prs >= 0 ? prs : 0;
		} else if (d->max_loop > 0 && strcmp(*argv, "-l") == 0) {
			int i;

			--argc; ++argv;
			if (!*argv)
				usage(prog, d);
			i = argint(prog, argv[0], d);
			p->max_loop = i < 1 ? 1 : i;
		} else
			usage(prog, d);
	}

	/* Argument */
	if (argc == 0) return;
	usage(prog, d);
}
