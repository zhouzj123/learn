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

static int read_gpio_open(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG "hello open\n");
	return 0;	
}

static int read_gpio_release(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG "hello release\n");
	return 0;	
}

static long read_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("cmd is %d,arg is %d\n",cmd,arg);

	if(cmd > 1)
		printk(KERN_EMERG "cmd is 0 or 1\n");
	if(arg > 1)
		printk(KERN_EMERG "arg is only 1\n");

	if(cmd == 0){
		return gpio_get_value(EXYNOS4_GPC0(3));
	}
	if(cmd == 1){
		return gpio_get_value(EXYNOS4_GPX0(6));
	}
	return 0;
}

static struct file_operations read_gpio_ops = {
	.owner 			= THIS_MODULE,
	.open 			= read_gpio_open,
	.release 		= read_gpio_release,
	.unlocked_ioctl = read_gpio_ioctl,
};

static struct miscdevice read_gpio_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVICE_NAME,
	.fops  = &read_gpio_ops,
};

static int read_gpio_probe(struct platform_device *pdev)
{
	int ret1, ret2;
	printk(KERN_EMERG "initialized\n");
	ret1 = gpio_request(EXYNOS4_GPC0(3), "SWITCH3");
	ret2 = gpio_request(EXYNOS4_GPX0(6),"SWITCH4");
	
	if(ret1 < 0 || ret2 < 0){
		printk(KERN_EMERG "gpio_request EXYNOS4_GPC0(x)failed!\n");
		return ret;		
	}else {
		s3c_gpio_cfgpin(EXYNOS4_GPC0(3), S3C_GPIO_INPUT);
		s3c_gpio_cfgpin(EXYNOS4_GPX0(6), S3C_GPIO_INPUT);
		
		s3c_gpio_setpull(EXYNOS4_GPC0(3), S3C_GPIO_PULL_NONE);
		s3c_gpio_setpull(EXYNOS4_GPX0(6), S3C_GPIO_PULL_NONE);
	}
	
	misc_register(&read_gpio_dev);
}

static int read_gpio_remove(struct platform_device *pdev)
{
	printk(KERN_EMERG "\tremove\n");
	misc_deregister(&read_gpio_dev);
	return 0;	
}

static void read_gpio_shutdown(struct platform_device *pdev)
{
	;
}

static int read_gpio_suspend(struct platform_device *pdev)
{
	return 0;
}

static int read_gpio_resume(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver read_gpio_driver = {
	.probe 		= read_gpio_probe,
	.remove 	= read_gpio_remove,
	.shutdown 	= read_gpio_shutdown,
	.suspend 	= read_gpio_suspend,
	.resume 	= read_gpio_resume,
	.driver 	= {
		.name 	= DRIVER_NAME,
		.owner 	= THIS_MODULE,
	}
};

static int __init read_gpio_init(void)
{
	int DriverState;
	printk(KERN_EMERG "HELLO WORLD enter!\n");
	DriverState = platform_driver_register(&read_gpio_driver);
	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}

static void __exit read_gpio_exit(void)
{
	printk(KERN_EMERG "HELLO WORLD exit!\n");
	platform_driver_unregister(&read_gpio_driver);
}

module_init(read_gpio_init);
module_exit(read_gpio_exit);
