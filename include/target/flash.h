#ifndef __FLASH_H_INCLUDE__
#define __FLASH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/mtd.h>

typedef uint8_t flash_bid_t;

#include <driver/flash.h>

#ifdef CONFIG_FLASH_MAX_BANKS
#define FLASH_MAX_BANKS			CONFIG_FLASH_MAX_BANKS
#else
#define FLASH_MAX_BANKS			1
#endif

#define INVALID_FLASH_BID		FLASH_MAX_BANKS

mtd_t flash_register_bank(mtd_addr_t addr, mtd_size_t size);

#endif /* __FLASH_H_INCLUDE__ */
