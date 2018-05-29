#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#include "spidev_test.h"
#include "spidev.h"

static struct spi_device *my_spi;


module_init(my_rc522_init);
module_exit(my_rc522_exit);

MODULE_AUTHOR("zhouzj");
MODULE_LICENSE("GPL");

