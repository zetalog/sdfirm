/****************************************************************************/
/*                           the diy toolsuite                              */
/*                                                                          */
/* Jade Alglave, University College London, UK.                             */
/* Luc Maranget, INRIA Paris-Rocquencourt, France.                          */
/*                                                                          */
/* This C source is a product of litmus7 and includes source that is        */
/* governed by the CeCILL-B license.                                        */
/****************************************************************************/
/* Includes */
#include <target/litmus.h>
#include <target/cmdline.h>

/* Parameters */
#define SIZE_OF_TEST 100
#define NUMBER_OF_RUN 10
#define STRIDE 1
#define MAX_LOOP 0
#define N 2
#define AFF_INCR (1)

/* params */
typedef struct {
  int verbose;
  int size_of_test, max_run;
  int stride;
  aff_mode_t aff_mode;
  int ncpus, ncpus_used;
  int do_change;
  cpus_t *cm;
} param_t;

static param_t prm;

/* Barriers macros */
inline static void barrier_wait(unsigned int id, unsigned int k, int volatile *b) {
  if ((k % N) == id) {
    *b = 1 ;
    mb();
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


static void do_dump_outcome(intmax_t *o, count_t c, int show) {
  litmus_log("%-6"PCTR"%c>1:x5=%i; 1:x7=%i;\n",c,show ? '*' : ':',(int)o[out_1_x5_f],(int)o[out_1_x7_f]);
}

static void just_dump_outcomes(hist_t *h) {
  outcome_t buff ;
  dump_outs(do_dump_outcome,h->outcomes,buff,NOUTS) ;
}

/**************************************/
/* Prefetch (and check) global values */
/**************************************/

static void check_globals(ctx_t *_a) {
  int *y = _a->y;
  int *x = _a->x;
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    if (rand_bit(&(_a->seed)) && y[_i] != 0) litmus_fatal("MP, check_globals failed");
    if (rand_bit(&(_a->seed)) && x[_i] != 0) litmus_fatal("MP, check_globals failed");
  }
  pb_wait(_a->fst_barrier);
}


/***************/
/* Litmus code */
/***************/

typedef struct {
  int th_id; /* I am running on this thread */
  int *cpu; /* On this cpu */
  ctx_t *_a; /* In this context */
} parg_t;

static void *P0(void *_vb) {
  mb();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
#if 0
  int _ecpu = _b->cpu[_b->th_id];
  force_one_affinity(_ecpu,AVAIL,_a->_p->verbose,"MP");
#endif
  check_globals(_a);
  int _th_id = _b->th_id;
  int volatile *barrier = _a->barrier;
  int _size_of_test = _a->_p->size_of_test;
  int _stride = _a->_p->stride;
  for (int _j = _stride ; _j > 0 ; _j--) {
    for (int _i = _size_of_test-_j ; _i >= 0 ; _i -= _stride) {
      barrier_wait(_th_id,_i,&barrier[_i]);
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
    }
  }
  mb();
  return NULL;
}

static void *P1(void *_vb) {
  mb();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
#if 0
  int _ecpu = _b->cpu[_b->th_id];
  force_one_affinity(_ecpu,AVAIL,_a->_p->verbose,"MP");
#endif
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
    }
  }
  mb();
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

typedef struct {
  int z_id;
  int *cpus;
  ctx_t ctx;
  parg_t parg[N];
} zyva_t;

#define NT N

static tsc_t g_start;
static cmd_t g_def = {
  0, NUMBER_OF_RUN, SIZE_OF_TEST, STRIDE, AVAIL, 0, 0,
  aff_incr, 1, 1, AFF_INCR, NULL, NULL, -1, MAX_LOOP,
  NULL, NULL, -1, -1, -1, 0, 1
};
static cmd_t g_cmd;
static int g_max_exe;
static int g_n_th;
static int g_n_run;
static int g_next_cpu;
static int g_delta;
static int g_start_scan;
static int g_max_start;
static int *g_aff_cpu;

static cpus_t *g_def_all_cpus;
static cpus_t *g_all_cpus;
static int g_n_aff_cpus;
static int *g_aff_cpus;
static zyva_t *g_zargs;
static hist_t *g_hists;
static hist_t *g_hist;


typedef struct {
  f_t *fun;
  void *arg;
  int p;
} targ_t;

targ_t g_targs[NR_CPUS];

static void zyva(int cpu, int p, f_t *fun, void *arg)
{
#ifdef CONFIG_TEST_LITMUS_DEBUG
  printf("ZYVA: CPU%d: P%d\n", cpu, p);
#endif
  g_targs[cpu].p = p;
  g_targs[cpu].fun = fun;
  g_targs[cpu].arg = arg;
}

static f_t *g_fun[] = {&P0,&P1};
cpu_exec_cpu_t g_cpus[NT];

#ifdef ASS
static void ass(void) { }
#else
#include "MP.h"
#endif

static void prelude(void) {
  litmus_log("%s\n","%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  litmus_log("%s\n","% Results for ./tests/dummy/MP.litmus %");
  litmus_log("%s\n","%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  litmus_log("%s\n","RISCV MP");
  litmus_log("%s\n","\"PodWW Rfe PodRR Fre\"");
  litmus_log("%s\n","{0:x5=1; 0:x6=x; 0:x7=y; 1:x6=y; 1:x8=x;}");
  litmus_log("%s\n"," P0          | P1          ;");
  litmus_log("%s\n"," sw x5,0(x6) | lw x5,0(x6) ;");
  litmus_log("%s\n"," sw x5,0(x7) | lw x7,0(x8) ;");
  litmus_log("%s\n","");
  litmus_log("%s\n","exists (1:x5=1 /\\ 1:x7=0)");
  litmus_log("Generated assembler\n");
  ass();
}

static int color_0[] = {0, -1};
static int color_1[] = {1, -1};
static int *color[] = {color_0, color_1, NULL};
static int diff[] = {1, 0, -1};

#define ENOUGH 10

static void postlude(cmd_t *cmd,hist_t *hist,count_t p_true,count_t p_false,tsc_t total) {
  litmus_log("Test MP Allowed\n");
  litmus_log("Histogram (%i states)\n",finals_outs(hist->outcomes));
  just_dump_outcomes(hist);
  __unused int cond = p_true > 0;
  litmus_log("%s\n",cond?"Ok":"No");
  litmus_log("\nWitnesses\n");
  litmus_log("Positive: %" PCTR ", Negative: %" PCTR "\n",p_true,p_false);
  litmus_log("Condition %s is %svalidated\n","exists (1:x5=1 /\\ 1:x7=0)",cond ? "" : "NOT ");
#ifndef CONFIG_TEST_VERBOSE
  printf("%s: %s\n","exists (1:x5=1 /\\ 1:x7=0)",cond ? "OK" : "NG");
#endif
  litmus_log("Hash=2939da84098a543efdbb91e30585ab71\n");
  litmus_log("Cycle=Rfe PodRR Fre PodWW\n");
  litmus_log("Relax MP %s %s\n",p_true > 0 ? "Ok" : "No","");
  litmus_log("Safe=Rfe Fre PodWW PodRR\n");
  litmus_log("Generator=diy7 (version 7.51+4(dev))\n");
  litmus_log("Com=Rf Fr\n");
  litmus_log("Orig=PodWW Rfe PodRR Fre\n");
  if (cmd->aff_mode == aff_custom) {
    litmus_log("Affinity=[0] [1] ; (1,0)\n");
  }
  __unused count_t cond_true = p_true;
  __unused count_t cond_false = p_false;
  litmus_log("Observation MP %s %" PCTR " %" PCTR "\n",!cond_true ? "Never" : !cond_false ? "Always" : "Sometimes",cond_true,cond_false);
  if (p_true > 0) {
    if (cmd->aff_mode == aff_scan) {
      for (int k = 0 ; k < SCANSZ ; k++) {
        count_t c = g_ngroups[k];
        if ((c*100)/p_true > ENOUGH) { litmus_log("Topology %-6" PCTR":> %s\n",c,group[k]); }
      }
    } else if (cmd->aff_mode == aff_topo) {
      litmus_log("Topology %-6" PCTR ":> %s\n",g_ngroups[0],cmd->aff_topo);
    }
  }
  litmus_log("Time MP %.2f\n",total / 1000000.0);
}

void litmus_start(void) {
  if (g_cmd.prelude) prelude();
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
      litmus_log("Bad topology %s, reverting to scan affinity\n",g_cmd.aff_topo);
      g_cmd.aff_mode = aff_scan; g_cmd.aff_topo = NULL;
    }
  }
  prm.do_change = g_cmd.aff_mode != aff_custom && g_cmd.aff_mode != aff_scan && g_cmd.aff_mode != aff_topo;
  if (g_cmd.fix) prm.do_change = 0;
  prm.cm = coremap_seq(g_def_all_cpus->sz,2,"prm.cm");
/* Computes number of test concurrent instances */
  int n_avail = g_cmd.avail > 0 ? g_cmd.avail : g_cmd.aff_cpus->sz;
  if (n_avail >  g_cmd.aff_cpus->sz) litmus_log("Warning: avail=%i, available=%i\n",n_avail, g_cmd.aff_cpus->sz);
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
    litmus_log( "MP: n=%i, r=%i, s=%i",g_max_exe,prm.max_run,prm.size_of_test);
    litmus_log(", st=%i",prm.stride);
    if (g_cmd.aff_mode == aff_incr) {
      litmus_log( ", i=%i",g_cmd.aff_incr);
    } else if (g_cmd.aff_mode == aff_random) {
      litmus_log(", +ra");
    } else if (g_cmd.aff_mode == aff_custom) {
      litmus_log(", +ca");
    } else if (g_cmd.aff_mode == aff_scan) {
      litmus_log(", +sa");
    }
    litmus_log(", p='");
    cpus_dump(false, g_cmd.aff_cpus);
    litmus_log("'");
    litmus_log("\n");
    if (prm.verbose > 1 && prm.cm) {
      litmus_log("logical proc -> core: ");
      cpus_dump(false, prm.cm);
      litmus_log("\n");
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
      litmus_log("thread allocation: \n");
      cpus_dump_test(g_aff_cpus,g_n_aff_cpus,prm.cm,N);
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
  for (int k=0; k < g_max_exe; k++) {
    zyva_t *p = &g_zargs[k];
    ctx_t *_a = &(p->ctx);
    p->z_id = k;
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
    _a->_p = &prm;
    init(_a);
    for (int _p = N-1 ; _p >= 0 ; _p--) {
      p->parg[_p].th_id = _p; p->parg[_p]._a = &(p->ctx);
      p->parg[_p].cpu = &(p->cpus[0]);
    }
  }
  g_n_run = 0;
}

void litmus_run_start(void) {
  memset(g_targs, 0, sizeof(g_targs));

  for (int k=0; k < g_max_exe; k++) {
    zyva_t *p = &g_zargs[k];
    ctx_t *_a = &(p->ctx);
    if (_a->_p->aff_mode == aff_random) {
      if (p->z_id == 0)
        perm_prefix_ints(&(_a->seed),p->cpus,_a->_p->ncpus_used,_a->_p->ncpus);
    } else if (_a->_p->aff_mode == aff_scan) {
      int idx_scan = g_n_run % SCANSZ;
      int *from =  &cpu_scan[SCANLINE*idx_scan];
      from += N*p->z_id;
      for (int i = 0 ; i < N ; i++) p->cpus[i] = from[i];
    } else {
    }
    if (_a->_p->verbose>1) litmus_log("Run %i of %i\r", g_n_run, _a->_p->max_run);
    reinit(_a);
    if (_a->_p->do_change) perm_funs(&(_a->seed),g_fun,N);
    for (int _p = NT-1 ; _p >= 0 ; _p--) {
#if 0
      launch(&g_cpus[_p],g_fun[_p],&g_parg[_p]);
#endif
      parg_t *parg = (parg_t *)&(p->parg[_p]);
      int _ecpu = parg->cpu[parg->th_id];
      zyva(_ecpu, _p, g_fun[_p], parg);
    }
  }
  litmus_exec("litmus_MP");
}

void litmus_run_stop(void)
{
  for (int k=0; k < g_max_exe; k++) {
    zyva_t *p = &g_zargs[k];
    ctx_t *_a = &(p->ctx);
#if 0
    if (_a->_p->do_change) perm_cpus(&(p->seed),g_cpus,NT);
    for (int _p = NT-1; _p >= 0 ; _p--) {
      join(&g_cpus[_p]);
    }
#endif
    /* Log final states */
    for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
      int _out_1_x5_i = _a->out_1_x5[_i];
      int _out_1_x7_i = _a->out_1_x7[_i];
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
      if (_a->_p->aff_mode == aff_scan && p->cpus[0] >= 0 && cond) {
        g_ngroups[g_n_run % SCANSZ]++;
      } else if (_a->_p->aff_mode == aff_topo && p->cpus[0] >= 0 && cond) {
        g_ngroups[0]++;
      }
      if (cond) { hist->n_pos++; } else { hist->n_neg++; }
    }
  }
  g_n_run++;
}

void litmus_stop(void)
{
  for (int k=0; k < g_max_exe; k++) {
    zyva_t *p = &g_zargs[k];
    ctx_t *_a = &(p->ctx);
    finalize(_a);
  }

  count_t n_outs = prm.size_of_test; n_outs *= prm.max_run;
  for (int k=0 ; k < g_n_th ; k++) {
    hist_t *hk = &g_hists[k];
    if (sum_outs(hk->outcomes) != n_outs || hk->n_pos + hk->n_neg != n_outs) {
      litmus_fatal("MP, sum_hist");
    }
    merge_hists(g_hist,hk);
  }
  cpus_free(g_all_cpus, "g_all_cpus");
  tsc_t total = timeofday() - g_start;

  n_outs *= g_max_exe;
  if (sum_outs(g_hist->outcomes) != n_outs || g_hist->n_pos + g_hist->n_neg != n_outs) {
    litmus_fatal("MP, sum_hist") ;
  }
  count_t p_true = g_hist->n_pos, p_false = g_hist->n_neg;
  postlude(&g_cmd,g_hist,p_true,p_false,total);
  free_hists(g_max_exe, g_hists, "g_hists");
  free_check(g_zargs, "g_zargs");
  free_check(g_aff_cpus, "g_aff_cpus");
  cpus_free(prm.cm, "prm.cm");
  if (g_def_all_cpus != g_cmd.aff_cpus)
    cpus_free(g_def_all_cpus, "g_def_all_cpus");
}

bool litmus_closed(void) {
  return g_n_run >= prm.max_run;
}

static int MP(int argc, char **argv) {
  g_def_all_cpus = read_force_affinity(AVAIL,0,"g_def_all_cpus");
  if (g_def_all_cpus->sz < N) {
    cpus_free(g_def_all_cpus, "g_def_all_cpus");
    return -EINVAL;
  }
  g_def.aff_cpus = g_def_all_cpus;
  g_cmd = g_def;
  if (parse_cmd(argc,argv,&g_def,&g_cmd) != 0) {
    if (g_def_all_cpus != g_cmd.aff_cpus)
      cpus_free(g_def_all_cpus, "g_def_all_cpus");
    return -EINVAL;
  }
  litmus_launch();
  return 0;
}

DEFINE_COMMAND(MP, MP, "Run memory model litmus tests - " "MP",
	"MP" " -h\n"
	"    -display test usage\n"
);

int litmus_MP(caddr_t percpu_area) {
  int cpu = smp_processor_id();
  targ_t *targ = &(g_targs[cpu]);

  if (targ->fun)
    targ->fun(targ->arg);
  return 0;
}

__define_testfn(litmus_MP, 0, SMP_CACHE_BYTES,
		CPU_EXEC_SIMPLE, 1, CPU_WAIT_INFINITE);
