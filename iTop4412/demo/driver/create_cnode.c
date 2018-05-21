#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEVICE_NAME "chardevnode"
#define DEVICE_MINOR_NUM 2
#define DEV_MAJOR 0
#define DEV_MINOR 0
#define REGDEV_SIZE 3000

int numdev_major = DEV_MAJOR;
int numdev_minor = DEV_MINOR;
static struct class *myclass;

struct reg_dev {
	char *data;
	unsigned long size;
	struct cdev cdev;
};

struct reg_dev *my_devices;

struct file_operations my_fops = {
	.owner = THIS_MODULE,
};

static void reg_init_cdev(struct reg_dev *dev, int index)
{
	int err;
	int devno = MKDEV(numdev_major, numdev_minor + index);

	cdev_init(&dev->cdev, &my_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &my_fops;

	err = cdev_add(&dev->cdev, devno, 1);
	if(err){
		printk(KERN_EMERG "cdev_add %d is fail! %d\n",index,err);
	}
	else {
		printk(KERN_EMERG "cdev_add %d is success!\n",numdev_minor+index);
	}
}

static int __init scdev_init(void)
{
	int ret = 0, i;
	dev_t num_dev;
	printk(KERN_EMERG"numdev_major = %d\n", numdev_major);
	printk(KERN_EMERG"numdev_minor = %d\n", numdev_minor);

	if(numdev_major){
		num_dev = MKDEV(numdev_major, numdev_minor);
		ret = register_chrdev_region(num_dev, DEVICE_MINOR_NUM, DEVICE_NAME);
	}
	else{
		ret = alloc_chrdev_region(&num_dev, numdev_minor, DEVICE_MINOR_NUM, DEVICE_NAME);
		numdev_major = MAJOR(num_dev);
		printk(KERN_EMERG"adev_region req %d\n", numdev_major);
	}
	if(ret < 0){
		printk(KERN_EMERG"register_chrdev_region req %d is failed\n", numdev_major);
	}
	myclass = class_create(THIS_MODULE, DEVICE_NAME);

	my_devices = kmalloc(DEVICE_MINOR_NUM * sizeof(struct reg_dev), GFP_KERNEL);
	if(!my_devices){
		ret = -ENOMEM;
		goto fail;
	}
	memset(my_devices, 0, DEVICE_MINOR_NUM*sizeof(struct reg_dev));

	for(i=0; i<DEVICE_MINOR_NUM;i++){
		my_devices[i].data = kmalloc(REGDEV_SIZE, GFP_KERNEL);
		memset(my_devices[i].data, 0, REGDEV_SIZE);
		reg_init_cdev(&my_devices[i], i);
		device_create(myclass, NULL, MKDEV(numdev_major, numdev_minor+i), NULL, DEVICE_NAME"%d",i);
	}
	printk(KERN_EMERG"scdev_init\n");
	return 0;

fail:
	unregister_chrdev_region(MKDEV(numdev_major, numdev_minor), DEVICE_MINOR_NUM);
	printk(KERN_EMERG"kmalloc is fail\n");
}

static void __exit scdev_exit(void)
{
	int i;
	printk(KERN_EMERG"scdev_exit\n");

	for(i=0; i<DEVICE_MINOR_NUM; i++){
		cdev_del(&(my_devices[i].cdev));
		device_destroy(myclass, MKDEV(numdev_major, numdev_minor+i));
	}
	class_destroy(myclass);
	unregister_chrdev_region(MKDEV(numdev_major, numdev_minor), DEVICE_MINOR_NUM);
}

module_init(scdev_init);
module_exit(scdev_exit);

