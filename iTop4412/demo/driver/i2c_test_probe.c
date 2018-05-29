#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <linux/regulator/consumer.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/ft5x0x_touch.h>

static int i2c_test_probe(struct i2c_client *client, const struct i2c_device_id *d)
{
	printk(KERN_EMERG"probe\n");
	return 0;
}

static int __devexit i2c_test_remove(struct i2c_client *client)
{
	i2c_set_clientdata(client, NULL);
	printk(KERN_EMERG"remove\n");
	return 0;
}

static const struct i2c_device_id i2c_test_id[] = {
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
	ret =gpio_request(EXYNOS4_GPL0(2), "TP1_EN");
	if(ret)
		printk(KERN_EMERG"i2c_io_init failed\n");

	gpio_direction_output(EXYNOS4_GPL0(2), 1);
	s3c_gpio_cfgpin(EXYNOS4_GPL0(2), S3C_GPIO_OUTPUT);
	gpio_free(EXYNOS4_GPL0(2));
	mdelay(5);

	ret = gpio_request(EXYNOS4_GPX0(3), "gpx0_3");
	if(ret){
		gpio_free(EXYNOS4_GPX0(3));
		ret = gpio_request(EXYNOS4_GPX0(3), "GPX0_3");
	}
	if(ret)
		printk(KERN_EMERG"request EXYNOS4_GPX0(3) failed");

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

MODULE_AUTHOR("xunwei_rty");
MODULE_DESCRIPTION("TsI2CTest");
MODULE_LICENSE("GPL");


