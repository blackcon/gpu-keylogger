obj-m += key.o

KDIR = /usr/src/linux-headers-$(shell uname -r)
#KDIR = /usr/src/linux-headers-3.13.0-24-generic
ccflags-y := -g

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order .*
