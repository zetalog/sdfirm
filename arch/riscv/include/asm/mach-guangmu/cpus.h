#ifndef __CPUS_GUANGMU_H_INCLUDE__
#define __CPUS_GUANGMU_H_INCLUDE__

#ifdef CONFIG_SMP
#define MAX_CPU_NUM		CONFIG_GM_SMP_CPUS
#else
#define MAX_CPU_NUM		1
#endif

#endif /* __CPUS_GUANGMU_H_INCLUDE__ */
