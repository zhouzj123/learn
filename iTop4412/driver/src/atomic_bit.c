#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include <asm/types.h>
#include <asm/atomic.h>

#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");

unsigned long int value_bit = 0;

static int atomic_bit_open(void)
{
	printk(KERN_EMERG"open\n");
	if(test_bit(0, &value_bit) != 0 )
		return -EBUSY;
	set_bit(0, &value_bit);
	return 0;
}

static int atomic_bit_release(void)
{
	printk(KERN_EMERG"release\n");
	clear_bit(0, &value_bit);
	return 0;
}

static struct file_operations atomic_bit_ops = {
	.open = atomic_bit_open,
	.release = atomic_bit_release,
	.owner = THIS_MODULE,
};

static struct miscdevice atomic_bit_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &atomic_bit_ops,
};

static int atomic_bit_probe(struct platform_device *pdev)
{
	printk(KERN_EMERG"probe\n");
	misc_register(&atomic_bit_device);
	return 0;
}

static int atomic_bit_remove(struct platform_device *pdev)
{
	printk(KERN_EMERG"remove\n");
	misc_deregister(&atomic_bit_device);
	return 0;
}

static struct platform_driver atomic_bit_driver = {
	.probe 			= atomic_bit_probe,
	.remove 		= atomic_bit_remove,
	.driver 		= {
			.name 	= DRIVER_NAME,
			.owner 	= THIS_MODULE,
	}
};

static int __init atomic_bit_init(void)
{
	printk(KERN_EMERG"init\n");
	platform_driver_register(&atomic_bit_driver);
	return 0;
}

static void __exit atomic_bit_exit(void)
{
	printk(KERN_EMERG"exit\n");
	platform_driver_unregister(&atomic_bit_driver);
}

module_init(atomic_bit_init);
module_exit(atomic_bit_exit);
