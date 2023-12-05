#include <spacemit_lpc.h>

uint8_t lpc_get_config(void)
{
	return spacemit_lpc_get_config;
}

uint8_t lpc_set_config(uint8_t byte)
{
	return spacemit_lpc_set_config(byte);
}

uint8_t lpc_read_start(void)
{
	return spacemit_lpc_read_start;
}

uint8_t lpc_write_start(void)
{
	return spacemit_lpc_write_start;
}

uint8_t lpc_get_status(void)
{
	return spacemit_lpc_get_status;
}

uint8_t lpc_get_int_mask(void)
{
	return spacemit_lpc_get_int_mask;
}

uint8_t lpc_set_int_mask(uint8_t byte)
{
	return spacemit_lpc_set_int_mask(byte);
}

uint8_t lpc_get_int_status(void)
{
	return spacemit_get_int_status;
}

uint8_t lpc_get_int_raw_status(void)
{
	return spacemit_get_int_raw_status;
}

uint8_t lpc_int_clear(uint8_t byte)
{
	return spacemit_lpc_int_clear(byte);
}

uint8_t lpc_get_wait_count(void)
{
	return spacemit_lpc_get_wait_count;
}

uint8_t lpc_set_wait_count(uint8_t byte)
{
	return spacemit_lpc_set_wait_count(byte);
}

uint8_t lpc_get_addr(void)
{
	return spacemit_lpc_get_addr;
}

uint8_t lpc_set_addr(uint8_t byte)
{
	return spacemit_lpc_set_addr(byte);
}

uint8_t lpc_write_data(uint8_t byte)
{
	return spacemit_lpc_write_data(byte);
}

uint8_t lpc_read_data(void)
{
	return spacemit_lpc_read_data;
}

uint8_t lpc_get_debug(void)
{
	return spacemit_lpc_get_debug;
}

uint8_t lpc_get_serirq_cfg(void)
{
	return spacemit_lpc_get_serirq_cfg;
}

uint8_t lpc_set_serirq_cfg(uint8_t byte)
{
	return spacemit_lpc_set_serirq_cfg(byte);
}

uint8_t lpc_set_serirq_op(uint8_t byte)
{
	return spacemit_lpc_set_serirq_op(byte);
}

uint8_t lpc_get_serirq_slot_mask(void)
{
	return spacemit_lpc_get_serirq_slot_mask;
}

uint8_t lpc_set_serirq_slot_mask(uint8_t byte)
{
	return spacemit_lpc_set_serirq_slot_mask(byte);
}

uint8_t lpc_get_serirq_slot_irq(void)
{
	return spacemit_lpc_get_serirq_slot_irq;
}

uint8_t lpc_set_serirq_slot_clr(uint8_t byte)
{
	return spacemit_lpc_set_serirq_slot_clr(byte);
}

uint8_t lpc_get_serirq_debug(void)
{
	return spacemit_lpc_get_serirq_debug;
}

uint8_t lpc_get_mem_cfg(void)
{
	return spacemit_lpc_get_mem_cfg;
}

uint8_t lpc_set_mem_cfg(uint8_t byte)
{
	return spacemit_lpc_set_mem_cfg(byte);
}

uint8_t lpc_get_err_addr(void)
{
	return spacemit_lpc_get_err_addr;
}