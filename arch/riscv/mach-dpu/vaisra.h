#define PAR_EL1            0x5c0
#define ESR_EL1            0x5c1
#define BPCR_EL1           0x5c2
#define BPDR_EL1           0x5c3
#define BPTR0_EL1          0x5c4
#define BPTR1_EL1          0x5c5
#define BPTR2_EL1          0x5c6
#define BPUBTCR_EL1        0x5c7
#define BPUBTTR_EL1        0x5c8
#define CCPR_EL1           0x5c9
#define AFSR0_EL1          0x5ca
#define CPMR_EL1           0x5cb
#define DCCR0_EL1          0x5cc
#define DCECR_EL1          0x5cd
#define DCESR_EL1          0x5ce
#define ELMCR_EL1          0x5cf
#define ICECR_EL1          0x5d0
#define ICESR_EL1          0x5d1
#define L2CPUSRDR_EL1      0x5d2
#define L2CPUSRSELR_EL1    0x5d3
#define SCTLR_EL1          0x5de
#define TLBCR_EL1          0x5d4
#define CSSELR_EL1         0x5d5
#define DCEAR_EL1          0x5d6
#define DCESYNR0_EL1       0x5d7
#define DCESYNR1_EL1       0x5d8
#define PVR0F0_EL1         0x5e0
#define PVR0F1_EL1         0x5e1
#define PVR0F2_EL1         0x5e2
#define PVR0F3_EL1         0x5e3
#define PVR0F4_EL1         0x5e4
#define PVR1F0_EL1         0x5e5
#define PVR1F1_EL1         0x5e6
#define PVR2F0_EL1         0x5e7
#define PVR2F1_EL1         0x5e8
#define PVR2F2_EL1         0x5e9
#define PVR2F3_EL1         0x5ea
#define PVR2F7_EL1         0x5eb
#define PVR7F6_EL0         0x5ec
#define PVR7F7_EL0         0x5ed
#define CCSIDR_EL1         0xdc1
#define L1DCRDR_EL1        0xdc2
#define L1DCRTR0_EL1       0xdc3
#define L1DCRTR1_EL1       0xdc4
#define TLBRDR0_EL1        0xdc5
#define TLBRDR1_EL1        0xdc6
#define TLBRTR0_EL1        0xdc7
#define TLBRTR1_EL1        0xdd8
#define TLBRTR2_EL1        0xdd9
#define L0ICRDR_EL1        0xdda
#define L0ICRTR_EL1        0xddb
#define L1ICRDR_EL1        0xddc
#define L1ICRTR_EL1        0xddd
#define MPIDR              0xdc0
#define SCTLR_EL2          0x6c0
#define AFSR0_EL2          0x6c1
#define ESR_EL2            0x6c2
#define PMACFG0            0x7c0
#define PMACFG1            0x7c1
#define PMACFG2            0x7c2
#define PMACFG3            0x7c3
#define PMAADDR0           0x7d0
#define PMAADDR1           0x7d1
#define PMAADDR2           0x7d2
#define PMAADDR3           0x7d3
#define PMAADDR4           0x7d4
#define PMAADDR5           0x7d5
#define PMAADDR6           0x7d6
#define PMAADDR7           0x7d7
#define PMAADDR8           0x7d8
#define PMAADDR9           0x7d9
#define PMAADDR10          0x7da
#define PMAADDR11          0x7db
#define PMAADDR12          0x7dc
#define PMAADDR13          0x7dd
#define PMAADDR14          0x7de
#define PMAADDR15          0x7df
#define ESR_EL3            0x7e0
#define AFSR0_EL3          0x7e1
#define CCPR_EL3           0x7e2
#define RMR_EL3            0x7e3
#define SCTLR_EL3          0x7e6
#define NMICSR             0x7e8
#define ACPTR_EL3          0x7e9
#define VTTBR_EL2          0x680




#define BPRSW        .word 0x9c00802b
#define BPWSW        .word 0x9c10802b
#define DCCISW       .word 0x9c20802b
#define DCCIVAC      .word 0x9c30802b
#define DCCSW        .word 0x9c40802b
#define DCCVAC       .word 0x9c50802b
#define DCCVAU       .word 0x9c60802b
#define DCIALL       .word 0x9c70802b
#define DCISW        .word 0x9c80802b
#define DCIVAC       .word 0x9c90802b
#define DCRSW        .word 0x9ca0802b
#define DCZVA        .word 0x9cb0802b
#define ICRSW        .word 0x9cc0802b
#define ISB          .word 0x9cd0802b
#define UTLBRSW      .word 0x9ce0802b
#define ATS12E0R     .word 0x9d00802b
#define ATS12E0W     .word 0x9d10802b
#define ATS12E1R     .word 0x9d20802b
#define ATS12E1W     .word 0x9d30802b
#define ATS1E0R      .word 0x9d40802b
#define ATS1E0W      .word 0x9d50802b
#define ATS1E1R      .word 0x9d60802b
#define ATS1E1W      .word 0x9d70802b
#define ATS1E2R      .word 0x9d80802b
#define ATS1E2W      .word 0x9d90802b
#define ATS1E3R      .word 0x9da0802b
#define ATS1E3W      .word 0x9db0802b

#define DCCIVAC_X1   .word 0x9c30002b | 1 <<15
#define DCCIVAC_X2   .word 0x9c30002b | 2 <<15
#define DCCIVAC_X3   .word 0x9c30002b | 3 <<15
#define DCCIVAC_X4   .word 0x9c30002b | 4 <<15
#define DCCIVAC_X5   .word 0x9c30002b | 5 <<15
#define DCCIVAC_X6   .word 0x9c30002b | 6 <<15
#define DCCIVAC_X7   .word 0x9c30002b | 7 <<15
#define DCCIVAC_X8   .word 0x9c30002b | 8 <<15
#define DCCIVAC_X9   .word 0x9c30002b | 9 <<15
#define DCCIVAC_X10  .word 0x9c30002b | 10<<15
#define DCCIVAC_X11  .word 0x9c30002b | 11<<15
#define DCCIVAC_X12  .word 0x9c30002b | 12<<15
#define DCCIVAC_X13  .word 0x9c30002b | 13<<15
#define DCCIVAC_X14  .word 0x9c30002b | 14<<15
#define DCCIVAC_X15  .word 0x9c30002b | 15<<15
#define DCCIVAC_X16  .word 0x9c30002b | 16<<15
#define DCCIVAC_X17  .word 0x9c30002b | 17<<15
#define DCCIVAC_X18  .word 0x9c30002b | 18<<15
#define DCCIVAC_X19  .word 0x9c30002b | 19<<15
#define DCCIVAC_X20  .word 0x9c30002b | 20<<15
#define DCCIVAC_X21  .word 0x9c30002b | 21<<15
#define DCCIVAC_X22  .word 0x9c30002b | 22<<15
#define DCCIVAC_X23  .word 0x9c30002b | 23<<15
#define DCCIVAC_X24  .word 0x9c30002b | 24<<15
#define DCCIVAC_X25  .word 0x9c30002b | 25<<15
#define DCCIVAC_X26  .word 0x9c30002b | 26<<15
#define DCCIVAC_X27  .word 0x9c30002b | 27<<15
#define DCCIVAC_X28  .word 0x9c30002b | 28<<15
#define DCCIVAC_X29  .word 0x9c30002b | 29<<15
#define DCCIVAC_X30  .word 0x9c30002b | 30<<15
#define DCCIVAC_X31  .word 0x9c30002b | 31<<15

#define DCCVAC_X1    .word 0x9c50002b | 1 <<15
#define DCCVAC_X2    .word 0x9c50002b | 2 <<15
#define DCCVAC_X3    .word 0x9c50002b | 3 <<15
#define DCCVAC_X4    .word 0x9c50002b | 4 <<15
#define DCCVAC_X5    .word 0x9c50002b | 5 <<15
#define DCCVAC_X6    .word 0x9c50002b | 6 <<15
#define DCCVAC_X7    .word 0x9c50002b | 7 <<15
#define DCCVAC_X8    .word 0x9c50002b | 8 <<15
#define DCCVAC_X9    .word 0x9c50002b | 9 <<15
#define DCCVAC_X10   .word 0x9c50002b | 10<<15
#define DCCVAC_X11   .word 0x9c50002b | 11<<15
#define DCCVAC_X12   .word 0x9c50002b | 12<<15
#define DCCVAC_X13   .word 0x9c50002b | 13<<15
#define DCCVAC_X14   .word 0x9c50002b | 14<<15
#define DCCVAC_X15   .word 0x9c50002b | 15<<15
#define DCCVAC_X16   .word 0x9c50002b | 16<<15
#define DCCVAC_X17   .word 0x9c50002b | 17<<15
#define DCCVAC_X18   .word 0x9c50002b | 18<<15
#define DCCVAC_X19   .word 0x9c50002b | 19<<15
#define DCCVAC_X20   .word 0x9c50002b | 20<<15
#define DCCVAC_X21   .word 0x9c50002b | 21<<15
#define DCCVAC_X22   .word 0x9c50002b | 22<<15
#define DCCVAC_X23   .word 0x9c50002b | 23<<15
#define DCCVAC_X24   .word 0x9c50002b | 24<<15
#define DCCVAC_X25   .word 0x9c50002b | 25<<15
#define DCCVAC_X26   .word 0x9c50002b | 26<<15
#define DCCVAC_X27   .word 0x9c50002b | 27<<15
#define DCCVAC_X28   .word 0x9c50002b | 28<<15
#define DCCVAC_X29   .word 0x9c50002b | 29<<15
#define DCCVAC_X30   .word 0x9c50002b | 30<<15
#define DCCVAC_X31   .word 0x9c50002b | 31<<15

#define DCCVAU_X1    .word 0x9c60002b | 1 <<15
#define DCCVAU_X2    .word 0x9c60002b | 2 <<15
#define DCCVAU_X3    .word 0x9c60002b | 3 <<15
#define DCCVAU_X4    .word 0x9c60002b | 4 <<15
#define DCCVAU_X5    .word 0x9c60002b | 5 <<15
#define DCCVAU_X6    .word 0x9c60002b | 6 <<15
#define DCCVAU_X7    .word 0x9c60002b | 7 <<15
#define DCCVAU_X8    .word 0x9c60002b | 8 <<15
#define DCCVAU_X9    .word 0x9c60002b | 9 <<15
#define DCCVAU_X10   .word 0x9c60002b | 10<<15
#define DCCVAU_X11   .word 0x9c60002b | 11<<15
#define DCCVAU_X12   .word 0x9c60002b | 12<<15
#define DCCVAU_X13   .word 0x9c60002b | 13<<15
#define DCCVAU_X14   .word 0x9c60002b | 14<<15
#define DCCVAU_X15   .word 0x9c60002b | 15<<15
#define DCCVAU_X16   .word 0x9c60002b | 16<<15
#define DCCVAU_X17   .word 0x9c60002b | 17<<15
#define DCCVAU_X18   .word 0x9c60002b | 18<<15
#define DCCVAU_X19   .word 0x9c60002b | 19<<15
#define DCCVAU_X20   .word 0x9c60002b | 20<<15
#define DCCVAU_X21   .word 0x9c60002b | 21<<15
#define DCCVAU_X22   .word 0x9c60002b | 22<<15
#define DCCVAU_X23   .word 0x9c60002b | 23<<15
#define DCCVAU_X24   .word 0x9c60002b | 24<<15
#define DCCVAU_X25   .word 0x9c60002b | 25<<15
#define DCCVAU_X26   .word 0x9c60002b | 26<<15
#define DCCVAU_X27   .word 0x9c60002b | 27<<15
#define DCCVAU_X28   .word 0x9c60002b | 28<<15
#define DCCVAU_X29   .word 0x9c60002b | 29<<15
#define DCCVAU_X30   .word 0x9c60002b | 30<<15
#define DCCVAU_X31   .word 0x9c60002b | 31<<15

#define DCIVAC_X1    .word 0x9c90002b | 1 <<15
#define DCIVAC_X2    .word 0x9c90002b | 2 <<15
#define DCIVAC_X3    .word 0x9c90002b | 3 <<15
#define DCIVAC_X4    .word 0x9c90002b | 4 <<15
#define DCIVAC_X5    .word 0x9c90002b | 5 <<15
#define DCIVAC_X6    .word 0x9c90002b | 6 <<15
#define DCIVAC_X7    .word 0x9c90002b | 7 <<15
#define DCIVAC_X8    .word 0x9c90002b | 8 <<15
#define DCIVAC_X9    .word 0x9c90002b | 9 <<15
#define DCIVAC_X10   .word 0x9c90002b | 10<<15
#define DCIVAC_X11   .word 0x9c90002b | 11<<15
#define DCIVAC_X12   .word 0x9c90002b | 12<<15
#define DCIVAC_X13   .word 0x9c90002b | 13<<15
#define DCIVAC_X14   .word 0x9c90002b | 14<<15
#define DCIVAC_X15   .word 0x9c90002b | 15<<15
#define DCIVAC_X16   .word 0x9c90002b | 16<<15
#define DCIVAC_X17   .word 0x9c90002b | 17<<15
#define DCIVAC_X18   .word 0x9c90002b | 18<<15
#define DCIVAC_X19   .word 0x9c90002b | 19<<15
#define DCIVAC_X20   .word 0x9c90002b | 20<<15
#define DCIVAC_X21   .word 0x9c90002b | 21<<15
#define DCIVAC_X22   .word 0x9c90002b | 22<<15
#define DCIVAC_X23   .word 0x9c90002b | 23<<15
#define DCIVAC_X24   .word 0x9c90002b | 24<<15
#define DCIVAC_X25   .word 0x9c90002b | 25<<15
#define DCIVAC_X26   .word 0x9c90002b | 26<<15
#define DCIVAC_X27   .word 0x9c90002b | 27<<15
#define DCIVAC_X28   .word 0x9c90002b | 28<<15
#define DCIVAC_X29   .word 0x9c90002b | 29<<15
#define DCIVAC_X30   .word 0x9c90002b | 30<<15
#define DCIVAC_X31   .word 0x9c90002b | 31<<15




#define L2BCR0_IA         0x008
#define L2BCR1_IA         0x009
#define L2CPMR_IA         0x500
#define L2CR0_IA          0x000
#define L2DCRDR_IA        0x300
#define L2DCRTR0_IA       0x302
#define L2DCRTR1_IA       0x303
#define L2EAR_IA          0x20E
#define L2ECR0_IA         0x200
#define L2ECR1_IA         0x201
#define L2ESR0_IA         0x204
#define L2ESRS0_IA        0x205
#define L2ESYNR0_IA       0x208
#define L2ESYNR1_IA       0x209
#define L2ESYNR2_IA       0x20A
#define L2ESYNR3_IA       0x20B
#define L2FASTPCCMDR_IA   0x608
#define L2FASTPCDATAR_IA  0x609
#define L2LKCR_IA         0x100
#define L2SWDR0_IA        0x600
#define L2SWDR1_IA        0x601
#define L2SWDR2_IA        0x602
#define L2SWDR3_IA        0x603
#define L2SWDR4_IA        0x604
#define L2SWDR5_IA        0x605
#define L2SWDR6_IA        0x606
#define L2SWDR7_IA        0x607
#define L2VRF0_IA         0x700
#define L2VRF2_IA         0x702
#define L2ACDDCR_IA       0x588
#define L2ACDDIAGCTR_IA   0x587
#define L2ACDDVMFIFO_IA   0x586
#define L2ACDDVMLC_IA     0x585
#define L2ACDSR_IA        0x583
#define L2CPMSPARE0_IA    0x591
#define L2NMOCFGR_IA      0x595
#define L2NMOCR_IA        0x594
#define L2PSCSR_IA        0x592
#define L2ZMCR_IA         0x590
#define L2PMACTLR_IA      0x40D
#define L2PMACCNTCR_IA    0x409
#define L2PMACCNTR_IA     0x40A
#define L2PMACCNTSR_IA    0x40C
#define L2PMCNTENCLR_IA   0x403
#define L2PMCNTENSET_IA   0x404
#define L2PMCR_IA         0x400
#define L2PMEVCNTCR0_IA   0x420
#define L2PMEVCNTCR1_IA   0x430
#define L2PMEVCNTCR2_IA   0x440
#define L2PMEVCNTCR3_IA   0x450
#define L2PMEVCNTCR4_IA   0x460
#define L2PMEVCNTCR5_IA   0x470
#define L2PMEVCNTCR6_IA   0x480
#define L2PMEVCNTCR7_IA   0x490

#define L2PMEVCNTR0_IA    0x421
#define L2PMEVCNTR1_IA    0x431
#define L2PMEVCNTR2_IA    0x441
#define L2PMEVCNTR3_IA    0x451
#define L2PMEVCNTR4_IA    0x461
#define L2PMEVCNTR5_IA    0x471
#define L2PMEVCNTR6_IA    0x481
#define L2PMEVCNTR7_IA    0x491

#define L2PMEVCNTSR0_IA   0x422
#define L2PMEVCNTSR1_IA   0x432
#define L2PMEVCNTSR2_IA   0x442
#define L2PMEVCNTSR3_IA   0x452
#define L2PMEVCNTSR4_IA   0x462
#define L2PMEVCNTSR5_IA   0x472
#define L2PMEVCNTSR6_IA   0x482
#define L2PMEVCNTSR7_IA   0x492

#define L2PMEVFILTER0_IA  0x423
#define L2PMEVFILTER1_IA  0x433
#define L2PMEVFILTER2_IA  0x443
#define L2PMEVFILTER3_IA  0x453
#define L2PMEVFILTER4_IA  0x463
#define L2PMEVFILTER5_IA  0x473
#define L2PMEVFILTER6_IA  0x483
#define L2PMEVFILTER7_IA  0x493

#define L2PMEVTYPER0_IA   0x424
#define L2PMEVTYPER1_IA   0x434
#define L2PMEVTYPER2_IA   0x444
#define L2PMEVTYPER3_IA   0x454
#define L2PMEVTYPER4_IA   0x464
#define L2PMEVTYPER5_IA   0x474
#define L2PMEVTYPER6_IA   0x484
#define L2PMEVTYPER7_IA   0x494

#define L2PMINTENCLR_IA   0x405
#define L2PMINTENSET_IA   0x406
#define L2PMOVSCLR_IA     0x407
