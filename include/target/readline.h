#ifndef __TESTOS_CUI_H_INCLUDE__
#define __TESTOS_CUI_H_INCLUDE__

/* Configurations */
#define READLINE_BUFFER_SIZE	128

#define CONFIG_SYS_CBSIZE 128
#define CONFIG_CMDLINE_EDITING 1
/* #define CONFIG_AUTO_COMPLETE 1 */

typedef int (*readline_cmd_handler)(char *buffer, int length);

int readline_register_handler(readline_cmd_handler handler);
int readline_async(void);
int readline_init(void);
extern char console_buffer[];
int cli_readline(const char *const prompt);

#endif /* __TESTOS_CUI_H_INCLUDE__ */
