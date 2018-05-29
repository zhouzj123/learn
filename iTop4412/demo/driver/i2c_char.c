#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linuxdelay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/regulator/consumer.h>

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/ft5x0x_touch.h>


static struct i2c_client *this_client;

static int i2c_test_read_reg(struct i2c_client *client, u8 addr, u8 *pdata)
{
	u8 buf1[4] = {0};
	u8 buf2[4] = {0};

	struct i2c_msg msgs[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = 1,
			.buf = buf1,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = buf2,
		}
	};

	int ret;
	buf1[0] = addr;
	ret = i2c_transfer(client->adapter, msgs, 2);
	if(ret < 0)
		pr_err("read reg (0x%02x) error, %d\n", addr, ret);
	else
		*pdata = buf2[0];

	return ret;
	
}

static int i2c_test_read_fw_reg(struct i2c_client *client, unsigned char *var)
{
	int ret;
	*val = 0xff;
	ret = i2c_test_read_reg(client, 0xa6, val);
	printk(KERN_EMERG"val = %d\n",*val);
	return ret;
}

static int i2c_open_func(struct file *filp)
{
	printk(KERN_EMERG"open\n");
	return 0;
}

static int i2c_release_func(struct file *filp)
{
	printk(KERN_EMERG"release\n");
	return 0;
}

static ssize_t i2c_read_func(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
	int ret;
	u8 reg_data;

	ret = copy_from_user(&reg_data, buffer, 1);

	struct i2c_msg msgs[] = {
		{
			.addr = this_client->addr,
			.flags = 0,
			.len = 1,
			.buf = &reg_data
		},
		{
			.addr = this_client->addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = &reg_data
		}
	};

	ret = i2c_transfer(this_client->adapter, msgs, 2);
	if(ret < 0)
		pr_err("read err\n");

	ret = copy_to_user(buffer, &reg_data, 1);
	return ret;
}

static ssize_t i2c_write_func(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msgs[1];

	ret = copy_from_user(&buf, buffer, 2);

	msgs[0].addr = this_client->addr;
	msgs[0].flags = 0;
	msgs[0].len = 2;
	msgs[0].buf = buf;

	ret = i2c_transfer(this_client->adapter, msgs, 1);
	if(ret < 0)
		pr_err("write reg 0x%02x error!\n",buf[0]);

	ret = copy_to_user(buffer, buf, 1);
	return ret;
}

static struct file_operations i2c_ops = {
	.owner 		= THIS_MODULE,
	.open 		= i2c_open_func,
	.release 	= i2c_release_func,
	.write 		= i2c_write_func,
	.read 		= i2c_read_func
};

static struct miscdevice i2c_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "i2c_control",
	.fops  = &i2c_ops
};

static int i2c_test_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	unsigned char val;
	printk(KERN_EMERG"probe\n");
	i2c_test_read_fw_reg(client, &val);

	this_client = client;
	misc_register(&i2c_dev);
	
	return 0;
}

static int __devexit i2c_test_remove(struct i2c_client *client)
{
	i2c_set_clientdata(client, NULL);
	misc_deregister(&i2c_dev);
	printk(KERN_EMERG"remove\n");
	return 0;
}

static const struct i2c_device_id i2c_test_id = {
		{"i2c_test", 0},
};

static struct i2c_driver i2c_test_driver = {
	.probe 		= i2c_test_probe,
	.remove 	= __devexit_p(i2c_test_remove),
	.id_table 	= i2c_test_id,
	.driver 	= {
		.name 	= "i2c_test",
		.owner 	= THIS_MODULE,
	}
};

static void i2c_io_init(void)
{
	int ret;
	gpio_free(EXYNOS4_GPL0(2));
	gpio_free(EXYNOS4_GPX0(3));

	ret = gpio_request(EXYNOS4_GPL0(2), "TP1_EN");
	if(ret){
		printk(KERN_EMERG"EXYNOS4_GPL0(2) request failed\n");
		return;
	}
	
	gpio_direction_output(EXYNOS4_GPL0(2), 1);
	s3c_gpio_cfgpin(EXYNOS4_GPL0(2), S3C_GPIO_OUTPUT);
	gpio_free(EXYNOS4_GPL0(2));
	mdelay(5);

	ret = gpio_request(EXYNOS4_GPX0(3), "GPX0_3");
	gpio_direction_output(EXYNOS4_GPX0(3), 0);
	mdelay(200);
	gpio_direction_output(EXYNOS4_GPX0(3), 1);
	s3c_gpio_cfgpin(EXYNOS4_GPX0(3), S3C_GPIO_OUTPUT);
	gpio_free(EXYNOS4_GPX0(3));
	msleep(300);
}

static int __init i2c_test_init(void)
{
	printk(KERN_EMERG"init\n");
	i2c_io_init();
	return i2c_add_driver(&i2c_test_driver);
}

static void __exit i2c_test_exit(void)
{
	printk(KERN_EMERG"exit\n");
	i2c_del_driver(&i2c_test_driver);
}

late_initcall(i2c_test_init);
module_exit(i2c_test_exit);

MODULE_AUTHOR("zhouzj");
MODULE_DESCRIPTION("TsI2CTest");
MODULE_LICENSE("GPL");


