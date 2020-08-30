/****************************************************************************/
/*                           the diy toolsuite                              */
/*                                                                          */
/* Jade Alglave, University College London, UK.                             */
/* Luc Maranget, INRIA Paris-Rocquencourt, France.                          */
/*                                                                          */
/* This C source is a product of litmus7 and includes source that is        */
/* governed by the CeCILL-B license.                                        */
/****************************************************************************/

/* Parameters */
#define SIZE_OF_TEST 100
#define NUMBER_OF_RUN 10
#define AVAIL 4
#define STRIDE 1
#define MAX_LOOP 0
#define N 2
#define AFF_INCR (1)
/* Includes */
#include <target/litmus.h>
#include <target/bh.h>
#include <target/cmdline.h>

/* params */
typedef struct {
  int verbose;
  int size_of_test,max_run;
  int stride;
  aff_mode_t aff_mode;
  int ncpus, ncpus_used;
  int do_change;
  cpus_t *cm;
} param_t;


/* Full memory barrier */

inline static void mbar(void) {
#ifdef CONFIG_RISCV
  asm __volatile__ ("fence rw,rw" ::: "memory");
#else
  asm __volatile__ ("mfence" ::: "memory");
#endif
}

/* Barriers macros */
inline static void barrier_wait(unsigned int id, unsigned int k, int volatile *b) {
  if ((k % N) == id) {
    *b = 1 ;
    mbar();
  } else {
    while (*b == 0) ;
  }
}

/*
 Topology: {{{0, 1}, {2, 3}}}
*/

static int cpu_scan[] = {
// [[0],[1]]
2, 0, 3, 1,
// [[0,1]]
2, 3, 0, 1,
};

static char *group[] = {
"[[0],[1]]",
"[[0,1]]",
};

#define SCANSZ 2
#define SCANLINE 4

static count_t g_ngroups[SCANSZ];

/**********************/
/* Context definition */
/**********************/


typedef struct {
/* Shared variables */
  int *y;
  int *x;
/* Final content of observed  registers */
  int *out_1_x5;
  int *out_1_x7;
/* Check data */
  pb_t *fst_barrier;
/* Barrier for litmus loop */
  int volatile *barrier;
/* Instance seed */
  st_t seed;
/* Parameters */
  param_t *_p;
} ctx_t;

inline static int final_cond(int _out_1_x5,int _out_1_x7) {
  switch (_out_1_x5) {
  case 1:
    switch (_out_1_x7) {
    case 0:
      return 1;
    default:
      return 0;
    }
  default:
    return 0;
  }
}

inline static int final_ok(int cond) {
  return cond;
}

/**********************/
/* Outcome collection */
/**********************/
#define NOUTS 2
typedef intmax_t outcome_t[NOUTS];

static const int out_1_x5_f = 0 ;
static const int out_1_x7_f = 1 ;


typedef struct hist_t {
  outs_t *outcomes;
  count_t n_pos,n_neg;
} hist_t ;

static hist_t *alloc_hists(int sz, const char *name)
{
  hist_t *p;
  p = malloc_check(sizeof(hist_t) * sz, "g_hists");
  for (int k=0; k<sz; k++) {
    p[k].outcomes = NULL;
    p[k].n_pos = p[k].n_neg = 0;
  }
  return p;
}

static void free_hists(int sz, hist_t *p, const char *name)
{
  for (int k=0; k<sz; k++)
    free_outs(p[k].outcomes);
  free_check(p, name);
}

static void add_outcome(hist_t *h, count_t v, outcome_t o, int show) {
  h->outcomes = add_outcome_outs(h->outcomes,o,NOUTS,v,show);
}

static void merge_hists(hist_t *h0, hist_t *h1) {
  h0->n_pos += h1->n_pos ;
  h0->n_neg += h1->n_neg ;
  h0->outcomes = merge_outs(h0->outcomes,h1->outcomes,NOUTS) ;
}


static void do_dump_outcome(FILE *fhist, intmax_t *o, count_t c, int show) {
  fprintf(fhist,"%-6"PCTR"%c>1:x5=%i; 1:x7=%i;\n",c,show ? '*' : ':',(int)o[out_1_x5_f],(int)o[out_1_x7_f]);
}

static void just_dump_outcomes(FILE *fhist, hist_t *h) {
  outcome_t buff ;
  dump_outs(fhist,do_dump_outcome,h->outcomes,buff,NOUTS) ;
}

/**************************************/
/* Prefetch (and check) global values */
/**************************************/

static void check_globals(ctx_t *_a) {
  int *y = _a->y;
  int *x = _a->x;
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    if (rand_bit(&(_a->seed)) && y[_i] != 0) fatal("MP, check_globals failed");
    if (rand_bit(&(_a->seed)) && x[_i] != 0) fatal("MP, check_globals failed");
  }
  pb_wait(_a->fst_barrier);
}

/***************/
/* Litmus code */
/***************/

typedef struct {
  int th_id; /* I am running on this thread */
  int *cpu; /* On this cpu */
  ctx_t *_a;   /* In this context */
} parg_t;

typedef struct {
  pm_t *p_mutex;
  pb_t *p_barrier;
  param_t *_p;
  int z_id;
  int *cpus;
} zyva_t;

#define NT N

static tsc_t g_start;
static param_t prm;
static cmd_t g_def = {
  0, NUMBER_OF_RUN, SIZE_OF_TEST, STRIDE, AVAIL, 0, 0,
  aff_incr, 1, 1, AFF_INCR, NULL, NULL, -1, MAX_LOOP,
  NULL, NULL, -1, -1, -1, 0, 1
};
static cmd_t g_cmd;
static int g_max_exe;
static int g_n_th;
static int g_next_cpu;
static int g_delta;
static int g_start_scan;
static int g_max_start;
static int *g_aff_cpu;
static zyva_t *g_zarg;

static cpus_t *g_def_all_cpus;
static cpus_t *g_all_cpus;
static int g_n_aff_cpus;
static int *g_aff_cpus;
static zyva_t *g_zargs;
static hist_t *g_hists;
static hist_t *g_hist;
static pm_t *g_mutex;
static pb_t *g_barrier;

static parg_t g_parg[N];
static ctx_t g_ctx;
static int g_n_run;
static int g_n_exe;

static void *P0(void *_vb) {
  mbar();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
  int _ecpu = _b->cpu[_b->th_id];
  /* force_one_affinity(_ecpu,AVAIL,_a->_p->verbose,"MP"); */
  check_globals(_a);
  int _th_id = _b->th_id;
  int volatile *barrier = _a->barrier;
  int _size_of_test = _a->_p->size_of_test;
  int _stride = _a->_p->stride;
  for (int _j = _stride ; _j > 0 ; _j--) {
    for (int _i = _size_of_test-_j ; _i >= 0 ; _i -= _stride) {
      barrier_wait(_th_id,_i,&barrier[_i]);
#ifdef CONFIG_RISCV
asm __volatile__ (
"\n"
"#START _litmus_P0\n"
"#_litmus_P0_0\n\t"
"sw %[x5],0(%[x6])\n"
"#_litmus_P0_1\n\t"
"sw %[x5],0(%[x7])\n"
"#END _litmus_P0\n\t"
:
:[x5] "r" (1),[x6] "r" (&_a->x[_i]),[x7] "r" (&_a->y[_i])
:"cc","memory"
);
#else
      _a->x[_i] = 1;
      _a->y[_i] = 1;
#endif
    }
  }
  mbar();
  return NULL;
}

static void *P1(void *_vb) {
  mbar();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
  int _ecpu = _b->cpu[_b->th_id];
  /* force_one_affinity(_ecpu,AVAIL,_a->_p->verbose,"MP"); */
  check_globals(_a);
  int _th_id = _b->th_id;
  int volatile *barrier = _a->barrier;
  int _size_of_test = _a->_p->size_of_test;
  int _stride = _a->_p->stride;
  int *out_1_x5 = _a->out_1_x5;
  int *out_1_x7 = _a->out_1_x7;
  for (int _j = _stride ; _j > 0 ; _j--) {
    for (int _i = _size_of_test-_j ; _i >= 0 ; _i -= _stride) {
      barrier_wait(_th_id,_i,&barrier[_i]);
#ifdef CONFIG_RISCV
asm __volatile__ (
"\n"
"#START _litmus_P1\n"
"#_litmus_P1_0\n\t"
"lw %[x5],0(%[x6])\n"
"#_litmus_P1_1\n\t"
"lw %[x7],0(%[x8])\n"
"#END _litmus_P1\n\t"
:[x7] "=&r" (out_1_x7[_i]),[x5] "=&r" (out_1_x5[_i])
:[x6] "r" (&_a->y[_i]),[x8] "r" (&_a->x[_i])
:"cc","memory"
);
#else
      out_1_x5[_i] = _a->y[_i];
      out_1_x7[_i] = _a->x[_i];
#endif
    }
  }
  mbar();
  return NULL;
}

/*******************************************************/
/* Context allocation, freeing and reinitialization    */
/*******************************************************/

static void init(ctx_t *_a) {
  int size_of_test = _a->_p->size_of_test;

  _a->seed = rand();
  _a->out_1_x5 = malloc_check(size_of_test*sizeof(*(_a->out_1_x5)), "ctx->out_1_x5");
  _a->out_1_x7 = malloc_check(size_of_test*sizeof(*(_a->out_1_x7)), "ctx->out_1_x7");
  _a->y = malloc_check(size_of_test*sizeof(*(_a->y)), "ctx->y");
  _a->x = malloc_check(size_of_test*sizeof(*(_a->x)), "ctx->x");
  _a->fst_barrier = pb_create(N);
  _a->barrier = malloc_check(size_of_test*sizeof(*(_a->barrier)), "ctx->barrier");
}

static void finalize(ctx_t *_a) {
  free_check((void *)_a->y, "ctx->y");
  free_check((void *)_a->x, "ctx->x");
  free_check((void *)_a->out_1_x5, "ctx->out_1_x5");
  free_check((void *)_a->out_1_x7, "ctx->out_2_x5");
  pb_free(_a->fst_barrier);
  free_check((void *)_a->barrier, "ctx->barrier");
}

static void reinit(ctx_t *_a) {
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    _a->y[_i] = 0;
    _a->x[_i] = 0;
    _a->out_1_x5[_i] = -239487;
    _a->out_1_x7[_i] = -239487;
    _a->barrier[_i] = 0;
  }
}

static f_t *g_fun[] = {&P0,&P1};

static void zyva(void) {
#if 0
  cpu_exec_cpu_t thread[NT];

  for (int _p = NT-1 ; _p >= 0 ; _p--) {
    launch(&thread[_p],g_fun[_p],&g_parg[_p]);
  }
  if (_b->do_change) perm_cpus(&g_ctx.seed,thread,NT);
  for (int _p = NT-1 ; _p >= 0 ; _p--) {
    join(&thread[_p]);
  }
#endif
}

#ifdef ASS
static void ass(FILE *out) { }
#else
#include "foobar.h"
#endif

static void prelude(FILE *out) {
  fprintf(out,"%s\n","%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  fprintf(out,"%s\n","% Results for ./tests/dummy/foobar.litmus %");
  fprintf(out,"%s\n","%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  fprintf(out,"%s\n","RISCV MP");
  fprintf(out,"%s\n","\"PodWW Rfe PodRR Fre\"");
  fprintf(out,"%s\n","{0:x5=1; 0:x6=x; 0:x7=y; 1:x6=y; 1:x8=x;}");
  fprintf(out,"%s\n"," P0          | P1          ;");
  fprintf(out,"%s\n"," sw x5,0(x6) | lw x5,0(x6) ;");
  fprintf(out,"%s\n"," sw x5,0(x7) | lw x7,0(x8) ;");
  fprintf(out,"%s\n","");
  fprintf(out,"%s\n","exists (1:x5=1 /\\ 1:x7=0)");
  fprintf(out,"Generated assembler\n");
  ass(out);
}

static int color_0[] = {0, -1};
static int color_1[] = {1, -1};
static int *color[] = {color_0, color_1, NULL};
static int diff[] = {1, 0, -1};

#define ENOUGH 10

static void postlude(FILE *out,cmd_t *cmd,hist_t *hist,count_t p_true,count_t p_false,tsc_t total) {
  fprintf(out,"Test MP Allowed\n");
  fprintf(out,"Histogram (%i states)\n",finals_outs(hist->outcomes));
  just_dump_outcomes(out,hist);
  __unused int cond = p_true > 0;
  fprintf(out,"%s\n",cond?"Ok":"No");
  fprintf(out,"\nWitnesses\n");
  fprintf(out,"Positive: %" PCTR ", Negative: %" PCTR "\n",p_true,p_false);
  fprintf(out,"Condition %s is %svalidated\n","exists (1:x5=1 /\\ 1:x7=0)",cond ? "" : "NOT ");
  fprintf(out,"Hash=2939da84098a543efdbb91e30585ab71\n");
  fprintf(out,"Cycle=Rfe PodRR Fre PodWW\n");
  fprintf(out,"Relax MP %s %s\n",p_true > 0 ? "Ok" : "No","");
  fprintf(out,"Safe=Rfe Fre PodWW PodRR\n");
  fprintf(out,"Generator=diy7 (version 7.51+4(dev))\n");
  fprintf(out,"Com=Rf Fr\n");
  fprintf(out,"Orig=PodWW Rfe PodRR Fre\n");
  if (cmd->aff_mode == aff_custom) {
    fprintf(out,"Affinity=[0] [1] ; (1,0)\n");
  }
  __unused count_t cond_true = p_true;
  __unused count_t cond_false = p_false;
  fprintf(out,"Observation MP %s %" PCTR " %" PCTR "\n",!cond_true ? "Never" : !cond_false ? "Always" : "Sometimes",cond_true,cond_false);
  if (p_true > 0) {
    if (cmd->aff_mode == aff_scan) {
      for (int k = 0 ; k < SCANSZ ; k++) {
        count_t c = g_ngroups[k];
        if ((c*100)/p_true > ENOUGH) { fprintf(out,"Topology %-6" PCTR":> %s\n",c,group[k]); }
      }
    } else if (cmd->aff_mode == aff_topo) {
      fprintf(out,"Topology %-6" PCTR ":> %s\n",g_ngroups[0],cmd->aff_topo);
    }
  }
  fprintf(out,"Time MP %.2f\n",total / 1000000.0);
  fflush(out);
}

static void ctor(FILE *out) {
  if (g_cmd.prelude) prelude(out);
  g_start = timeofday();
/* Set some parameters */
  prm.verbose = g_cmd.verbose;
  prm.size_of_test = g_cmd.size_of_test;
  prm.max_run = g_cmd.max_run;
  prm.stride = g_cmd.stride > 0 ? g_cmd.stride : N ;
  int ntopo = -1;
  if (g_cmd.aff_mode == aff_topo) {
    ntopo = find_string(group,SCANSZ,g_cmd.aff_topo);
    if (ntopo < 0) {
      log_error("Bad topology %s, reverting to scan affinity\n",g_cmd.aff_topo);
      g_cmd.aff_mode = aff_scan; g_cmd.aff_topo = NULL;
    }
  }
  prm.do_change = g_cmd.aff_mode != aff_custom && g_cmd.aff_mode != aff_scan && g_cmd.aff_mode != aff_topo;
  if (g_cmd.fix) prm.do_change = 0;
  prm.cm = coremap_seq(g_def_all_cpus->sz,2,"prm.cm");
/* Computes number of test concurrent instances */
  int n_avail = g_cmd.avail > 0 ? g_cmd.avail : g_cmd.aff_cpus->sz;
  if (n_avail >  g_cmd.aff_cpus->sz) log_error("Warning: avail=%i, available=%i\n",n_avail, g_cmd.aff_cpus->sz);
  if (g_cmd.n_exe > 0) {
    g_max_exe = g_cmd.n_exe;
  } else {
    g_max_exe = n_avail < N ? 1 : n_avail / N;
  }
/* Set affinity parameters */
  g_all_cpus = g_cmd.aff_cpus;
  g_n_aff_cpus = g_cmd.aff_mode == aff_random ? max(g_all_cpus->sz,N*g_max_exe) : N*g_max_exe;
  g_aff_cpus = malloc_check(sizeof(int) * g_n_aff_cpus, "g_aff_cpus");
  prm.aff_mode = g_cmd.aff_mode;
  prm.ncpus = g_n_aff_cpus;
  prm.ncpus_used = N*g_max_exe;
/* Show parameters to user */
  if (prm.verbose) {
    log_error( "MP: n=%i, r=%i, s=%i",g_max_exe,prm.max_run,prm.size_of_test);
    log_error(", st=%i",prm.stride);
    if (g_cmd.aff_mode == aff_incr) {
      log_error( ", i=%i",g_cmd.aff_incr);
    } else if (g_cmd.aff_mode == aff_random) {
      log_error(", +ra");
    } else if (g_cmd.aff_mode == aff_custom) {
      log_error(", +ca");
    } else if (g_cmd.aff_mode == aff_scan) {
      log_error(", +sa");
    }
    log_error(", p='");
    cpus_dump(stderr,g_cmd.aff_cpus);
    log_error("'");
    log_error("\n");
    if (prm.verbose > 1 && prm.cm) {
      log_error("logical proc -> core: ");
      cpus_dump(stderr,prm.cm);
      log_error("\n");
    }
  }
  if (g_cmd.aff_mode == aff_random) {
    for (int k = 0 ; k < g_n_aff_cpus ; k++) {
      g_aff_cpus[k] = g_all_cpus->cpu[k % g_all_cpus->sz];
    }
  } else if (g_cmd.aff_mode == aff_custom) {
    st_t seed = 0;
    custom_affinity(&seed,prm.cm,color,diff,g_all_cpus,g_max_exe,g_aff_cpus);
    if (prm.verbose) {
      log_error("thread allocation: \n");
      cpus_dump_test(stderr,g_aff_cpus,g_n_aff_cpus,prm.cm,N);
    }
  } else if (g_cmd.aff_mode == aff_topo) {
    int *from = &cpu_scan[ntopo * SCANLINE];
    for (int k = 0 ; k < g_n_aff_cpus; k++) {
      g_aff_cpus[k] = *from++;
    }
  }
  g_n_th = g_max_exe-1;
  g_zargs = malloc_check(sizeof(zyva_t) * g_max_exe, "g_zargs");
  g_hists = alloc_hists(g_max_exe, "g_hists");
  g_hist = &(g_hists[g_n_th]);
  g_mutex = pm_create();
  g_barrier = pb_create(g_max_exe);
  g_next_cpu = 0;
  g_delta = g_cmd.aff_incr;
  if (g_delta <= 0) {
    for (int k=0 ; k < g_all_cpus->sz ; k++) g_all_cpus->cpu[k] = -1;
    g_delta = 1;
  } else {
    g_delta %= g_all_cpus->sz;
  }
  g_start_scan=0, g_max_start=gcd(g_delta,g_all_cpus->sz);
  g_aff_cpu = g_aff_cpus;
  g_n_exe = 0;
}

static void exe_ctor(FILE *out)
{
  zyva_t *p = g_zarg = &g_zargs[g_n_exe];
  p->_p = &prm;
  p->p_mutex = g_mutex; p->p_barrier = g_barrier; 
  p->z_id = g_n_exe;
  p->cpus = g_aff_cpu;
  if (g_cmd.aff_mode != aff_incr) {
    g_aff_cpu += N;
  } else {
    for (int i=0 ; i < N ; i++) {
      *g_aff_cpu = g_all_cpus->cpu[g_next_cpu]; g_aff_cpu++;
      g_next_cpu += g_delta; g_next_cpu %= g_all_cpus->sz;
      if (g_next_cpu == g_start_scan) {
        g_start_scan++ ; g_start_scan %= g_max_start;
        g_next_cpu = g_start_scan;
      }
    }
  }
  /* Make this operation asynchronous */
  /* pb_wait(p->p_barrier); */
  g_ctx._p = p->_p;

  init(&g_ctx);
  for (int _p = N-1 ; _p >= 0 ; _p--) {
    g_parg[_p].th_id = _p; g_parg[_p]._a = &g_ctx;
    g_parg[_p].cpu = &(p->cpus[0]);
  }
  g_n_run = 0;
}

static void run_ctor(FILE *out) {
  zyva_t *p = g_zarg = &g_zargs[g_n_exe];
  param_t *_b = p->_p;

  if (_b->aff_mode == aff_random) {
    pb_wait(p->p_barrier);
    if (p->z_id == 0) perm_prefix_ints(&g_ctx.seed,p->cpus,_b->ncpus_used,_b->ncpus);
    pb_wait(p->p_barrier);
  } else if (_b->aff_mode == aff_scan) {
    pb_wait(p->p_barrier);
    int idx_scan = g_n_run % SCANSZ;
    int *from =  &cpu_scan[SCANLINE*idx_scan];
    from += N*p->z_id;
    for (int k = 0 ; k < N ; k++) p->cpus[k] = from[k];
    pb_wait(p->p_barrier);
  } else {
  }
  if (_b->verbose>1) fprintf(stderr,"Run %i of %i\r", g_n_run, _b->max_run);
  reinit(&g_ctx);
  if (_b->do_change) perm_funs(&g_ctx.seed,g_fun,N);
  zyva();
}

static void run_dtor(FILE *out)
{
  zyva_t *p = g_zarg = &g_zargs[g_n_exe];
  param_t *_b = p->_p;

  /* Log final states */
  for (int _i = _b->size_of_test-1 ; _i >= 0 ; _i--) {
    int _out_1_x5_i = g_ctx.out_1_x5[_i];
    int _out_1_x7_i = g_ctx.out_1_x7[_i];
    outcome_t o;
    int cond;
    hist_t *hist;

    if (p->z_id < g_n_th)
      hist = &(g_hists[p->z_id]);
    else
      hist = g_hist;
    cond = final_ok(final_cond(_out_1_x5_i,_out_1_x7_i));
    o[out_1_x5_f] = _out_1_x5_i;
    o[out_1_x7_f] = _out_1_x7_i;
    add_outcome(hist,1,o,cond);
    if (_b->aff_mode == aff_scan && p->cpus[0] >= 0 && cond) {
      pm_lock(p->p_mutex);
      g_ngroups[g_n_run % SCANSZ]++;
      pm_unlock(p->p_mutex);
    } else if (_b->aff_mode == aff_topo && p->cpus[0] >= 0 && cond) {
      pm_lock(p->p_mutex);
      g_ngroups[0]++;
      pm_unlock(p->p_mutex);
    }
    if (cond) { hist->n_pos++; } else { hist->n_neg++; }
  }
  g_n_run++;
  if (g_n_run < _b->max_run) {
    litmus_raise(LITMUS_EVT_RUN_NEXT);
  } else {
    litmus_raise(LITMUS_EVT_CLOSE);
  }
}

static void exe_dtor(FILE *out)
{
  finalize(&g_ctx);
  g_n_exe++;
  if (g_n_exe < g_max_exe) {
    litmus_raise(LITMUS_EVT_EXE_START);
  } else {
    litmus_raise(LITMUS_EVT_CLOSE);
  }
}

static void dtor(FILE *out) {
  count_t n_outs = prm.size_of_test; n_outs *= prm.max_run;
  for (int k=0 ; k < g_n_th ; k++) {
    hist_t *hk = &g_hists[k];
    if (sum_outs(hk->outcomes) != n_outs || hk->n_pos + hk->n_neg != n_outs) {
      fatal("MP, sum_hist");
    }
    merge_hists(g_hist,hk);
  }
  cpus_free(g_all_cpus, "g_all_cpus");
  tsc_t total = timeofday() - g_start;
  pm_free(g_mutex);
  pb_free(g_barrier);

  n_outs *= g_max_exe;
  if (sum_outs(g_hist->outcomes) != n_outs || g_hist->n_pos + g_hist->n_neg != n_outs) {
    fatal("MP, sum_hist") ;
  }
  count_t p_true = g_hist->n_pos, p_false = g_hist->n_neg;
  postlude(out,&g_cmd,g_hist,p_true,p_false,total);
  free_hists(g_max_exe, g_hists, "g_hists");
  free_check(g_zargs, "g_zargs");
  free_check(g_aff_cpus, "g_aff_cpus");
  cpus_free(prm.cm, "prm.cm");
  if (g_def_all_cpus != g_cmd.aff_cpus)
    cpus_free(g_def_all_cpus, "g_def_all_cpus");
  litmus_raise(LITMUS_EVT_CLOSE);
}

int MP(int argc, char **argv, FILE *out) {
  g_def_all_cpus = read_force_affinity(AVAIL,0,"g_def_all_cpus");
  if (g_def_all_cpus->sz < N) {
    cpus_free(g_def_all_cpus, "g_def_all_cpus");
    return EXIT_SUCCESS;
  }
  g_def.aff_cpus = g_def_all_cpus;
  g_cmd = g_def;
  if (parse_cmd(argc,argv,&g_def,&g_cmd) == 0) {
    litmus_launch();
  } else if (g_def_all_cpus != g_cmd.aff_cpus)
    cpus_free(g_def_all_cpus, "g_def_all_cpus");
  return EXIT_SUCCESS;
}

int foobar(caddr_t percpu_area) {
  return 1;
}

__define_testfn(foobar, 0, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);

static int do_litmus(int argc, char **argv)
{
  return MP(argc, argv, stderr);
}

DEFINE_COMMAND(litmus, do_litmus, "Run memory model litmus tests",
  "litmus -h\n"
  "    -display test usage\n"
);

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
  case LITMUS_EVT_EXE_START:
    printf("E: EXE START\n");
    break;
  case LITMUS_EVT_EXE_STOP:
    printf("E: EXE STOP\n");
    break;
  case LITMUS_EVT_RUN_NEXT:
    printf("E: RUN NEXT\n");
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
  case LITMUS_STA_EXE_LOOP:
    printf("S: EXE LOOP\n");
    break;
  case LITMUS_STA_RUN_LOOP:
    printf("S: RUN LOOP\n");
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
    switch (state) {
    default:
      break;
    }
  }
}

static void litmus_handler(uint8_t __event)
{
  uint32_t event = litmus_event;

  litmus_event = 0;
  switch (litmus_state) {
  case LITMUS_STA_IDLE:
    if (event & LITMUS_EVT_OPEN) {
      ctor(stderr);
      litmus_enter(LITMUS_STA_EXE_LOOP);
      litmus_raise(LITMUS_EVT_EXE_START);
    }
    break;
  case LITMUS_STA_EXE_LOOP:
    if (event & LITMUS_EVT_EXE_START) {
      exe_ctor(stderr);
      litmus_enter(LITMUS_STA_RUN_LOOP);
      litmus_raise(LITMUS_EVT_RUN_NEXT);
    }
    if (event & LITMUS_EVT_EXE_STOP) {
      exe_dtor(stderr);
    }
    if (event & LITMUS_EVT_CLOSE) {
      dtor(stderr);
      litmus_enter(LITMUS_STA_IDLE);
    }
    break;
  case LITMUS_STA_RUN_LOOP:
    if (event & LITMUS_EVT_RUN_NEXT) {
      run_ctor(stderr);
      run_dtor(stderr);
    }
    if (event & LITMUS_EVT_CLOSE) {
      litmus_enter(LITMUS_STA_EXE_LOOP);
      litmus_raise(LITMUS_EVT_EXE_STOP);
    }
  }
}

void litmus_launch(void)
{
  litmus_raise(LITMUS_EVT_OPEN);
}

void litmus_init(void)
{
  litmus_event = 0;
  litmus_state = LITMUS_STA_IDLE;
  litmus_bh = bh_register_handler(litmus_handler);
}
