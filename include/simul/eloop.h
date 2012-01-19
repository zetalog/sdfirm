/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2008
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@hzcnc.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)eloop.h: event loop handler interfaces
 * $Id: eloop.h,v 1.2 2011-04-26 04:23:49 zhenglv Exp $
 */

#ifndef __ELOOP_H_INCLUDE__
#define __ELOOP_H_INCLUDE__

#include <simul/dfastm.h>

typedef struct _eloop_t eloop_t;

/**
 * ELOOP_ALL_CTX - eloop_cancel_timeout() magic number to match all timeouts
 */
#define ELOOP_ALL_CTX (void *) -1

/**
 * eloop_event_t - eloop socket event type for eloop_register_sock()
 * @EVENT_TYPE_READ: Socket has data available for reading
 * @EVENT_TYPE_WRITE: Socket has room for new data to be written
 * @EVENT_TYPE_EXCEPTION: An exception has been reported
 */
typedef enum {
	EVENT_TYPE_READ = 0,
	EVENT_TYPE_WRITE,
	EVENT_TYPE_EXCEPTION
} eloop_event_t;

/**
 * eloop_sock_cb - eloop socket event callback type
 * @sock: File descriptor number for the socket
 * @eloop_ctx: Registered callback context data (eloop_data)
 * @sock_ctx: Registered callback context data (user_data)
 */
typedef void (*eloop_sock_cb)(int sock, void *eloop_ctx, void *sock_ctx);

/**
 * eloop_alarm_cb - eloop timeout event callback type
 * @eloop_ctx: Registered callback context data (eloop_data)
 * @sock_ctx: Registered callback context data (user_data)
 */
typedef void (*eloop_alarm_cb)(void *eloop_data, void *user_ctx);

/**
 * eloop_signal_cb - eloop signal event callback type
 * @sig: Signal number
 * @eloop_ctx: Registered callback context data (global user_data from
 * eloop_init() call)
 * @signal_ctx: Registered callback context data (user_data from
 * eloop_register_signal(), eloop_register_signal_terminate(), or
 * eloop_register_signal_reconfig() call)
 */
typedef void (*eloop_signal_cb)(int sig, void *eloop_ctx, void *signal_ctx);

/**
 * eloop_context_init() - Initialize global event loop data
 * @user_data: Pointer to global data passed as eloop_ctx to signal handlers
 * Returns: 0 on success, -1 on failure
 *
 * This function must be called before any other eloop_* function. user_data
 * can be used to configure a global (to the process) pointer that will be
 * passed as eloop_ctx parameter to signal handlers.
 */
eloop_t *eloop_context_init(void *user_data);

/**
 * eloop_register_read_sock - Register handler for read events
 * @sock: File descriptor number for the socket
 * @handler: Callback function to be called when data is available for reading
 * @eloop_data: Callback context data (eloop_ctx)
 * @user_data: Callback context data (sock_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a read socket notifier for the given file descriptor. The handler
 * function will be called whenever data is available for reading from the
 * socket. The handler function is responsible for clearing the event after
 * having processed it in order to avoid eloop from calling the handler again
 * for the same event.
 */
int eloop_register_read_sock(eloop_t *eloop, int sock, eloop_sock_cb handler,
			     void *eloop_data, void *user_data);
int eloop_register_write_sock(eloop_t *eloop, int sock, eloop_sock_cb handler,
			      void *eloop_data, void *user_data);

/**
 * eloop_unregister_read_sock - Unregister handler for read events
 * @sock: File descriptor number for the socket
 *
 * Unregister a read socket notifier that was previously registered with
 * eloop_register_read_sock().
 */
void eloop_unregister_read_sock(eloop_t *eloop, int sock);
void eloop_unregister_write_sock(eloop_t *eloop, int sock);

/**
 * eloop_register_sock - Register handler for socket events
 * @sock: File descriptor number for the socket
 * @type: Type of event to wait for
 * @handler: Callback function to be called when the event is triggered
 * @eloop_data: Callback context data (eloop_ctx)
 * @user_data: Callback context data (sock_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register an event notifier for the given socket's file descriptor. The
 * handler function will be called whenever the that event is triggered for the
 * socket. The handler function is responsible for clearing the event after
 * having processed it in order to avoid eloop from calling the handler again
 * for the same event.
 */
int eloop_register_sock(eloop_t *eloop, int sock, eloop_event_t type,
			eloop_sock_cb handler,
			void *eloop_data, void *user_data);

/**
 * eloop_unregister_sock - Unregister handler for socket events
 * @sock: File descriptor number for the socket
 * @type: Type of event for which sock was registered
 *
 * Unregister a socket event notifier that was previously registered with
 * eloop_register_sock().
 */
void eloop_unregister_sock(eloop_t *eloop, int sock, eloop_event_t type);

/**
 * eloop_register_timeout - Register timeout
 * @secs: Number of seconds to the timeout
 * @usecs: Number of microseconds to the timeout
 * @handler: Callback function to be called when timeout occurs
 * @eloop_data: Callback context data (eloop_ctx)
 * @user_data: Callback context data (sock_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a timeout that will cause the handler function to be called after
 * given time.
 */
int eloop_register_timeout(eloop_t *eloop, int secs,
			   int usecs, eloop_alarm_cb handler,
			   void *eloop_data, void *user_data);

/**
 * eloop_cancel_timeout - Cancel timeouts
 * @handler: Matching callback function
 * @eloop_data: Matching eloop_data or %ELOOP_ALL_CTX to match all
 * @user_data: Matching user_data or %ELOOP_ALL_CTX to match all
 *
 * Cancel matching <handler,eloop_data,user_data> timeouts registered with
 * eloop_register_timeout(). ELOOP_ALL_CTX can be used as a wildcard for
 * cancelling all timeouts regardless of eloop_data/user_data.
 */
void eloop_cancel_timeout(eloop_t *eloop, eloop_alarm_cb handler,
			  void *eloop_data, void *user_data);

/* schedule a event into the eloop queue */
stm_instance_t *eloop_create_automaton(const char name[16],
				       stm_callback_cb func, unsigned long data,
				       const stm_table_t *table, int state);
void eloop_delete_automaton(stm_instance_t *inst);
void eloop_post_event(eloop_t *eloop, stm_instance_t *fsmi,
		      int event, void *data);
void eloop_send_event(eloop_t *eloop, stm_instance_t *fsmi,
		      int event, void *data);
void eloop_cancel_event(eloop_t *eloop, stm_instance_t *fsmi,
			int event, void *data);

/**
 * eloop_register_signal - Register handler for signals
 * @sig: Signal number (e.g., SIGHUP)
 * @handler: Callback function to be called when the signal is received
 * @user_data: Callback context data (signal_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a callback function that will be called when a signal is received.
 * The callback function is actually called only after the system signal
 * handler has returned. This means that the normal limits for sighandlers
 * (i.e., only "safe functions" allowed) do not apply for the registered
 * callback.
 *
 * Signals are 'global' events and there is no local eloop_data pointer like
 * with other handlers. The global user_data pointer registered with
 * eloop_init() will be used as eloop_ctx for signal handlers.
 */
int eloop_register_signal(int sig, eloop_signal_cb handler, void *user_data);

/**
 * eloop_register_signal_terminate - Register handler for terminate signals
 * @handler: Callback function to be called when the signal is received
 * @user_data: Callback context data (signal_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a callback function that will be called when a process termination
 * signal is received. The callback function is actually called only after the
 * system signal handler has returned. This means that the normal limits for
 * sighandlers (i.e., only "safe functions" allowed) do not apply for the
 * registered callback.
 *
 * Signals are 'global' events and there is no local eloop_data pointer like
 * with other handlers. The global user_data pointer registered with
 * eloop_init() will be used as eloop_ctx for signal handlers.
 *
 * This function is a more portable version of eloop_register_signal() since
 * the knowledge of exact details of the signals is hidden in eloop
 * implementation. In case of operating systems using signal(), this function
 * registers handlers for SIGINT and SIGTERM.
 */
int eloop_register_signal_terminate(eloop_signal_cb handler, void *user_data);

/**
 * eloop_register_signal_reconfig - Register handler for reconfig signals
 * @handler: Callback function to be called when the signal is received
 * @user_data: Callback context data (signal_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a callback function that will be called when a reconfiguration /
 * hangup signal is received. The callback function is actually called only
 * after the system signal handler has returned. This means that the normal
 * limits for sighandlers (i.e., only "safe functions" allowed) do not apply
 * for the registered callback.
 *
 * Signals are 'global' events and there is no local eloop_data pointer like
 * with other handlers. The global user_data pointer registered with
 * eloop_init() will be used as eloop_ctx for signal handlers.
 *
 * This function is a more portable version of eloop_register_signal() since
 * the knowledge of exact details of the signals is hidden in eloop
 * implementation. In case of operating systems using signal(), this function
 * registers a handler for SIGHUP.
 */
int eloop_register_signal_reconfig(eloop_signal_cb handler, void *user_data);

/**
 * eloop_run - Start the event loop
 *
 * Start the event loop and continue running as long as there are any
 * registered event handlers. This function is run after event loop has been
 * initialized with event_init() and one or more events have been registered.
 */
void eloop_context_run(eloop_t *eloop);

/**
 * eloop_terminate - Terminate event loop
 *
 * Terminate event loop even if there are registered events. This can be used
 * to request the program to be terminated cleanly.
 */
void eloop_terminate(eloop_t *eloop);

/**
 * eloop_free - Free any resources allocated for the event loop
 *
 * After calling eloop_destroy(), other eloop_* functions must not be called
 * before re-running eloop_init().
 */
void eloop_context_free(eloop_t *eloop);

/**
 * eloop_terminated - Check whether event loop has been terminated
 * Returns: 1 = event loop terminate, 0 = event loop still running
 *
 * This function can be used to check whether eloop_terminate() has been called
 * to request termination of the event loop. This is normally used to abort
 * operations that may still be queued to be run when eloop_terminate() was
 * called.
 */
int eloop_terminated(eloop_t *eloop);

/**
 * eloop_get_user_data - Get global user data
 * Returns: user_data pointer that was registered with eloop_init()
 */
void *eloop_get_user_data(eloop_t *eloop);

#define timeval_before(a, b)					\
	((a)->tv_sec < (b)->tv_sec ||				\
	 ((a)->tv_sec == (b)->tv_sec && (a)->tv_usec < (b)->tv_usec))

#define timeval_sub(a, b, res)					\
	do {							\
		(res)->tv_sec = (a)->tv_sec - (b)->tv_sec;	\
		(res)->tv_usec = (a)->tv_usec - (b)->tv_usec;	\
		if ((res)->tv_usec < 0) {			\
			(res)->tv_sec--;			\
			(res)->tv_usec += 1000000;		\
		}						\
	} while (0)

/*
 * If there are external threads running and these threads hope to exit before
 * eloop, following functions may useful.
 * There are also situations eloop contexts get cross referenced.  In such
 * cases, following functions should also be called by the eloop users to
 * increase/decrease cross reference counters.
 */
void eloop_context_hold(eloop_t *eloop);
void eloop_context_unhold(eloop_t *eloop);

int __init eloop_init(void);

#endif /* __ELOOP_H_INCLUDE__ */
