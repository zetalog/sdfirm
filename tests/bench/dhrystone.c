#include <target/dhrystone.h>

/* Global Variables: */

#ifdef HOSTED
Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];
#else
#ifdef CONFIG_DHRYSTONE_TIMEOUT
#define DHRYSTONE_TIMEOUT		CONFIG_DHRYSTONE_TIMEOUT
#else
#define DHRYSTONE_TIMEOUT		(1 * 1000 * 1000)
#endif
#ifdef CONFIG_DHRYSTONE_REPEATS
#define DHRYSTONE_REPEATS		CONFIG_DHRYSTONE_REPEATS
#else
#define DHRYSTONE_REPEATS		5000000
#endif

uint64_t dhrystone_num_of_runs(void)
{
	return DHRYSTONE_REPEATS;
}

#ifdef CONFIG_DHRYSTONE_TIME
uint64_t dhrystone_expected_timeout(void)
{
	return DHRYSTONE_TIMEOUT;
}
#endif

#define TRACE_ID_MASK	0x000000FF
#define TRACE_RUN_MASK	0xFFFFFF00
#define TRACE_RUN_SHIFT 8

#include <target/percpu.h>

#ifdef CONFIG_DHRYSTONE_SPECIFIC_PERCPU
struct dhrystone_percpu {
	struct dhrystone_context *ptr;
} __cache_aligned;

struct dhrystone_percpu dhrystone_ctx[MAX_CPU_NUM];
#define get_dhrystone		(dhrystone_ctx[smp_processor_id()].ptr)
#endif
#ifdef CONFIG_DHRYSTONE_GENERIC_PERCPU
DEFINE_PERCPU(struct dhrystone_context, dhrystone_ctx);
#define get_dhrystone		this_cpu_ptr(&dhrystone_ctx)
#endif
#ifdef CONFIG_DHRYSTONE_BENCH_PERCPU
struct dhrystone_context *get_dhrystone;
#endif
#define Ptr_Glob		get_dhrystone->Ptr_Glob
#define Next_Ptr_Glob		get_dhrystone->Next_Ptr_Glob
#define Int_Glob		get_dhrystone->Int_Glob
#define Bool_Glob		get_dhrystone->Bool_Glob
#define Ch_1_Glob		get_dhrystone->Ch_1_Glob
#define Ch_2_Glob		get_dhrystone->Ch_2_Glob
#define Arr_1_Glob		get_dhrystone->Arr_1_Glob
#define Arr_2_Glob		get_dhrystone->Arr_2_Glob
#endif

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

void Proc_1 (REG Rec_Pointer Ptr_Val_Par);
void Proc_2 (One_Fifty *Int_Par_Ref);
void Proc_3 (Rec_Pointer *Ptr_Ref_Par);
void Proc_4 (void);
void Proc_5 (void);
void Proc_6 (Enumeration Enum_Val_Par, Enumeration *Enum_Ref_Par);
void Proc_7 (One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val,
             One_Fifty *Int_Par_Ref);
void Proc_8 (Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref,
             int Int_1_Par_Val, int Int_2_Par_Val);

Enumeration Func_1 (Capital_Letter Ch_1_Par_Val,
                    Capital_Letter Ch_2_Par_Val);
Boolean Func_2 (Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref);
Boolean Func_3 (Enumeration Enum_Par_Val);

  /* forward declaration necessary since Enumeration may not simply be int */

#include "dhry_1.c"
#include "dhry_2.c"


#ifdef HOSTED
int main (int argc, char *argv[])
#else
int dhrystone (caddr_t percpu_area)
#endif

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
        One_Fifty   Int_1_Loc;
  REG   One_Fifty   Int_2_Loc = 3;
        One_Fifty   Int_3_Loc;
  REG   char        Ch_Index;
        Enumeration Enum_Loc;
        Str_30      Str_1_Loc;
        Str_30      Str_2_Loc;
  REG   int         Run_Index;
        int         Number_Of_Runs;
        int         errors = 0;

  /* variables for time measurement: */

                    /* Measurements should last at least 2 seconds */

        long long   User_Time,
                    Begin_Time,
                    End_Time;
#ifdef CONFIG_DHRYSTONE_TIME
	long long   Expected_End_Time;
#endif
#ifdef CONFIG_FP
#define DHRY_FMT    "%6.1f"
#define DHRY_FMT2   "%12.21f"
        float       __unused Microseconds;
        float       Vax_Mips,
                    Dhrystones_Per_Second;
#else /* CONFIG_FP */
#define DHRY_FMT    "%6lld"
#define DHRY_FMT2   "%12lld"
        long        __unused Microseconds;
        long long   Vax_Mips,
                    Dhrystones_Per_Second;
#endif /* CONFIG_FP */

#ifndef HOSTED
        Rec_Type    Type_Glob,
                    Next_Type_Glob;
#endif

  /* end of variables for time measurement */

  /* Initializations */

#ifdef HOSTED
  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
#else
#ifdef CONFIG_DHRYSTONE_BENCH_PERCPU
  get_dhrystone = (struct dhrystone_context *)percpu_area;
#endif
  Next_Ptr_Glob = &Next_Type_Glob;
  Ptr_Glob = &Type_Glob;
#endif

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;
  strcpy (Ptr_Glob->variant.var_1.Str_Comp,
          "DHRYSTONE PROGRAM, SOME STRING");
  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

  Arr_2_Glob [8][7] = 10-DHRYSTONE_WARMUP_RUNS;
        /* Was missing in published program. Without this statement,   */
        /* Arr_2_Glob [8][7] would have an undefined value.            */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000, */
        /* overflow may occur for this array element.                  */

#ifdef HOSTED
  if (argc != 2)
  {
      dhry_printf("usage: ./gcc_dhry2 <Number of runs Example:1000000000> \n");
      return 0;
  }
#endif

  dhry_printf ("\n");
  dhry_printf ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  dhry_printf ("\n");
  if (Reg)
  {
    dhry_printf ("Program compiled with 'register' attribute\n");
    dhry_printf ("\n");
  }
  else
  {
    dhry_printf ("Program compiled without 'register' attribute\n");
    dhry_printf ("\n");
  }
#ifdef HOSTED
  Number_Of_Runs = atoi(argv[1]);
#else
  Number_Of_Runs = dhrystone_num_of_runs();
#ifdef CONFIG_DHRYSTONE_TIME
  Expected_End_Time = dhrystone_expected_timeout();
  if (Expected_End_Time != CPU_WAIT_INFINITE)
  {
    Expected_End_Time += dhrystone_time();
  }
#endif /* CONFIG_DHRYSTONE_TIME */
#endif

  dhry_printf ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

  Begin_Time = dhrystone_time();
  Number_Of_Runs += DHRYSTONE_WARMUP_RUNS;

  for (Run_Index = 0; Run_Index <= Number_Of_Runs; ++Run_Index)
  {

    if (unlikely(Run_Index == DHRYSTONE_WARMUP_RUNS))
    {
      /***************/
      /* Start timer */
      /***************/

      Begin_Time = dhrystone_time();
    }

    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;

    dhry_strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    }   /* while */
       /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                         /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        dhry_strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

#ifndef HOSTED
#ifdef CONFIG_DHRYSTONE_TIME
    if (unlikely(Expected_End_Time != CPU_WAIT_INFINITE) &&
        likely(Run_Index > DHRYSTONE_WARMUP_RUNS)) {
      if (time_after(dhrystone_time(), Expected_End_Time))
      {
        Number_Of_Runs = Run_Index;
        break;
      }
    }
#endif /* CONFIG_DHRYSTONE_TIME */
#endif
  }  /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/

  Number_Of_Runs -= DHRYSTONE_WARMUP_RUNS;
  End_Time = dhrystone_time();

  dhry_printf ("Execution ends\n");
  dhry_printf ("\n");
  dhry_printf ("Final values of the variables used in the benchmark:\n");
  dhry_printf ("\n");
  dhry_printf ("Int_Glob:            %d\n", Int_Glob);
  dhry_printf ("        should be:   %d\n", 5);
  dhry_printf ("Bool_Glob:           %d\n", Bool_Glob);
  dhry_printf ("        should be:   %d\n", 1);
  dhry_printf ("Ch_1_Glob:           %c\n", Ch_1_Glob);
  dhry_printf ("        should be:   %c\n", 'A');
  dhry_printf ("Ch_2_Glob:           %c\n", Ch_2_Glob);
  dhry_printf ("        should be:   %c\n", 'B');
  dhry_printf ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  dhry_printf ("        should be:   %d\n", 7);
  dhry_printf ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  dhry_printf ("        should be:   Number_Of_Runs + 10\n");
  dhry_printf ("Ptr_Glob->\n");
  dhry_printf ("  Ptr_Comp:          %d\n", (int) (uintptr_t)(Ptr_Glob->Ptr_Comp));
  dhry_printf ("        should be:   (implementation-dependent)\n");
  dhry_printf ("  Discr:             %d\n", Ptr_Glob->Discr);
  dhry_printf ("        should be:   %d\n", 0);
  dhry_printf ("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
  dhry_printf ("        should be:   %d\n", 2);
  dhry_printf ("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
  dhry_printf ("        should be:   %d\n", 17);
  dhry_printf ("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
  dhry_printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  dhry_printf ("Next_Ptr_Glob->\n");
  dhry_printf ("  Ptr_Comp:          %d\n", (int) (uintptr_t)(Next_Ptr_Glob->Ptr_Comp));
  dhry_printf ("        should be:   (implementation-dependent), same as above\n");
  dhry_printf ("  Discr:             %d\n", Next_Ptr_Glob->Discr);
  dhry_printf ("        should be:   %d\n", 0);
  dhry_printf ("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
  dhry_printf ("        should be:   %d\n", 1);
  dhry_printf ("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
  dhry_printf ("        should be:   %d\n", 18);
  dhry_printf ("  Str_Comp:          %s\n",
                                Next_Ptr_Glob->variant.var_1.Str_Comp);
  dhry_printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  dhry_printf ("Int_1_Loc:           %d\n", Int_1_Loc);
  dhry_printf ("        should be:   %d\n", 5);
  dhry_printf ("Int_2_Loc:           %d\n", Int_2_Loc);
  dhry_printf ("        should be:   %d\n", 13);
  dhry_printf ("Int_3_Loc:           %d\n", Int_3_Loc);
  dhry_printf ("        should be:   %d\n", 7);
  dhry_printf ("Enum_Loc:            %d\n", Enum_Loc);
  dhry_printf ("        should be:   %d\n", 1);
  dhry_printf ("Str_1_Loc:           %s\n", Str_1_Loc);
  dhry_printf ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  dhry_printf ("Str_2_Loc:           %s\n", Str_2_Loc);
  dhry_printf ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  dhry_printf ("\n");

  User_Time = End_Time - Begin_Time;

  if (User_Time < Too_Small_Time)
  {
    printf ("Measured time too small to obtain meaningful results\n");
    printf ("Please increase number of runs\n");
    printf ("\n");
    /* Vax_Mips is not trustworthy then, time output is added for the
     * debugging purpose.
     */
#ifdef HOSTED
    (void) dhrystone_time();
    printf ("Begin time %lld, end time %lld: %s\n", Begin_Time, End_Time,
            errno != 0 ? strerror(errno) : "no error");
#endif
    Vax_Mips = -1;
  }
  else
  {
#ifdef CONFIG_FP
#ifdef HOSTED
#ifdef HAVE_CLOCK
    /* Microseconds based clock() */
    Microseconds = (float) User_Time
                        / (float) Number_Of_Runs / Ticks_Per_Mic;
    Dhrystones_Per_Second = (float) Number_Of_Runs * Mic_secs_Per_Second * Ticks_Per_Mic
                        / (float) User_Time;
#else
    /* Seconds based time() */
    Microseconds = (float) User_Time * Mic_secs_Per_Second
                        / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#endif
#else
    /* Microseconds based clock() or tsc_read_counter() */
    Microseconds = (float) User_Time
                        / (float) Number_Of_Runs / Ticks_Per_Mic;
    Dhrystones_Per_Second = (float) Number_Of_Runs * Mic_secs_Per_Second * Ticks_Per_Mic
                        / (float) User_Time;
#endif
#else /* CONFIG_FP */
    /* CONFIG_FP=n is only possible for HOSTED=n version where floating
     * pointer may be configured out, thus is microseconds based only.
     */
    Microseconds = User_Time / Number_Of_Runs / Ticks_Per_Mic;
    Dhrystones_Per_Second = (long long) Number_Of_Runs * Mic_secs_Per_Second * Ticks_Per_Mic
                        / (long long) User_Time;
#endif /* CONFIG_FP */
    Vax_Mips = Dhrystones_Per_Second / 1757;
    printf ("Number of runs:                             %d \n", Number_Of_Runs);
    printf ("User time (%s):                             %llu \n", DHRYSTONE_TIME_UNIT, User_Time);
    dhry_printf ("Microseconds for one run through Dhrystone: " DHRY_FMT " \n", Microseconds);
    dhry_printf ("Dhrystones per Second:                      " DHRY_FMT " \n", Dhrystones_Per_Second);
    printf ("VAX MIPS rating:                            " DHRY_FMT2 " \n", Vax_Mips);
    dhry_printf ("\n");
  }

  {
    /********************************************************************
     *              Add results to output file Dhry.txt                 *
     ********************************************************************/
#ifdef HOSTED
#ifdef CNNT
    #define options   "Non-optimised"
    #define opt "0"
#else
    #define options   "Optimised"
    #define opt "1"
#endif
#else
#ifdef CONFIG_CC_OPT_SPEED
    #define options   "Optimised"
    #define opt "1"
#else
    #define options   "Non-optimised"
    #define opt "0"
#endif
#endif

#ifdef HOSTED
#define Ap stdout
#else
#define Ap ((void *)1)
#endif

    dhry_fprintf (Ap, " #####################################################\n\n");
    dhry_fprintf (Ap, " Dhrystone Benchmark 2.1 %s via C/C++ \n", options);

    dhry_fprintf(Ap, " Classic Benchmark Ratings for CPUSpeed.txt where 100 MHz Pentium = 100\n");
    if (strtoul(opt, NULL, 10) == 1)
    {
      dhry_fprintf(Ap, " Integer Dhry2 Opt %d\n\n", (int)(Vax_Mips / 130 * 100));
    }
    else
    {
      dhry_fprintf(Ap, " Integer Dhry2 NoOpt %d\n\n", (int)(Vax_Mips / 32 * 100));
    }

    if (Int_Glob != 5)
    {
      dhry_fprintf (Ap, " Wrong result Int_Glob Was %d Should be 5\n", Int_Glob);
      errors = errors + 1;
    }
    if (Bool_Glob != 1)
    {
      dhry_fprintf (Ap, " Wrong result Bool_Glob Was %d Should be 1\n", Bool_Glob);
      errors = errors + 1;
    }
    if (Ch_1_Glob != 'A')
    {
      dhry_fprintf (Ap, " Wrong result Ch_1_Glob Was %c Should be A\n", Ch_1_Glob);
      errors = errors + 1;
    }
    if (Ch_2_Glob != 'B')
    {
      dhry_fprintf (Ap, " Wrong result Ch_2_Glob Was %c Should be B\n", Ch_2_Glob);
      errors = errors + 1;
    }
    if (Arr_1_Glob[8] != 7)
    {
      dhry_fprintf (Ap, " Wrong result Arr_1_Glob[8] Was %d Should be 7\n", Arr_1_Glob[8]);
      errors = errors + 1;
    }
    if (Arr_2_Glob[8][7] != Number_Of_Runs + 11)
    {
      dhry_fprintf (Ap, " Wrong result Arr_2_Glob[8][7] Was %d Should be %d\n", Arr_2_Glob[8][7], Number_Of_Runs + 11);
      errors = errors + 1;
    }
    if (Ptr_Glob->Discr != 0)
    {
      dhry_fprintf (Ap, " Wrong result Ptr_Glob->Discr Was %d Should be 0\n", Ptr_Glob->Discr);
      errors = errors + 1;
    }
    if (Ptr_Glob->variant.var_1.Enum_Comp != 2)
    {
      dhry_fprintf (Ap, " Wrong result Ptr_Glob->variant.var_1.Enum_Comp Was %d Should be 2\n", Ptr_Glob->variant.var_1.Enum_Comp);
      errors = errors + 1;
    }
    if (Ptr_Glob->variant.var_1.Int_Comp != 17)
    {
      dhry_fprintf (Ap, " Wrong result Ptr_Glob->variant.var_1.Int_Comp Was %d Should be 17\n", Ptr_Glob->variant.var_1.Int_Comp);
      errors = errors + 1;
    }
    if (strcmp(Ptr_Glob->variant.var_1.Str_Comp,  "DHRYSTONE PROGRAM, SOME STRING") != 0)
    {
      dhry_fprintf (Ap, " Wrong result Ptr_Glob->variant.var_1.Str_Comp Was %s Should be DHRYSTONE PROGRAM, SOME STRING\n", Ptr_Glob->variant.var_1.Str_Comp);
      errors = errors + 1;
    }
    if (Next_Ptr_Glob->Discr != 0)
    {
      dhry_fprintf (Ap, " Wrong result Next_Ptr_Glob->Discr Was %d Should be 0\n", Next_Ptr_Glob->Discr);
      errors = errors + 1;
    }
    if (Next_Ptr_Glob->variant.var_1.Enum_Comp != 1)
    {
      dhry_fprintf (Ap, " Wrong result Next_Ptr_Glob->variant.var_1.Enum_Comp Was %d Should be 1\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
      errors = errors + 1;
    }
    if (Next_Ptr_Glob->variant.var_1.Int_Comp != 18)
    {
      dhry_fprintf (Ap, " Wrong result Next_Ptr_Glob->variant.var_1.Int_Comp Was %d Should be 18\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
      errors = errors + 1;
    }
    if (strcmp(Next_Ptr_Glob->variant.var_1.Str_Comp,  "DHRYSTONE PROGRAM, SOME STRING") != 0)
    {
      dhry_fprintf (Ap, " Wrong result Next_Ptr_Glob->variant.var_1.Str_Comp Was %s Should be DHRYSTONE PROGRAM, SOME STRING\n", Next_Ptr_Glob->variant.var_1.Str_Comp);
      errors = errors + 1;
    }
    if (Int_1_Loc != 5)
    {
      dhry_fprintf (Ap, " Wrong result Int_1_Loc Was %d Should be 5\n", Int_1_Loc);
      errors = errors + 1;
    }
    if (Int_2_Loc != 13)
    {
      dhry_fprintf (Ap, " Wrong result Int_2_Loc Was %d Should be 13\n", Int_2_Loc);
      errors = errors + 1;
    }
    if (Int_3_Loc != 7)
    {
      dhry_fprintf (Ap, " Wrong result Int_3_Loc Was %d Should be 7\n", Int_3_Loc);
      errors = errors + 1;
    }
    if (Enum_Loc != 1)
    {
      dhry_fprintf (Ap, " Wrong result Enum_Loc Was %d Should be 1\n", Enum_Loc);
      errors = errors + 1;
    }
    if (strcmp(Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING") != 0)
    {
      dhry_fprintf (Ap, " Wrong result Str_1_Loc Was %s Should be DHRYSTONE PROGRAM, 1'ST STRING\n", Str_1_Loc);
      errors = errors + 1;
    }
    if (strcmp(Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING") != 0)
    {
      dhry_fprintf (Ap, " Wrong result Str_2_Loc Was %s Should be DHRYSTONE PROGRAM, 2'ND STRING\n", Str_2_Loc);
      errors = errors + 1;
    }
    if (errors == 0)
    {
      dhry_fprintf (Ap, " Numeric results were correct\n");
    }
    else
    {
      dhry_fprintf (Ap, " Numeric results were bad\n");
    }
    dhry_fprintf (Ap, "\n");

#ifdef HOSTED
    fclose(Ap);
#endif
  }

  return errors ? 0 : 1;
}
__define_testfn(dhrystone, sizeof (struct dhrystone_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
