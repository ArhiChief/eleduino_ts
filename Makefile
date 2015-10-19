# Put "y" to activate debug mode. Put "n" overvise.
DEBUG=y

# Put "y" enable multitouch usability. Put "n" overvise.
USE_MULTITOUCH=n



ifeq ($(DEBUG), y)
	DEBFLAGS = -0 -g -PDEBUG
else
	DEBFLAGS = -O2
endif

EXCTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= eleduino_ts.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif

rebuild:
	make clean
	make