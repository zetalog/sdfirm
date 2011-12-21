#include <host/missing.h>
#include <host/logger.h>

static int g_log_open = 0;
static int g_log_level = LOG_INFO;

int log_name2level(const char *name)
{
	if (!name)
		return LOG_NONE;
	if (strcmp(name, "debug") == 0)
		return LOG_DEBUG;
	if (strcmp(name, "error") == 0)
		return LOG_ERROR;
	if (strcmp(name, "info") == 0)
		return LOG_INFO;
	return LOG_NONE;
}

void log_set_level(int level)
{
	g_log_level = level;
}

int log_init(void)
{
	g_log_open = 1;
	return 0;
}

void log_exit(void)
{
	g_log_open = 0;
}

#define LOG_BYTES_PER_LINE	10
#define LOG_LINE_OFFSET(i)	(i%LOG_BYTES_PER_LINE)
void log_dump(const char *prefix, uint8_t *data, int len)
{
	int i, j = 1;
	char buff[3*LOG_BYTES_PER_LINE + 1];

	if (0 == g_log_open)
		return;

	memset(buff, 0x00, sizeof (buff));
	for (i = 0; i < len; ) {
		sprintf(buff + 3*LOG_LINE_OFFSET(i), "%02X ", data[i]);
		i++;
		if (LOG_LINE_OFFSET(i) == 0) {
			log_msg(LOG_DEBUG, "(%d) %s %-20s", j, prefix, buff);
			memset(buff, 0x00, sizeof (buff));
			j++;
		}
	}

	if (LOG_LINE_OFFSET(i))
		log_msg(LOG_DEBUG, "(%d) %s %-20s", j, prefix, buff);

}

void log_msg(int level, const char *format, ...)
{
	va_list args;
	FILE *stream = stdout;
	const char *prefix;

	if (g_log_open == 0 || g_log_level < level)
		return;

	switch (level) {
	case LOG_INFO:
		prefix = "info";
		break;
	case LOG_ERROR:
		stream = stderr;
		prefix = "error";
		break;
	case LOG_DEBUG:
		stream = stderr;
		prefix = "debug";
		break;
	default:
		stream = stderr;
		prefix = "unknown";
		break;
	}

#if 1
	fprintf(stream, "test: ");
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
	fprintf(stream, "\r\n");
#endif
}
