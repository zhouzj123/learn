#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include "memdev.h"

static int mem_major = MEMDEV_MAJOR;

module_param(mem_major, int, S_IRUGO);

struct mem_dev *mem_devp;
struct cdev cdev;

int mem_open(struct inode *inode, struct file *filp)
{
	struct mem_dev *dev;

	int num = MINOR(inode->i_rdev);

	if(num >= MEMDEV_NR_DEVS)
		return -ENODEV;
	dev = &mem_devp[num];

	filp->private_data = dev;

	return 0;
}

int mem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct mem_dev *dev = filp->private_data;

    if(p >= MEMDEV_SIZE)
        return 0;
    if(count > MEMDEV_SIZE-p)
        count = MEMDEV_SIZE-p;

    if(copy_to_user(buf, (void*)(dev->data+p), count)){
        ret = - EFAULT;
    }else{
        *ppos += count;
        ret = count;
        printk(KERN_INFO"read %d bytes from %ld\n",count, p);
    }

    return ret;
}

static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct mem_dev *dev = filp->private_data;

    if(p >= MEMDEV_SIZE)
        return 0;
    if(count > MEMDEV_SIZE-p)
        count = MEMDEV_SIZE-p;

    if(copy_from_user(dev->data+p, buf, count))
        ret = EFAULT;
    else{
        *ppos += count;
        ret = count;

        printk(KERN_INFO"written %d bytes form %ld\n",count , p);
    }
    return ret;
}

static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t newpos;
    switch(whence){
    case SEEK_SET:
        newpos = offset;
        break;
    case SEEK_CUR:
        newpos = filp->f_pos + offset;
        break;
    case SEEK_END:
        newpos = MEMDEV_SIZE-1+offset;
        break;
    default:
        return -EINVAL;
    }

    if((newpos<0) || (newpos>MEMDEV_SIZE))
        return -EINVAL;

    filp->f_pos = newpos;
    return newpos;
}
static const struct file_operations mem_fops = {
    .owner   = THIS_MODULE,
    .llseek  = mem_llseek,
    .read    = mem_read,
    .write   = mem_write,
    .open    = mem_open,
    .release = mem_release
};

static int __init memdev_init(void)
{
	int result;
	int i;

	dev_t devno = MKDEV(mem_major, 0);

	if(mem_major)
		result = register_chrdev_region(devno, 2, "memdev");
	else{
		result = alloc_chrdev_region(&devno, 0, 2, "memdev");
		mem_major = MAJOR(devno);
	}

	if(result < 0)
		return result;

	cdev_init(&cdev, &mem_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &mem_fops;

	cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

	mem_devp = kzalloc(MEMDEV_NR_DEVS*sizeof(struct mem_dev), GFP_KERNEL);
	if(!mem_devp){
		result = -ENOMEM;
		goto fail_malloc;
	}

    for(i=0; i<MEMDEV_NR_DEVS; i++){
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kzalloc(MEMDEV_SIZE, GFP_KERNEL);
    }

    return 0;

fail_malloc:
    unregister_chrdev_region(devno, 1);
    return result;
}

static void __exit memdev_exit(void)
{
    cdev_del(&cdev);
    kfree(mem_devp);
    unregister_chrdev_region(MKDEV(mem_major, 0), 2);
}
module_init(memdev_init);
module_exit(memdev_exit);

MODULE_AUTHOR("ZHOUZJ");
MODULE_LICENSE("GPL v2");
