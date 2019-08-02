/*!****************************************************************************
* Filename: spd_simm.h
*
* Description: Provides interfacing and related functionality
*               to make use of the SPD simulation facilities.
*
******************************************************************************/
#ifndef _SPD_SIMM_H
#define _SPD_SIMM_H

/******************************************************************************
* Included Headers
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/
extern bool spd_use_simulated_spd;  // use simulation
extern bool spd_use_simm_in_sil;    // use simulation in Silicon

/******************************************************************************
* Function Prototypes
******************************************************************************/

void spd_simm_init(target_mode_e mode);
unsigned spd_simm_get_spd_block(uint8_t smbus_id, uint8_t spd_bus_addr, uint16_t spd_reg_id, uint32_t xfer_block_size, uint64_t dst_buff_addr);
unsigned spd_simm_get_word(uint8_t smbus_id, uint8_t spd_bus_addr, uint16_t spd_reg_id, uint16_t *spd_data_ptr);
unsigned spd_simm_get_byte(uint8_t smbus_id, uint8_t spd_bus_addr, uint16_t spd_reg_id, uint8_t *spd_data_ptr);
void spd_simm_get_sim_data(target_mode_e mode);
unsigned spd_simm_set_word(uint8_t smbus_id, uint8_t spd_bus_addr, uint16_t spd_reg_id, uint16_t spd_data);
unsigned spd_simm_set_byte(uint8_t smbus_id, uint8_t spd_bus_addr, uint16_t spd_reg_id, uint8_t spd_data);

#endif
