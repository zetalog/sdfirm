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
#ifndef _UTILS_H
#define _UTILS_H 1

#include <target/generic.h>
#include <target/arch.h>
#include <target/spinlock.h>
#include <target/bench.h>
#include <target/heap.h>
#include <target/panic.h>
#include <time.h>

typedef long int		intmax_t;
#define malloc(sz)		heap_alloc(sz)
#define free(ptr)		heap_free(ptr)
#ifdef CONFIG_TEST_VERBOSE
#define fprintf(f, ...)		printf(__VA_ARGS__)
#define vfprintf(f, fmt, args)	vprintf(fmt, args)
#else
#define fprintf(f, ...)		do { } while (0)
#define vfprintf(f, fmt, args)	do { } while (0)
#endif
#define fflush(f)		do { } while (0)
#define fopen(p, m)		((FILE *)-1)
#define fclose(f)		do { } while (0)
#define EXIT_SUCCESS		CPU_EXEC_SUCCESS
#define exit(ecode)		do { } while (0)
#define timeofday()		clock()

/* type of state for pseudorandom  generators */
typedef uint32_t st_t;
/* 64bit counters, should be enough! */
typedef uint64_t count_t;
typedef tsc_count_t tsc_t;

typedef void* f_t(void *);
typedef void dump_outcome(FILE *chan, intmax_t *o, count_t c, int show);

#define PCTR "%p"
#define PTSC "%llu"

/**********/
/* Random */
/**********/

/* Unlocked random bit */
int rand_bit(st_t *st);
uint32_t rand_k(st_t *st, uint32_t n);

/********/
/* Misc */
/********/

#define errlog			stderr
#define log_error(...)		fprintf(errlog, __VA_ARGS__)
void fatal(char *msg);
/* e is errno */
void errexit(char *msg, int e);
int gcd(int a, int b);
int find_string(char *t[], int sz, char *s);
void *malloc_check(size_t sz);
void pp_ints(FILE *fp, int *p, int n);

/***********/
/* CPU set */
/***********/

#define CPUS_DEFINED 1
typedef struct {
	int sz;
	int *cpu;
} cpus_t;

cpus_t *cpus_create(int sz);
void cpus_free(cpus_t *p);
void cpus_dump(FILE *fp, cpus_t *p);
void cpus_dump_test(FILE *fp, int *p, int sz, cpus_t *cm, int nprocs);

cpus_t *coremap_seq(int navail, int nways);
void custom_affinity(st_t *st, cpus_t *cm, int **color, int *diff,
		     cpus_t *aff_cpus, int n_exe, int *r);

/*************/
/* Int array */
/*************/

typedef struct {
	int sz;
	int *t;
} ints_t;

void ints_dump(FILE *fp, ints_t *p);

/************************/
/* Histogram structure  */
/************************/

typedef struct outs_t {
	struct outs_t *next, *down;
	count_t c;
	intmax_t k;
	int show;
} outs_t ;

void free_outs(outs_t *p);
outs_t *add_outcome_outs(outs_t *p, intmax_t *o, int sz, count_t v, int show);
int finals_outs(outs_t *p);
count_t sum_outs(outs_t *p);
void dump_outs(FILE *chan, dump_outcome *dout, outs_t *p,
	       intmax_t *buff, int sz) ;
outs_t *merge_outs(outs_t *p,outs_t *q, int sz);

/* Prefetch directives */
typedef enum {
	none,
	flush,
	touch,
	touch_store
} prfdir_t;

typedef struct {
	char *name;
	prfdir_t dir;
} prfone_t;

typedef struct {
	int nvars;
	prfone_t *t;
} prfproc_t;

typedef struct {
	int nthreads;
	prfproc_t *t;
} prfdirs_t;

void prefetch_dump(FILE *fp, prfdirs_t *p);
int parse_prefetch(char *p, prfdirs_t *r);
void set_prefetch(prfdirs_t *p, prfdir_t d);

/*****************/
/* Random things */
/*****************/

/* permutations */
void perm_prefix_ints(unsigned *st, int *_t, int m, int n);
void perm_funs(st_t *st, f_t *t[], int sz);
void perm_cpus(st_t *st, cpu_exec_cpu_t t[], int sz) ;

/********************/
/* Thread utilities */
/********************/

/* Lock */
typedef spinlock_t pm_t;

pm_t *pm_create(void);
void pm_free(pm_t *p);
void pm_lock(pm_t *m);
void pm_unlock(pm_t *m);

/* Barrier */
typedef struct {
	volatile unsigned int count;
	volatile int turn;
	pm_t *cond;
	unsigned int nprocs;
} pb_t;

pb_t *pb_create(int nprocs);
void pb_free(pb_t *p);
void pb_wait(pb_t *p);

/************************/
/* Command line options */
/************************/
typedef enum
  { aff_none, aff_incr, aff_random, aff_custom,
    aff_scan, aff_topo} aff_mode_t ;

typedef struct {
  int verbose ;
  /* Test parmeters */
  int max_run ;
  int size_of_test ;
  int stride ;
  int avail ;
  int n_exe ;
  int sync_n ;
  /* Affinity */
  aff_mode_t aff_mode ;
  int aff_custom_enabled ;
  int aff_scan_enabled ;
  int aff_incr ;
  cpus_t *aff_cpus ;
  char *aff_topo ;
  /* indirect mode */
  int shuffle ;
  /* loop test */
  int max_loop ;
  /* time base delays */
  ints_t * delta_tb ;
  /* prefetch control */
  prfdirs_t *prefetch ;
  int static_prefetch ;
  /* show time of synchronisation */
  int verbose_barrier ;
  /* Stop as soon as condition is settled */
  int speedcheck ;
  /* Enforce fixed launch order (ie cancel change lauch) */
  int fix ;
  /* Dump prelude to test output */
  int prelude ;
} cmd_t ;

void parse_cmd(int argc, char **argv, cmd_t *def, cmd_t *p) ;

/* Thread launch and join */

void launch(cpu_exec_cpu_t *th, f_t *f, void *a) ;
void *join(cpu_exec_cpu_t *th) ;
cpus_t *read_force_affinity(int n_avail, int verbose);

#endif
