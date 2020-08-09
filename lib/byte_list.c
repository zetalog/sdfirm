#include <target/byte_list.h>

int bl_init_list(byte_list_t *bl, uint8_t *src_array, uint32_t max_entries)
{
	uint32_t i;

	/* max_entries prototype in uint8_t */
	if (max_entries == 0 || max_entries >= 0xFF)
		return -EINVAL;
	bl->store = src_array;
	bl->max_entries = (uint8_t)max_entries;
	bl->num_entries = 0;
	bl->currt_entry = 0;
	bl->scan_complete = true;
	for (i = 0; i < max_entries; ++i)
		bl->store[i] = 0;
	return 0;
}

void bl_init_list_empty(byte_list_t *bl)
{
	bl->num_entries = 0;
	bl->currt_entry = 0;
	bl->scan_complete = true;
}

int bl_add_to_list(byte_list_t *bl, uint8_t entry)
{
	uint32_t entry_num;
	uint32_t num_entries;

	/* Make sure the list is not already full. */
	if (bl->num_entries >= bl->max_entries)
		return -ERANGE;
	num_entries = bl->num_entries;
	entry_num = num_entries;
	++(bl->num_entries);
	bl->store[entry_num] = entry;
	bl->scan_complete = false;
	return 0;
}

void bl_copy_list(byte_list_t *src, byte_list_t *dst)
{
	uint32_t i;

	/* Duplicates scan state. */
	dst->num_entries = src->num_entries;
	dst->currt_entry = src->currt_entry;
	dst->scan_complete = src->scan_complete;

	for (i = 0; i < src->num_entries; ++i)
		dst->store[i] = src->store[i];
}

/****************************************************************************
 * This function returns the next element from a list during a sequential
 * scan of the list. If the last entry has already been returned, the call
 * will not return a valid entry but instead will trigger reporting the
 * completion of the scan. Use bl_scan_complete() following this call to
 * determine when a scan has completed. The scan is non-destructive and
 * bl_scan_reset() can be used at anytime to terminate an ongoing or
 * complete scan and begin a new scan.
 * The next element in the scan of the list.
 * Notes: Completed scan state persists until a new elemnt is added to the
 *        list or the scan is reset.
 ****************************************************************************/
uint8_t bl_get_next_entry(byte_list_t *bl)
{
	uint8_t entry;

	if (bl->currt_entry >= bl->num_entries) {
		bl->scan_complete = true;
		/* The last get retrieved the last entry so the scan is
		 * finished.
		 */
		return 0;
	}

	entry = bl->store[bl->currt_entry];
	++(bl->currt_entry);
	return entry;
}

/****************************************************************************
 * This function check to see if the scan of a byte list has completed.
 * Notes: Completed scan state is persistent so bl_scan_reset() must be
 *        called to rescan a list. Zero length lists always show scan as
 *        complete. Zero length lists always show scan as complete.
 ****************************************************************************/
bool bl_scan_complete(byte_list_t *bl)
{
	if (bl->num_entries != 0)
		return bl->scan_complete;
	else
		return true;
}

uint32_t bl_get_num_entries(byte_list_t *bl)
{
	return bl->num_entries;
}

/****************************************************************************
 * This function resets the scan of a byte list to the start of the list. It
 * must be used to rescan a list after a scan is completed.
 * Notes: Completed scan state persists until a new elemnt is added to the
 *        list or the scan is reset.
 ****************************************************************************/
void bl_scan_reset(byte_list_t *bl)
{
	if (bl->num_entries != 0) {
		bl->currt_entry = 0;
		bl->scan_complete = false;
	}
}
