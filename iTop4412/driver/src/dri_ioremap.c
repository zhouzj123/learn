#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

volatile unsigned long virt_addr, phys_addr;
volatile unsigned long *GPL2CON, *GPL2DAT, *GPL2PUD;

void gpl2_device_init(void)
{
	phys_addr = 0x11000100;

	virt_addr = (unsigned long)ioremap(phys_addr, 0x10);

	GPL2CON = (unsigned long*)(virt_addr + 0x00);
	GPL2DAT = (unsigned long*)(virt_addr + 0x04);
	GPL2PUD = (unsigned long*)(virt_addr + 0x08);
}

void gpl2_configure(void)
{
	*GPL2CON &= 0xfffffff1;
	*GPL2CON |= 0x00000001;
	*GPL2PUD |= 0x0003;
}

void gpl2_on(void)
{
	*GPL2DAT |= 0x01;
}

void gpl2_off(void)
{
	*GPL2DAT &= 0xfe;
}

static int led_gpl2_init(void)
{
	printk(KERN_EMERG"init\n");
	gpl2_device_init();
	gpl2_configure();
	gpl2_on();
	printk(KERN_EMERG"led gpl2 open\n");
	return 0;
}

static void __exit led_gpl2_exit(void)
{
	printk(KERN_EMERG"exit\n");
	gpl2_off();
	printk(KERN_EMERG"led gpl2 close\n");
}

module_init(led_gpl2_init);
module_exit(led_gpl2_exit);

