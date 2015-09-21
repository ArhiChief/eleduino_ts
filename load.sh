#!/bin/sh

modprobe -r usbhid
insmod ./eleduino_ts.ko
modprobe usbhid