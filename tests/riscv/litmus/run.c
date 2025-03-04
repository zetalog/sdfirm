#include <target/litmus.h>
#include <target/cmdline.h>

/* Declarations of tests entry points */
#ifdef CONFIG_LITMUS_RISCV_DUMMY
extern int litmus_dummy_test(int argc,char **argv);
#else
#define litmus_dummy_test(argc,argv) do { } while (0)
#endif
#ifdef CONFIG_LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd
extern int LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd(int argc, char **argv);
#else
#define LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd(argc, argv)  do {} while(0)
#endif
#ifdef CONFIG_WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx
extern int WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx(int argc, char **argv);
#else
#define CONFIG_WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx(argc, argv)  do {} while(0)
#endif

/* Date function */
static void my_date(FILE *out) {
  clock_t t = clock();
  fprintf(out,"%ld\n",(unsigned long)t);
}

/* Postlude */
static void end_report(int argc,char **argv,FILE *out) {
  fprintf(out,"%s\n","Revision 6773cdcd07d2f1f9413a9cd2a38c74f552fe0faa, version 7.56+01(dev)");
  fprintf(out,"%s\n","Command line: litmus7 -mach ./riscv.cfg -avail 4 -excl gcc.excl -excl instructions.excl -o hw-tests-src tests/non-mixed-size/@all");
  fprintf(out,"%s\n","Parameters");
  fprintf(out,"%s\n","#define SIZE_OF_TEST 100");
  fprintf(out,"%s\n","#define NUMBER_OF_RUN 10");
  fprintf(out,"%s\n","#define AVAIL 4");
  fprintf(out,"%s\n","#define STRIDE 1");
  fprintf(out,"%s\n","#define MAX_LOOP 0");
  fprintf(out,"%s\n","/* gcc options: -D_GNU_SOURCE -DFORCE_AFFINITY -Wall -std=gnu99 -O2 -pthread */");
  fprintf(out,"%s\n","/* gcc link options: -static */");
  fprintf(out,"%s\n","/* barrier: userfence */");
  fprintf(out,"%s\n","/* launch: changing */");
  fprintf(out,"%s\n","/* affinity: incr1 */");
  fprintf(out,"%s\n","/* alloc: dynamic */");
  fprintf(out,"%s\n","/* memory: direct */");
  fprintf(out,"%s\n","/* stride: 1 */");
  fprintf(out,"%s\n","/* safer: write */");
  fprintf(out,"%s\n","/* preload: random */");
  fprintf(out,"%s\n","/* speedcheck: no */");
  fprintf(out,"%s\n","/* proc used: 4 */");
/* Command line options */
  fprintf(out,"Command:");
  for ( ; *argv ; argv++) {
    fprintf(out," %s",*argv);
  }
  fprintf(out,"\n");
}

/* Run all tests */
static void run(int argc,char **argv) {
  litmus_finishing = false;
  my_date(stderr);
#ifdef CONFIG_LITMUS_RISCV_DUMMY
  litmus_dummy_test(argc,argv);
#endif
#ifdef CONFIG_LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd
  litmus_dummy_test(argc,argv);
#endif
#ifdef CONFIG_WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx
  litmus_dummy_test(argc,argv);
#endif
  end_report(argc,argv,stderr);
  my_date(stderr);
  litmus_finishing = true;
  litmus_finish();
}

int litmus_main(int argc,char **argv) {
  run(argc,argv);
  return 0;
}

DEFINE_COMMAND(litmus,litmus_main,"Run all memory model litmus tests",
  "litmus -h\n"
  "    -display test usage\n"
);
