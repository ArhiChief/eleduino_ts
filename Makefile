 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 #
 # Should you need to contact me, the author, you can do so either by
 # e-mail - mail your message to <arhichief@gmail.com>.
 

# Put "y" to activate debug mode. Put "n" overvise.
DEBUG=y

# Multitouch support for driver
# 	Use "USE_MULTITOUCH=n" to disable multitouch support
#	Use "USE_MULTITOUCH=y" to enable multitouch support. Probavly will not work 
#		if no HID_MULTITOUCH defined in kernel build stage.
USE_MULTITOUCH=n


ifeq ($(DEBUG), y)
	DEBFLAGS = -g -DDEBUG
else
	DEBFLAGS = -O2
endif

ifeq ($(USE_MULTITOUCH), y)
	EXTRA_CFLAGS = -DUSE_MULTITOUCH
endif

EXTRA_CFLAGS += $(DEBFLAGS)

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

install:
	cp -f *.conf /etc/modprobe.d/
	cp -f *.rules /etc/udevd/rules.d/
	cp -f eleduino_ts.ko /usr/lib/modules/$(shell uname -r)/

remove:
	rm -f /etc/modprobe.d/10-eleduino_ts.*
	rm -f /etc/udevd/rules.d/10-eleduino_ts.*
	rm -f /usr/lib/modules/$(shell uname -r)/eleduino_ts.ko
