#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>

#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("ZHOUZJ");


static int hello_open(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG"hello open\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG"hello release\n");
	return 0;
}

static int hello_ioctl(struct file *files, unsigned int cmd, unsigned long args)
{
	printk(KERN_EMERG"cmd is %d, args is %d\n", cmd, args);

	if(cmd > 1){
		printk(KERN_EMERG"cmd is 0 or 1\n");
	}
	if(args > 1){
		printk(KERN_EMERG"args is only 1\n");
	}
	gpio_set_value(EXYNOS4_GPL2(0), cmd);
	return 0;
}

static struct file_operations hello_ops = {
	.owner 			= THIS_MODULE,
	.open 			= hello_open,
	.release 		= hello_release,
	.unlocked_ioctl = hello_ioctl,
};

static struct miscdevice hello_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVICE_NAME,
	.fops  = &hello_ops,
};

static int hello_probe(struct platform_device *pdev)
{
	int ret;
	printk(KERN_EMERG"initialized\n");
	ret = gpio_request(EXYNOS4_GPL2(0), "LEDS");
	if(ret < 0){
		printk(KERN_EMERG"gpio_request EXYNOS4_GPL2(0) failed\n");
		return ret;
	}

	s3c_gpio_cfgpin(EXYNOS4_GPL2(0), S3C_GPIO_OUTPUT);
	gpio_set_value(EXYNOS4_GPL2(0), 0);
	misc_register(&hello_dev);
	return 0;
}


static int hello_remove(struct platform_device *pdev)
{
	printk(KERN_EMERG"remove\n");
	misc_deregister(&hello_dev);
	return 0;
}

static void hello_shutdown(struct platform_device *pdev)
{
	;
}

static int hello_suspend(struct platform_device *pdev)
{
	return 0;
}

static int hello_resume(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver hello_driver = {
	.probe 		= hello_probe,
	.remove 	= hello_remove,
	.shutdown 	= hello_shutdown,
	.suspend 	= hello_suspend,
	.resume 	= hello_resume,
	.driver 	= {
		.name 	= DRIVER_NAME,
		.owner 	= THIS_MODULE,
	}
};

static int __init hello_init(void)
{
	int DriverState;

	printk(KERN_EMERG "hello world enter!\n");
	DriverState = platform_driver_register(&hello_driver);
	printk(KERN_EMERG"DriverState is %d\n", DriverState);

	int ret;
	printk(KERN_EMERG"initialized\n");
	ret = gpio_request(EXYNOS4_GPL2(0), "LEDS");
	if(ret < 0){
		printk(KERN_EMERG"gpio_request EXYNOS4_GPL2(0) failed\n");
		return ret;
	}

	s3c_gpio_cfgpin(EXYNOS4_GPL2(0), S3C_GPIO_OUTPUT);
	gpio_set_value(EXYNOS4_GPL2(0), 0);
	misc_register(&hello_dev);
	
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_EMERG"hello world exit!\n");
	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);