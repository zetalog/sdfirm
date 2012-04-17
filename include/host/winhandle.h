#ifndef __WINHANDLE_H_INCLUDE__
#define __WINHANDLE_H_INCLUDE__

#define HANDLE_FLAG_OVERLAPPED 1
#define HANDLE_FLAG_IGNOREEOF 2
#define HANDLE_FLAG_UNITBUFFER 4
struct handle;
typedef int (*handle_inputfn_t)(struct handle *h, void *data, int len);
typedef void (*handle_outputfn_t)(struct handle *h, int new_backlog);

struct handle *handle_input_new(HANDLE handle, handle_inputfn_t gotdata,
				void *privdata, int flags);
struct handle *handle_output_new(HANDLE handle, handle_outputfn_t sentdata,
				 void *privdata, int flags);
int handle_write(struct handle *h, const void *data, int len);
HANDLE *handle_get_events(int *nevents);
void handle_free(struct handle *h);
void handle_got_event(HANDLE event);
void handle_unthrottle(struct handle *h, int backlog);
int handle_backlog(struct handle *h);
void *handle_get_privdata(struct handle *h);

#endif /* __WINHANDLE_H_INCLUDE__ */
