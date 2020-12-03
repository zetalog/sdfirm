#ifndef __GETOPT_H_INCLUDE__
#define __GETOPT_H_INCLUDE__

#include <errno.h>

#ifdef CONFIG_STRING_GETOPT
char *optarg;
int optind;
int opterr;
int optopt;
void getopt_reset(void);
int getopt(int argc, char *const *argv, const char *optstring);
#else
#define optarg				NULL
#define optind				0
#define opterr				1
#define optopt				'?'
#define getopt_reset()			do { } while (0)
#define getopt(argc, argv, optstring)	(-EINVAL)
#endif

#endif /* __GETOPT_H_INCLUDE__ */
