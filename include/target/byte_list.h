#ifndef __BYTE_LIST_H_INCLUDE__
#define __BYTE_LIST_H_INCLUDE__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

/****************************************************************************
 * Description:
 *   Maximum number of byte entries a byte list can contain.
 ****************************************************************************/
/* TODO -GPK-  This could be reduced to 32 to save space. */
/*#define BYTE_LIST_MAX_ENTRIES		32*/

typedef struct {
	uint8_t *store;
	uint8_t max_entries;
	uint8_t num_entries;
	uint8_t currt_entry;
	bool scan_complete;
} byte_list_t;

#define bl_num(bl)	((bl)->num_entries)
#define bl_ent(bl, n)	((bl)->store[n])

int bl_init_list(byte_list_t *bl, uint8_t *src_array, uint32_t num_entries);
int bl_add_to_list(byte_list_t *bl, uint8_t entry);
void bl_copy_list(byte_list_t *src, byte_list_t *dst);
uint8_t bl_get_next_entry(byte_list_t *bl);
bool bl_scan_complete(byte_list_t *bl);
uint32_t bl_get_num_entries(byte_list_t *bl);
void bl_scan_reset(byte_list_t *bl);

#endif /* __BYTE_LIST_H_INCLUDE__ */
