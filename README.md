#Eleduino_TS

##Introduction

eleduino_ts is USB Touchscreen driver for Eleduino 7 inch tft lcd display with touchscreen.

Several months ago I bought this [touchscreen](http://www.eleduino.com/7-0-inch-800x480-Hdmi-touch-with-USB-touch-Display-Support-Raspberry-pi-Banana-Pi-Banana-Pro-Beagle-p10442.html) for my RPi B+. It's works fine with operation system images what vendor provides with it.

I asked vendors to give me driver source code, or any help to make it works on my custom kernel, but without any result. So, I decide to throw this cool lcd display into trash, but...

##What is this

This device is 7 inch TFT LCD Display with touchscreen and HDMI video interface. I found what the touchscreen based on FT5206 chip. This chip gives I2C or RS232 interface to communicate with host. But the device connect to host using USB. This action simply provides by STM32 microcontroller.

Ok, so far, so good.

USB HID kernel module catch this device and map it on /dev/hidrawX. After unsuccessfull tries to make it works with [tslib](https://github.com/kergoth/tslib), I decide to write my own driver.

Looks like, what vendor stole this touchscreen from FocalTech and make another USB vendor an product ID.

I found FT5206 datasheet and get ft5x06.c in kernel sources. So, my driver is recompilation of ft5x06 Linux driver with adding USB support.

##How it's all works

TODO: Add some notes about it

##Building

Where are two ways to build this kernel module. One way is to build sources on target system (e.g. Raspberry PI). Other way is to build all at host (Your PC with Linux on board) system.

Compile driver for your device. If you build it on your PC, you have to create toolchain for your device. I advise you to use crosstool-ng. You can read about it [here](http://www.bootc.net/archives/2012/05/26/how-to-build-a-cross-compiler-for-your-raspberry-pi/).

Them just run  

               $ CC=arm-unknown-linux-gnueabi-gcc make

If you will build driver on your device, be sure what your system contains installed linux-headers. And just run

               $ make

##Installation
For now, driver is in beta stage, and will only emit mouse left button click on position of your finger. I'll make another futures in the future.

It will works on kernels with version 2.6.32+.

1. Put **eleduino_ts.ko** in **/usr/lib/modules/<kernel_version>/** dirrectory.
2. If you have **modprobed**, put **10-eleduino_ts.conf** inside **/etc/modprobe.d/**. Otherwise, add **modprobe eleduino_ts.ko** at the end of your init script.
3. For now, I have a problems with **usb-hid** kernel module (usb-hid doesn't give alligment of touchscreen for my driver). I'm also working on fixing this bug. Put **10-eleduino_ts.rules** into **/etc/udev/rules.d/** dirrectory, and after full system initialization execute:

	`# echo -n '0eef 0005' > /sys/bus/usb/drivers/eleduino_ts/new_id`
