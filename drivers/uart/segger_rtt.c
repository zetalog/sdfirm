/* Additional information:
 *   H->T    Host to target communication
 *   T->H    Target to host communication
 *   RTT channel 0 is always present and reserved for Terminal usage.
 *   Name is fixed to "Terminal"
 *   Effective buffer size: len - 1
 *   WrOff == RdOff:       Buffer is empty
 *   WrOff == (RdOff - 1): Buffer is full
 *   WrOff >  RdOff:       Free space includes wrap-around
 *   WrOff <  RdOff:       Used space includes wrap-around
 *   (WrOff == (len - 1)) && (RdOff == 0):  
 *                         Buffer full and wrap-around after next byte
 */
#include <string.h>
#include <target/generic.h>
#include <target/irq.h>
#include <driver/segger_rtt.h>

/* Max. number of up-buffers (T->H) available on this target. */
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS		(2)
/* Max. number of down-buffers (H->T) available on this target. */
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS		(2)
/* Size of the buffer for terminal output of target, up to host */
#define BUFFER_SIZE_UP				(1024)
/* Size of the buffer for terminal input to target from host (Usually
 * keyboard input).
 */
#define BUFFER_SIZE_DOWN			(16)

/* Mode for pre-initialized terminal channel. */
#define SEGGER_RTT_MODE_DEFAULT			SEGGER_RTT_MODE_NO_BLOCK_SKIP

/* Operating modes. Define behavior if buffer is full (not enough space for
 * entire message).
 */
/* Skip. Do not block, output nothing. (Default) */
#define SEGGER_RTT_MODE_NO_BLOCK_SKIP		0
/* Trim: Do not block, output as much as fits. */
#define SEGGER_RTT_MODE_NO_BLOCK_TRIM		1
/* Block: Wait until there is space in the buffer. */
#define SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL	2
#define SEGGER_RTT_MODE_MASK			3

/* Circular buffer used as up-buffer (T->H). */
typedef struct {
	/* Optional name. Standard names so far are: "Terminal", "SysView",
	 * "J-Scope_t4i4".
	 */
	const char *name;
 	/* Pointer to start of buffer */
	uint8_t *buf;
	/* Buffer size in bytes. Note that one byte is lost, as this
	 * implementation does not fill up the buffer in order to avoid the
	 * problem of being unable to distinguish between full and empty.
	 */
	rtt_int_t len;
	/* Position of next item to be written by either target. */
  	rtt_int_t WrOff;
	/* Position of next item to be read by host. Must be volatile since
	 * it may be modified by host.
	 */
	volatile rtt_int_t RdOff;
	rtt_int_t flags;
} SEGGER_RTT_BUFFER_UP;

/* Circular buffer used as down-buffer (H->T). */
typedef struct {
	/* Optional name. Standard names so far are: "Terminal", "SysView",
	 * "J-Scope_t4i4".
	 */
	const char *name;
	/* Pointer to start of buffer */
	uint8_t *buf;
	/* Buffer size in bytes. Note that one byte is lost, as this
	 * implementation does not fill up the buffer in order to avoid the
	 * problem of being unable to distinguish between full and empty.
	 */
	rtt_int_t len;
	/* Position of next item to be written by host. Must be volatile
	 * since it may be modified by host.
	 */
	volatile rtt_int_t WrOff;
	/* Position of next item to be read by target (down-buffer). */
	rtt_int_t RdOff;
	rtt_int_t flags;
} SEGGER_RTT_BUFFER_DOWN;

#define SEGGER_RTT_HASDATA(n)			\
	(_SEGGER_RTT.aDown[n].WrOff - _SEGGER_RTT.aDown[n].RdOff)

/* RTT control block which describes the number of buffers available as
 * well as the configuration for each buffer.
 */
typedef struct {
	/* Initialized to "SEGGER RTT" */
	char id[16];
	/* Initialized to SEGGER_RTT_MAX_NUM_UP_BUFFERS (type. 2) */
	int MaxNumUbufs;
	/* Initialized to SEGGER_RTT_MAX_NUM_DOWN_BUFFERS (type. 2) */
	int MaxNumDownBuffers;
	/* Up buffers, transferring information up from target via debug
	 * probe to host.
	 */
	SEGGER_RTT_BUFFER_UP aUp[SEGGER_RTT_MAX_NUM_UP_BUFFERS];
	/* Down buffers, transferring information down from host via debug
	 * probe to target.
	 */
	SEGGER_RTT_BUFFER_DOWN aDown[SEGGER_RTT_MAX_NUM_DOWN_BUFFERS];
} SEGGER_RTT_CB;

static uint8_t segger_term_ids[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F' 
};
/* RTT Control Block and allocate buffers for channel 0 */
SEGGER_RTT_CB _SEGGER_RTT;
static uint8_t segger_up_buf[BUFFER_SIZE_UP];
static uint8_t segger_down_buf[BUFFER_SIZE_DOWN];
static char _ActiveTerminal;

/* Initializes the control block an buffers. May only be called via INIT()
 * to avoid overriding settings.
 */
#define INIT()						\
	do {						\
		if (_SEGGER_RTT.id[0] == '\0')	\
			_DoInit();			\
	} while (0)
static void _DoInit(void)
{
	SEGGER_RTT_CB *p = &_SEGGER_RTT;

	p->MaxNumUbufs	= SEGGER_RTT_MAX_NUM_UP_BUFFERS;
	p->MaxNumDownBuffers	= SEGGER_RTT_MAX_NUM_DOWN_BUFFERS;
	/* Up buffer 0 */
	p->aUp[0].name = "Terminal";
	p->aUp[0].buf = segger_up_buf;
	p->aUp[0].len = sizeof(segger_up_buf);
	p->aUp[0].RdOff = 0u;
	p->aUp[0].WrOff = 0u;
	p->aUp[0].flags = SEGGER_RTT_MODE_DEFAULT;
	/* Down buffer 0 */
	p->aDown[0].name = "Terminal";
	p->aDown[0].buf	= segger_down_buf;
	p->aDown[0].len	= sizeof(segger_down_buf);
	p->aDown[0].RdOff= 0u;
	p->aDown[0].WrOff = 0u;
	p->aDown[0].flags = SEGGER_RTT_MODE_DEFAULT;
	/* Finish initialization of the control block. Copy Id string in
	 * three steps to make sure "SEGGER RTT" is not found in
	 * initializer memory (usually flash) by J-Link.
	 */
	strcpy(&p->id[7], "RTT");
	strcpy(&p->id[0], "SEGGER");
	p->id[6] = ' ';
}

/* Initializes the RTT Control Block. Should be used in RAM targets, at
 * start of the application.
 */
void SEGGER_RTT_Init(void)
{
	_DoInit();
}

static rtt_int_t _GetAvailWriteSpace(SEGGER_RTT_BUFFER_UP *ring)
{
	rtt_int_t RdOff;
	rtt_int_t WrOff;
	rtt_int_t r;

	/* Avoid warnings regarding volatile access order. It's not a
	 * problem in this case, but dampen compiler enthusiasm.
	 */
	RdOff = ring->RdOff;
	WrOff = ring->WrOff;
	if (RdOff <= WrOff)
		r = ring->len - 1 - WrOff + RdOff;
	else
		r = RdOff - WrOff - 1;
	return r;
}

/* Stores a specified number of characters in SEGGER RTT ring buffer and
 * updates the associated write pointer which is periodically read by the
 * host.
 * The caller is responsible for managing the write chunk sizes as
 * _WriteBlocking() will block until all data has been posted successfully.
 */
static rtt_int_t _WriteBlocking(SEGGER_RTT_BUFFER_UP *ring,
				const uint8_t *buf, rtt_int_t len)
{
	rtt_int_t bytes;
	rtt_int_t res = 0;
	rtt_int_t RdOff;
	rtt_int_t WrOff;

	/* Write data to buffer and handle wrap-around if necessary */
	WrOff = ring->WrOff;
	do {
		/* May be changed by host (debug probe) in the meantime */
		RdOff = ring->RdOff;
		if (RdOff > WrOff)
			bytes = RdOff - WrOff - 1;
		else
			bytes = ring->len - (WrOff - RdOff + 1);
		/* Number of bytes that can be written until buffer
		 * wrap-around.
		 */
		bytes = min(bytes, (ring->len - WrOff));
		bytes = min(bytes, len);
		memcpy(ring->buf + WrOff, buf, bytes);
		res += bytes;
		buf += bytes;
		len -= bytes;
		WrOff += bytes;
		if (WrOff == ring->len)
			WrOff = 0;
		ring->WrOff = WrOff;
	} while (len);
	return res;
}

/* Stores a specified number of characters in SEGGER RTT ring buffer and
 * updates the associated write pointer which is periodically read by the
 * host.
 * It is callers responsibility to make sure data actually fits in buffer.
 */
static void _WriteNoCheck(SEGGER_RTT_BUFFER_UP *ring,
			  const uint8_t *buf, rtt_int_t len)
{
	rtt_int_t bytes;
	rtt_int_t WrOff;
	rtt_int_t rem;

	WrOff = ring->WrOff;
	rem = ring->len - WrOff;
	if (rem > len) {
		/* All data fits before wrap around */
		memcpy(ring->buf + WrOff, buf, len);
		ring->WrOff = WrOff + len;
	} else {
		/* We reach the end of the buffer, so need to wrap
		 * around.
		 */
		bytes = rem;
		memcpy(ring->buf + WrOff, buf, bytes);
		bytes = len - rem;
		memcpy(ring->buf, buf + rem, bytes);
		ring->WrOff = bytes;
	}
}

/* Reads characters from SEGGER real-time-terminal control block which have
 * been previously stored by the host.
 * Do not lock against interrupts and multiple access.
 */
size_t SEGGER_RTT_ReadNoLock(uint8_t index, void *data, size_t size)
{
	rtt_int_t rem;
	rtt_int_t res = 0;
	rtt_int_t RdOff;
	rtt_int_t WrOff;
	uint8_t *buf = (uint8_t *)data;
	SEGGER_RTT_BUFFER_DOWN *ring;

	INIT();
	ring = &_SEGGER_RTT.aDown[index];
	RdOff = ring->RdOff;
	WrOff = ring->WrOff;

	/* Read from current read position to wrap-around of buffer,
	 * first.
	 */
	if (RdOff > WrOff) {
		rem = ring->len - RdOff;
		rem = min(rem, size);
		memcpy(buf, ring->buf + RdOff, rem);
		res = rem;
		buf += rem;
		size -= rem;
		RdOff += rem;
		/* Handle wrap-around of buffer */
		if (RdOff == ring->len)
			RdOff = 0;
	}
	/* Read remaining items of buffer */
	rem = WrOff - RdOff;
	rem = min(rem, size);
	if (rem > 0) {
		memcpy(buf, ring->buf + RdOff, rem);
		res += rem;
		buf += rem;
		size -= rem;
		RdOff += rem;
	}
	if (res)
		ring->RdOff = RdOff;
	return (size_t)res;
}

/* Stores a specified number of characters in SEGGER RTT control block
 * which is then read by the host.
 * SEGGER_RTT_WriteNoLock does not lock the application.
 */
size_t SEGGER_RTT_WriteNoLock(uint8_t index, const void *data, size_t size)
{
	rtt_int_t res = 0;
	rtt_int_t avail;
	const uint8_t *buf = (const uint8_t *)data;
	SEGGER_RTT_BUFFER_UP *ring = &_SEGGER_RTT.aUp[index];

	switch (ring->flags) {
	case SEGGER_RTT_MODE_NO_BLOCK_SKIP:
		/* If we are in skip mode and there is no space for the
		 * whole of this output, don't bother.
		 */
		avail = _GetAvailWriteSpace(ring);
		if (avail >= size) {
			res = size;
			_WriteNoCheck(ring, buf, size);
		}
		break;
	case SEGGER_RTT_MODE_NO_BLOCK_TRIM:
		/* If we are in trim mode, trim to what we can output
		 * without blocking.
		 */
		avail = _GetAvailWriteSpace(ring);
		res = avail < size ? avail : size;
		_WriteNoCheck(ring, buf, res);
		break;
	case SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL:
		/* If we are in blocking mode, output everything. */
		res = _WriteBlocking(ring, buf, size);
		break;
	default:
		break;
	}
	return (size_t)res;
}

/* Switch terminal to the given terminal ID.  It is the caller's
 * responsibility to ensure the terminal ID is correct and there is enough
 * space in the buffer for this to complete successfully.
 */
static void _PostTerminalSwitch(SEGGER_RTT_BUFFER_UP *ring, uint8_t tid)
{
	uint8_t ac[2];

	ac[0] = 0xFF;
	/* Caller made already sure that TerminalId does not exceed
	 * our terminal limit.
	 */
	ac[1] = segger_term_ids[tid];
	_WriteBlocking(ring, ac, 2);
}

/* Sets the terminal to be used for output on channel 0. */
int SEGGER_RTT_SetTerminal(uint8_t tid)
{
	uint8_t ac[2];
	SEGGER_RTT_BUFFER_UP *ring;
	rtt_int_t avail;
	unsigned long flags;
	int res = 0;

	INIT();
	if (tid >= sizeof(segger_term_ids))
		return -1;

	ac[0] = 0xFFu;
	ac[1] = segger_term_ids[tid];
    	/* Buffer 0 is always reserved for terminal I/O, so we can use
	 * index 0 here, fixed.
	 */
	ring = &_SEGGER_RTT.aUp[0];
	irq_local_save(flags);
	/* Lock to make sure that no other task is writing into buffer,
	 * while we are and number of free bytes in buffer does not change
	 * downwards after checking and before writing.
	 */
	if ((ring->flags & SEGGER_RTT_MODE_MASK) ==
	    SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL) {
		_ActiveTerminal = tid;
		_WriteBlocking(ring, ac, 2);
	} else {
		/* Skipping mode or trim mode? => We cannot trim this
		 * command so handling is the same for both modes.
		 */
		avail = _GetAvailWriteSpace(ring);
		if (avail < 2) {
			res = -1;
			goto exit_unlock;
		}
		/* Only change active terminal in case of success. */
		_ActiveTerminal = tid;
		_WriteNoCheck(ring, ac, 2);
	}

exit_unlock:
	irq_local_restore(flags);
	return res;
}

/* Writes a string to the given terminal without changing the terminal for
 * channel 0. Need to change terminal.
 */
int SEGGER_RTT_TerminalOut(uint8_t tid, const char *s)
{
	int res = -1;
	rtt_int_t len;
	rtt_int_t avail;
	SEGGER_RTT_BUFFER_UP *ring = &_SEGGER_RTT.aUp[0];
	unsigned long flags;

	INIT();
	if (tid >= sizeof(segger_term_ids))
		return -1;

	len = strlen(s);

	irq_local_save(flags);
	avail = _GetAvailWriteSpace(ring);
	switch (ring->flags & SEGGER_RTT_MODE_MASK) {
	case SEGGER_RTT_MODE_NO_BLOCK_SKIP:
		/* If we are in skip mode and there is no space for the
		 * whole of this output, don't bother switching terminals
		 * at all.
		 */
		res = 0;
		if (avail >= (len + 4)) {
			_PostTerminalSwitch(ring, tid);
			res = (int)_WriteBlocking(ring, (uint8_t *)s, len);
			_PostTerminalSwitch(ring, _ActiveTerminal);
		}
		break;
	case SEGGER_RTT_MODE_NO_BLOCK_TRIM:
		/* If we are in trim mode and there is not enough space for
		 * everything, trim the output but always include the
		 * terminal switch.  If no room for terminal switch, skip
		 * that totally.
		 */
		if (avail < 4)
			goto exit_unlock;
		_PostTerminalSwitch(ring, tid);
		res = (int)_WriteBlocking(ring, (uint8_t *)s,
			(len < (avail - 4)) ? len : (avail - 4));
		_PostTerminalSwitch(ring, _ActiveTerminal);
		break;
	case SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL:
		/* If we are in blocking mode, output everything. */
		_PostTerminalSwitch(ring, tid);
		res = (int)_WriteBlocking(ring, (uint8_t *)s, len);
		_PostTerminalSwitch(ring, _ActiveTerminal);
		break;
	default:
		break;
	}

exit_unlock:
	irq_local_restore(flags);
	return res;
}
