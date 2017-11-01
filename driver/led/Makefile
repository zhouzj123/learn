KDIR = /home/zhouzj/work/atmel/source/linux-at91-linux-3.10-at91
obj-m := sama5d36_led.o
PWD=$(shell pwd)
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean