obj-m := dsa_wg_m270.o
ccflags-y := -I$(src)/../include

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

install: all
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules_install

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

probe:
	modprobe mv88e6xxx
	modprobe ./dsa_wg_m270.ko

cycle:
	rmmod dsa_wg_m270
	modprobe mv88e6xxx
	modprobe ./dsa_wg_m270.ko

