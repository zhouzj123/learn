#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include <asm/atomic.h>
#include <asm/types.h>

#define DRIVER_NAME "hello_ctl"
#define DRIVER_DEVICE "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");

static atomic_t value_atomic = ATOMIC_INIT(0);

static int atomic_int_open(void)
{
	printk(KERN_EMERG"open\n");
	if(atomic_read(&value_atomic)){
		return -EBUSY;
	}

	atomic_inc(&value_atomic);
	printk(KERN_EMERG"success\n");
	return 0;
}

static int atomic_int_release(void)
{
	printk(KERN_EMERG"release\n");
	atomic_dec(&value_atomic);
	return 0;
}


static struct file_operations atomic_int_ops = {
	.owner   = THIS_MODULE,
	.open    = atomic_int_open,
	.release = atomic_int_release,
};

static struct miscdevice atomic_int_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DRIVER_DEVICE,
	.fops  = &atomic_int_ops,
};

static int atomic_int_probe(struct platform_device *pdev)
{
	printk(KERN_EMERG"probe\n");
	misc_register(&atomic_int_dev);
	return 0;
}

static int atomic_int_remove(struct platform_device *pdev)
{
	printk(KERN_EMERG"remvoe\n");
	misc_deregister(&atomic_int_dev);
	return 0;
}


static struct platform_driver atomic_int_driver = {
	.probe 		= atomic_int_probe,
	.remove 	= atomic_int_remove,
	.driver 	= {
		.name 	= DRIVER_NAME,
		.owner 	= THIS_MODULE,
	}
};

static int __init atomic_int_init(void)
{
	printk(KERN_EMERG"init\n");
	platform_driver_register(&atomic_int_driver);
	return 0;
}

static void __exit atomic_int_exit(void)
{
	printk(KERN_EMERG"exit\n");
	platform_driver_unregister(&atomic_int_driver);
}

module_init(atomic_int_init);
module_exit(atomic_int_exit);
