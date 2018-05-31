#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");

static irqreturn_t eint9_interrupt(int irq, void *dev_id)
{
	printk(KERN_EMERG"eint9\n");
	return IRQ_HANDLED;
}

static irqreturn_t eint10_interrupt(int irq, void *dev_id)
{
	printk(KERN_EMERG"eint10\n");
	return IRQ_HANDLED;
}

static int keyirq_probe(struct platform_device *pdev)
{
	printk(KERN_EMERG"probe\n");
	request_irq(IRQ_EINT(9), eint9_interrupt, IRQ_TYPE_EDGE_FALLING, "myeint9", pdev);
	request_irq(IRQ_EINT(10), eint10_interrupt, IRQ_TYPE_EDGE_FALLING, "myeint10", pdev);
	return 0;
}

static int keyirq_remove(struct platform_device *pdev)
{
	printk(KERN_EMERG"remove\n");
	free_irq(IRQ_EINT(9), pdev);
	free_irq(IRQ_EINT(10), pdev);
	return 0;
}

static struct platform_driver keyirq_driver = {
	.probe 		= keyirq_probe,
	.remove 	= keyirq_remove,
	.driver 	= {
		.name 	= DRIVER_NAME,
		.owner 	= THIS_MODULE,
	}
};

static int __init keyirq_init(void)
{
	platform_driver_register(&keyirq_driver);
	return 0;
}

static void __exit keyirq_exit(void)
{
	return platform_driver_unregister(&keyirq_driver);
}

module_init(keyirq_init);
module_exit(keyirq_exit);
