#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/stat.h>
#include <linux/device.h>
#include <linux/moduleparam.h>

#include "char_driver_leds.h"
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-exynos4.h>

MODULE_LICENSE("Dual BSD/GPL");

static int numdev_major = DEV_MAJOR;
static int numdev_minor = DEV_MINOR;

module_param(numdev_major, int, S_IRUSR);
module_param(numdev_minor, int, S_IRUSR);

static int led_gpios[] = {
	EXYNOS4_GPL2(0), EXYNOS4_GPK1(1),
};

#define LED_NUM  ARRAY_SIZE(led_gpios)

static int gpio_init(void)
{
	int i, ret;

	for(i=0; i<LED_NUM; i++){
		ret = gpio_request(led_gpios[i], "LED");
		if(ret){
			printk("%s: request GPIO %d for LED failed, ret = %d\n", DEVICE_NAME,i,ret);
			return -1;
		}else {
			s3c_gpio_cfgpin(led_gpios[i], S3C_GPIO_OUTPUT);
			gpio_set_value(led_gpios[i], 1);
		}
	}
}

static int chardevnode_open(struct inode *inode, struct file *file){
	printk(KERN_EMERG "chardevnode_open is success!\n");
	
	return 0;
}
/*¹Ø±Õ²Ù×÷*/
static int chardevnode_release(struct inode *inode, struct file *file){
	printk(KERN_EMERG "chardevnode_release is success!\n");
	
	return 0;
}

static long chardevnode_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case 0:
		case 1:
			if(arg > LED_NUM)
				return -EINVAL;
			gpio_set_value(led_gpios[arg], cmd);
			break;
		default:
			return -EINVAL;
	}

	printk(KERN_EMERG "chardevnode_ioctl is success! cmd is %d,arg is %d \n",cmd,arg);
	return 0;
}

ssize_t chardevnode_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops){
	return 0;
}

ssize_t chardevnode_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops){
	return 0;
}

loff_t chardevnode_llseek(struct file *file, loff_t offset, int ence){
	return 0;
}

struct file_operations my_fops = {
	.owner 			= THIS_MODULE,
	.open 			= chardevnode_open,
	.release 		= chardevnode_release,
	.unlocked_ioctl = chardevnode_ioctl,
	.read 			= chardevnode_read,
	.write 			= chardevnode_write,
	.llseek 		= chardevnode_llseek,
};

static reg_init_cdev(struct reg_dev *dev, int index)
{
	int err;
	int devno = MKDEV(numdev_major, numdev_minor + index);
	
	cdev_init(&dev->cdev, &my_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &my_fops;
	err = cdev_add(&dev->cdev, devno, 1);

	if(err)
		printk(KERN_EMERG "cdev_add %d is fail! %d\n",index,err);
	else
		printk(KERN_EMERG "cdev_add %d is success!\n",numdev_minor+index);
}

static int __init scdev_init(void)
{
	int ret = 0, i;
	dev_t devno;

	if(numdev_major){
		devno = MKDEV(numdev_major, numdev_minor);
		ret = register_chrdev_region(devno, DEVICE_MINOR_NUM, DEVICE_NAME);
	}else {
		ret = alloc_chrdev_region(&devno, numdev_minor, DEVICE_MINOR_NUM, DEVICE_NAME);
		numdev_major = MAJOR(devno);
		printk(KERN_EMERG "adev_region req %d !\n",numdev_major);
	}
	if(ret < 0)
		printk(KERN_EMERG "register_chrdev_region req %d is failed!\n",numdev_major);

	myclass = class_create(THIS_MODULE, DEVICE_NAME);

	my_devices = kmalloc(DEVICE_MINOR_NUM*sizeof(struct reg_dev), GFP_KERNEL);
	if(!my_devices){
		ret = -ENOMEM;
		goto fail:
	}
	memset(my_devices, 0, DEVICE_MINOR_NUM*sizeof(struct reg_dev));

	for(i=0; i<DEVICE_MINOR_NUM; i++){
		my_devices[i].data = kmalloc(REGDEV_SIZE, GFP_KERNEL);
		memset(my_devices[i].data, 0, REGDEV_SIZE);
		reg_init_cdev(&(my_devices[i].cdev), i);

		device_create(myclass, NULL, MKDEV(numdev_major, numdev_minor + i), NULL, DEVICE_NAME"%d", i);
	}
	ret = gpio_init();
	if(ret)
		printk(KERN_EMERG "gpio_init failed!\n");
	printk(KERN_EMERG "scdev_init!\n");
	return 0;
fail:
	unregister_chrdev_region(MKDEV(numdev_major, numdev_minor), DEVICE_MINOR_NUM);
	printk(KERN_EMERG "kmalloc is fail!\n");
	return ret;	
}

static void __exit scdev_exit(void)
{
	int ret, i;

	for(i=0; i<DEVICE_MINOR_NUM; i++){
		cdev_del(&(my_devices[i].cdev))
		device_destroy(myclass, MKDEV(numdev_major, numdev_minor+i));		
	}
	class_destroy(myclass);
	kfree(my_devices);

	for(i=0; i<LED_NUM; i++){
		gpio_free(led_gpios[i]);
	}
	unregister_chrdev_region(MKDEV(numdev_major, numdev_minor), DEVICE_MINOR_NUM);
}

module_init(scdev_init);
module_exit(scdev_exit);