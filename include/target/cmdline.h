#ifndef __CMDLINE_H_INCLUDE__
#define __CMDLINE_H_INCLUDE__

#include <target/compiler.h>

#define CMDLINE_INPUT(align)				\
	. = ALIGN((align));				\
	__cmd_start = .;				\
	KEEP(*(SORT(.cmd.rodata)))			\
	__cmd_end = .;

#define CMDLINE_SECTION(align)				\
	.cmd.rodata : {					\
		CMDLINE_INPUT(align)			\
	}

#ifndef __ASSEMBLY__
typedef struct {
	char *name;
	int (*cmd)(int, char *[]);
	char *help;  /* short description */
	char *usage; /* long description */
} cmd_tbl;

#define DEFINE_COMMAND(name, cmd, help, usage)			\
	int cmd(int, char *[]);					\
	static cmd_tbl __cmd_##name				\
	__attribute__((used,__section__(".cmd.rodata")))	\
	= { #name, cmd, help, usage }

#ifdef CONFIG_COMMAND
extern cmd_tbl __cmd_start[0];
extern cmd_tbl __cmd_end[0];

int cmd_help(char *cmd);
int cmd_loop(void);
int cmd_init(void);
void cmd_dump_sect(void);
int cmd_parse(char *line, char *argv[]);
int cmd_execute(int argc, char * argv[]);
void cmd_batch(void);
#else
static inline void cmd_batch(void)
{
}
static inline int cmd_execute(int argc, char * argv[])
{
	return -ENODEV;
}
static inline int cmd_parse(char *line, char *argv[])
{
	return -ENODEV;
}
static inline int cmd_help(char *cmd)
{
	return -ENODEV;
}
static inline int cmd_loop(void)
{
	return -ENODEV;
}
static inline int cmd_init(void)
{
	return -ENODEV;
}
static inline void cmd_dump_sect(void)
{
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __CMDLINE_H_INCLUDE__ */
