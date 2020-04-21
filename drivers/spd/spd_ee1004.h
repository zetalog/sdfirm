#ifndef __SPD_EE1004_H__
#define __SPD_EE1004_H__


#define SPD_EE_PAGE_SIZE 256
#define SPD_EE_PAGE_CNT 2
#define SPD_EE_SIZE (SPD_EE_PAGE_SIZE * SPD_EE_PAGE_CNT)

#define SPD_EE_PAGE_WRITE_SIZE 16

/* Device Select Code */
#define SPD_DSC_RTR  0x31
#define SPD_DSC_WTR  0x30
#define SPD_DSC_RSPD 0xA1
#define SPD_DSC_WSPD 0xA0
#define SPD_DSC_SWP0 0x62
#define SPD_DSC_SWP1 0x68
#define SPD_DSC_SWP2 0x6A
#define SPD_DSC_SWP3 0x60
#define SPD_DSC_CWP  0x66
#define SPD_DSC_RPS0 0x63
#define SPD_DSC_RPS1 0x69
#define SPD_DSC_RPS2 0x6B
#define SPD_DSC_RPS3 0x61
#define SPD_DSC_SPA0 0x6C
#define SPD_DSC_SPA1 0x6E
#define SPD_DSC_RPA  0x6D

#endif
