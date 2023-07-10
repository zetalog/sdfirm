/*
 * $Id$
 */
#ifndef _TIMING_H
#define _TIMING_H

double	Delta(void);
double	Now(void);
void	adjust(int usec);
void	context(uint64 xfers);
uint64	delta(void);
int	get_enough(int);
uint64	get_n(void);
double	l_overhead(void);
char	last(char *s);
void	latency(uint64 xfers, uint64 size);
void	nano(char *s, uint64 n);
uint64	now(void);
void	ptime(uint64 n);
void	rusage(void);
void	save_n(uint64);
void	settime(uint64 usecs);
void	start();
uint64	stop();
uint64	t_overhead(void);
double	timespent(void);
uint64	tvdelta(uint64 , uint64 );
void	tvsub(uint64 *tdiff, uint64 t1, uint64 t0);
void	use_int(int result);
void	use_pointer(void *result);
uint64	usecs_spent(void);

#if defined(hpux) || defined(__hpux)
int	getpagesize();
#endif

#endif /* _TIMING_H */
