#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "sama5d36_led.h"

#define PIO_PER     0xFFFFFA00
#define PIO_OER     0xFFFFFA10
#define PIO_SODR    0xFFFFFA30
#define PIO_CODR    0xFFFFFA34

#define USER_ROOT_DIR "led"
#define USER_ENTRY    "led_entry"

static struct proc_dir_entry *led_root;
static struct proc_dir_entry *led_entry;

static int led_major = 250;
static int led_minor = 0;
static struct cdev led_cdev;
static void *pio_per, *pio_oer, *pio_sodr, *pio_codr;

static void led_init(void)
{
    writel(readl(pio_per)  | (1 << 24), pio_per);
    writel(readl(pio_oer)  | (1 << 24), pio_oer);
    writel(readl(pio_codr) | (1 << 24), pio_codr);
}

static int sama5d36_led_open(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG"led: device open\n");
    return 0 ;
}

static int sama5d36_led_release(struct inode *inode, struct file *file)
{
    printk(KERN_EMERG"led: device close\n");

    return 0;
}

static long sama5d36_led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd){
    case LED_ON:
        writel(readl(pio_sodr) | (1 << 24), pio_sodr);
        break;
    case LED_OFF:
        writel(readl(pio_codr) | (1 << 24), pio_codr);
        break;
    default:
        printk(KERN_EMERG"cmd invalid\n");
        return -EINVAL;
    }

    return 0;
}

static struct file_operations sama5d36_led_ops = {
    .owner          = THIS_MODULE,
    .open           = sama5d36_led_open,
    .release        = sama5d36_led_release,
    .unlocked_ioctl = sama5d36_led_ioctl
};

static ssize_t proc_led_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
    unsigned char value;
    value = (readl(pio_sodr) >> 24)&0x01;
    printk(KERN_EMERG"LEN %s",value == 1 ? "ON":"OFF");
    return 0;
}

static ssize_t proc_led_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
    char tmbuf[100];
    char *ptr = NULL;
    unsigned char value = 0;
    memset(tmbuf,0,100);

    if(copy_from_user(tmbuf, buf, count)){
        printk(KERN_EMERG"proc_led_write error\n");
        return -1;
    }
    ptr = strstr(tmbuf, "LED");
    if(NULL == ptr){
        printk(KERN_EMERG"cmd invalid\n");
        return -EINVAL;
    }
    value = *(ptr+4) - '0';
    if(1==value)
        writel(readl(pio_sodr)|1<<24, pio_sodr);
    else if(0==value)
        writel(readl(pio_codr)|1<<24, pio_codr);
    else{
        printk(KERN_EMERG"cmd invalid\n");
        return -EINVAL;
    }
    return 0;
}

struct file_operations proc_fops = {
    .read = proc_led_read,
    .write = proc_led_write,
    .owner = THIS_MODULE
};

static int proc_led_init(void)
{
    led_root = proc_mkdir(USER_ROOT_DIR, NULL);
    if(NULL==led_root){
        printk(KERN_EMERG"Create dir /proc/%s error!\n", USER_ROOT_DIR);
        return -1;
    }
    printk(KERN_EMERG"Create dir /proc/%s\n", USER_ROOT_DIR);

    led_entry = proc_create(USER_ENTRY,0666,led_root,&proc_fops);
    if(NULL==led_entry){
        printk(KERN_EMERG"Create entry %s under /proc/%s error!\n", \
               USER_ENTRY, USER_ROOT_DIR);
        goto err_out;
    }
    printk(KERN_EMERG"Create /proc/%s/%s\n",USER_ROOT_DIR, USER_ENTRY);
    return 0;
err_out:
    remove_proc_entry(USER_ROOT_DIR, led_root);
    return -1;
}

static void proc_led_exit(void)
{
    remove_proc_entry(USER_ENTRY, led_root);
    remove_proc_entry(USER_ROOT_DIR, NULL);
    printk(KERN_EMERG"All proc Entry Removed\n");
}

static int led_setup_cdev(struct cdev *cdev, struct file_operations *fops)
{
    int result;

    dev_t devno = MKDEV(led_major, led_minor);
    cdev_init(cdev,fops);
    cdev->owner = THIS_MODULE;
    result = cdev_add(cdev, devno, 1);
    if(result){
        printk(KERN_EMERG"led: cdev add failed\n");
        return result;
    }

    return 0 ;
}

static int __init sama5d36_led_init(void)
{
    int result;
    dev_t devno = MKDEV(led_major, led_minor);

    result = register_chrdev_region(devno, 1, "sama5d36_led");
    if(result){
        printk(KERN_EMERG"led: unable to get major %d\n", led_major);
        return result;
    }

    result = led_setup_cdev(&led_cdev, &sama5d36_led_ops);
    if(result)
        return result;

    pio_per  = ioremap(PIO_PER, 4);
    pio_oer  = ioremap(PIO_OER, 4);
    pio_sodr = ioremap(PIO_SODR,4);
    pio_codr = ioremap(PIO_CODR,4);

    led_init();

    printk(KERN_EMERG"led: driver installed, with major %d\n", led_major);
    proc_led_init();
    return 0;
}

static void __exit sama5d36_led_exit(void)
{
    dev_t devno = MKDEV(led_major, led_minor);

    cdev_del(&led_cdev);
    unregister_chrdev_region(devno, 1);
    iounmap(pio_per);
    iounmap(pio_oer);
    iounmap(pio_sodr);
    iounmap(pio_codr);
    proc_led_exit();
    printk(KERN_EMERG"led: driver uninstalled!\n");
}

module_init(sama5d36_led_init);
module_exit(sama5d36_led_exit);

MODULE_AUTHOR("ZhouZj");
MODULE_LICENSE("GPL v2");
