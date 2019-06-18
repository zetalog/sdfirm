#ifndef __CMDLINE_H_INCLUDE__
#define __CMDLINE_H_INCLUDE__

#include <errno.h>
#include <target/compiler.h>

#define CMD_LINE_SECTION(align)				\
	. = ALIGN((align));				\
	__cmd_start = .;				\
	.cmd.text : {					\
		*(.cmd.text)				\
	}						\
	__cmd_end = .;


#ifndef __ASSEMBLY__
#define DEFINE_COMMAND(name, cmd, help, usage)		\
	int cmd(int, char *[]);				\
	static cmd_tbl __cmd_##name			\
	__attribute__((used,__section__(".cmd.text")))	\
	= { #name, cmd, help, usage }

typedef struct {
	char *name;
	int (*cmd)(int, char *[]);
	char *help;  /* short description */
	char *usage; /* long description */
} cmd_tbl;

#ifdef CONFIG_CONSOLE_COMMAND
int cmd_help(char *cmd);
int cmd_loop(void);
int cmd_init(void);
#else
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
#endif
#endif /* __ASSEMBLY__ */

#endif /* __CMDLINE_H_INCLUDE__ */
