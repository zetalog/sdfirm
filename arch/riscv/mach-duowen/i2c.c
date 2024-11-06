#include <target/i2c.h>
#include <target/gpio.h>

bool duowen_i2c_initialized = false;

static inline void duowen_i2c_gpio(uint16_t pin, pad_cfg_t pad, uint8_t mux)
{
	gpio_config_mux(GPIO0B, pin, mux);
	gpio_config_pad(GPIO0B, pin, pad, 8);
}

void i2c_hw_ctrl_init(void)
{
	if (!duowen_i2c_initialized) {
		duowen_i2c_gpio(pad_gpio_8, GPIO_PAD_PULL_DOWN,
				TLMM_PAD_FUNCTION);
		duowen_i2c_gpio(pad_gpio_9, GPIO_PAD_PULL_DOWN,
				TLMM_PAD_FUNCTION);
		clk_enable(i2c0_clk + i2c_mid);
		duowen_i2c_initialized = true;
	}
	dw_i2c_master_init();
}
