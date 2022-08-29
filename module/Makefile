
obj-m += key.o

KDIR = /lib/modules/$(shell uname -r)/build
ccflags-y := -g

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
