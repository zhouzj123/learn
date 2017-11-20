DEBUG=y
ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DSCULL_DEBUG
else
	DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)

scull-objs := main.o pipe.o access.o
obj-m := scull.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD		  := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules
endif

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	
depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif