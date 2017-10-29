KDIR=/home/zhouzj/work/atmel/source/linux-at91-linux-3.10-at91/
PWD:=$(shell pwd)
obj-m := memdrv.o
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
