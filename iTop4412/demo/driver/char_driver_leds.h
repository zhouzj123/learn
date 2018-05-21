#ifndef _CHAR_DRIVER_LEDS_H_
#define _CHAR_DRIVER_LEDS_H_

#define DEVICE_NAME "chardevnode"
#define DEVICE_MINOR_NUM 2
#define DEV_MAJOR 0
#define DEV_MINOR 0
#define REGDEV_SIZE 3000

struct reg_dev{
	char *data;
	unsigned long size;
	struct cdev cdev;
};
#endif
