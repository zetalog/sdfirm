#include <target/sbi.h>
#include <target/spinlock.h>
#include <target/gpio.h>

DEFINE_SPINLOCK(sbi_pin_lock);

void sbi_pin_config_mux(unsigned long pinid, unsigned long mux)
{
	unsigned short port, pin;
	irq_flags_t flags;

	port = sbi_pin_port(pinid);
	pin = sbi_pin_pin(pinid);
	spin_lock_irqsave(&sbi_pin_lock, flags);
	gpio_config_mux(port, pin, mux);
	spin_unlock_irqrestore(&sbi_pin_lock, flags);
}

void sbi_pin_config_pad(unsigned long pinid, unsigned long cfg)
{
	unsigned short port, pin;
	unsigned long pad;
	unsigned char drive;
	irq_flags_t flags;

	port = sbi_pin_port(pinid);
	pin = sbi_pin_pin(pinid);
	pad = sbi_pad_pad(cfg);
	drive = sbi_pad_drive(cfg);
	spin_lock_irqsave(&sbi_pin_lock, flags);
	gpio_config_pad(port, pin, pad, drive);
	spin_unlock_irqrestore(&sbi_pin_lock, flags);
}
