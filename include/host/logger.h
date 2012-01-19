#ifndef __LOGGER_H_INCLUDE__
#define __LOGGER_H_INCLUDE__

#define LOG_NONE	0x00
#ifndef LOG_ERROR
#define LOG_ERROR	0x01
#endif
#ifndef LOG_INFO
#define LOG_INFO	0x02
#endif
#ifndef LOG_DEBUG
#define LOG_DEBUG	0x03
#endif

void log_exit(void);
int log_init(void);
void log_set_level(int level);
int log_name2level(const char *name);

void log_dump(const char *prefix, uint8_t *data, int len);
void log_msg(int level, const char *format, ...);

#endif /* __LOGGER_H_INCLUDE__ */
