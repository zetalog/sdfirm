#ifndef __TESTOS_CMDLINE_H_INCLUDE__
#define __TESTOS_CMDLINE_H_INCLUDE__

#include <errno.h>
#include <target/compiler.h>

#define MK_CMD(name, cmd, help, usage) \
    int cmd(int, char *[]); \
    static cmd_tbl cmd_list2_##name \
    __attribute__((used,section(".init_array.cmd_list2_"#name))) \
    = {#name, cmd, help, usage}

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

#endif /* __TESTOS_CMDLINE_H_INCLUDE__ */
