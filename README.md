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

TODO: Add some notes about it