#include <simul/eloop.h>
#include <stdio.h>

boolean sim_has_event;
uint64_t sim_timer_next;

uint64_t __sim_current_time_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((((uint64_t)tv.tv_sec )*1000) +
		(((uint64_t)tv.tv_usec)/1000));
}

static void __sim_windows_wait_forever(void)
{
	MSG msg;

	sim_has_event = FALSE;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_QUIT) {
			break;
		}
		
		TranslateMessage(&msg) ;
		DispatchMessage(&msg) ;
		if (sim_has_event) {
			/* sim_post_windows_event(&sim_win_event); */
			return;
		}
	}
}

static void __sim_windows_wait_timeout(uint64_t until)
{
	MSG msg;
	HANDLE h;
	int ret, timeout;
#ifdef LATER
	int alt;
#endif

	sim_has_event = FALSE;
	while (1) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (sim_has_event) {
				/* sim_post_windows_event(&sim_win_event); */
				return;
			}
		}
		
		if (until == 0)
			return;

		timeout = (int)(until - __sim_current_time_ms());
		if (timeout <= 0)
			return;
#ifdef LATER
		/* Use an alternative timeout if a timer is pending, in
		 * order to ensure that the timer is not made late by a
		 * long "util" value.
		 */
		alt = sim_timer_next - __sim_current_time_ms();
		if ((alt > 0) && (alt < timeout))
			timeout = alt;
#endif
		
		ret =  MsgWaitForMultipleObjects(0, &h, FALSE,
						 timeout, QS_ALLEVENTS);
		if (ret == WAIT_TIMEOUT) {
			return;
		} else if (ret == -1) {
			fprintf(stderr, "MsgWaitForMultipleObjects failed\n");
		}
	}
}

void sim_windows_wait(boolean forever, uint64_t until)
{
	if (forever) {
		__sim_windows_wait_forever();
	} else {
		__sim_windows_wait_timeout(until);
	}
}
