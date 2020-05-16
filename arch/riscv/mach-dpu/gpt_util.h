#ifndef _GPT_UTIL_H_
#define _GPT_UTIL_H_

int gpt_pgpt_init(uint8_t *image_start);
int gpt_get_part_by_name(uint8_t *part_name, uint32_t *offset, uint32_t *size, uint16_t *pad_size);
int gpt_get_file_by_name(uint8_t *file_name, uint32_t *offset, uint32_t *size);

#endif
