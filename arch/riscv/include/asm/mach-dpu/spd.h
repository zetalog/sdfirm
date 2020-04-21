#ifndef __SPD_DPU_H_INCLUDE__
#define __SPD_DPU_H_INCLUDE__

#ifdef CONFIG_DPU_DDR_SPD_BUS_NUM
#define SPD_BUS_NUM CONFIG_DPU_DDR_SPD_BUS_NUM
#else
#define SPD_BUS_NUM 0
#endif

#ifdef CONFIG_DPU_DDR_SPD_COUNT
#define SPD_COUNT CONFIG_DPU_DDR_SPD_COUNT
#else
#define SPD_COUNT 2
#endif

#endif /* __SPD_DPU_H_INCLUDE__ */
