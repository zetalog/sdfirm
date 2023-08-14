#ifndef __CNT_K1MAX_H_INCLUDE__
#define __CNT_K1MAX_H_INCLUDE__

#ifdef CONFIG_K1M_K1X
void cnt_enable_gcounter(void);
int cnt_status_gcounter(void);
#else
#define cnt_enable_gcounter()  do { } while (0)
#define cnt_status_gcounter()  do { } while (0)
#endif

#endif
