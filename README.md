# Eleduino_TS

## Introduction

Eleduino_TS is USB Touchscreen driver for Eleduino 7 inch tft lcd display with touchscreen.

Several months ago I bought this [touchscreen](http://www.eleduino.com/7-0-inch-800x480-Hdmi-touch-with-USB-touch-Display-Support-Raspberry-pi-Banana-Pi-Banana-Pro-Beagle-p10442.html) for my RPi B+. It's works fine with operation system images what vendor provides with it.

I asked vendors to give me driver source code, or any help to make it works on my custom kernel, but without any result. So, I decide to throw this cool lcd display into trash, but...

## What is this

This device is 7 inch TFT LCD Display with touchscreen and HDMI video interface. I found what the touchscreen based on FT5206 chip. This chip gives I2C or RS232 interface to communicate with host. But the device connect to host using USB. This action simply provides by STM32 microcontroller.

Ok, so far, so good.

USB HID kernel module catch this device and map it on /dev/hidrawX. After unsuccessfull tries to make it works with [tslib](https://github.com/kergoth/tslib), I decide to write my own driver.

Looks like, what vendor stole this touchscreen from FocalTech and make another USB vendor an product ID.

I found FT5206 datasheet and get ft5x06.c in kernel sources. So, my driver is recompilation of ft5x06 Linux driver with adding USB support.

## How it's all works

Touchscreen is contolled by FT5206 processor what handle user's touches and provides output described in **FT5206.pdf**. FT5206 doesn't support USB, so there is STM32 microcontroller, what provides USB interface to FT5206. FT5206 supports gesture identification and some other interesting things, but all of this aren't captured by STM32. It's only sends to host the coordinates of all touchpoints.

The structure of the packet that we can recive on host is something like what:

```
arhichief@robotron $ sudo xxd -c 25 /dev/hidraw4
0000000: aa00 0000 0000 bb00 0000 0000 0000 0000 0000 0000 0000 0000 00  .........................
0000019: aa01 00c5 0134 bb01 0000 0000 0000 0000 0000 0000 0000 0000 cc  .....4...................
```

There is description of this output that can be found at the end of [this](http://www.waveshare.com/w/upload/7/73/7inch-HDMI-LCD-B-UserManual.pdf) document:

|Byte (Hex address)|Description|
|:---------------:|:---------------:|
|**0x00**| The frame header is 0xAA.
|**0x01**| Touch flag (1: valid touch data. 0: The flag of touch finished)
|**0x02**| The x-coordinate high byte of the 1st touch point.
|**0x03**| The x-coordinate low byte of the 1st touch point.
|**0x04**| The y-coordinate high byte of the 1st touch point.
|**0x05**| The y-coordinate low byte of the 1st touch point.
|**0x06**| The 1st touch finished flag is 0xBB. Raspberry Pi only uses the first 7 Bytes.|
|**0x07**| The quantity of touch coordinates (1~5)
|**0x08**| The x-coordinate high byte of the 2nd touch point.|
|**0x09**| The x-coordinate low byte of the 2nd touch point.|
|**0x0A**| The y-coordinate high byte of the 2nd touch point.|
|**0x0B**| The y-coordinate low byte of the 2nd touch point.|
|**0x0C**| The x-coordinate high byte of the 3rd touch point.|
|**0x0D**| The x-coordinate low byte of the 3rd touch point.|
|**0x0E**| The y-coordinate high byte of the 3rd touch point.|
|**0x0F**| The y-coordinate low byte of the 3rd touch point.|
|**0x10**| The x-coordinate high byte of the 4th touch point.|
|**0x12**| The x-coordinate low byte of the 4th touch point.|
|**0x13**| The y-coordinate high byte of the 4th touch point.|
|**0x14**| The y-coordinate low byte of the 4th touch point.|
|**0x15**| The x-coordinate high byte of the 5th touch point.|
|**0x16**| The x-coordinate low byte of the 5th touch point.|
|**0x17**| The y-coordinate high byte of the 5th touch point.|
|**0x18**| The y-coordinate low byte of the 5th touch point.|
|**0x19**| The Frame end is 0xCC.|

## Building

Where are two ways to build this kernel module. One way is to build sources on target system (e.g. Raspberry PI). Other way is to build all at host (Your PC with Linux on board) system.

Compile driver for your device. If you build it on your PC, you have to create toolchain for your device. I advise you to use crosstool-ng. You can read about it [here](http://www.bootc.net/archives/2012/05/26/how-to-build-a-cross-compiler-for-your-raspberry-pi/). 

Them just run  
```
               $ CC=arm-unknown-linux-gnueabi-gcc make
```
			   
If you will build driver on your device, be sure what your system contains installed linux-headers. And just run 
```
               $ make
```

## Installation
For now, driver is in beta stage, and will only emit mouse left button click on position of your finger. I'll make another futures in the future.

It will works on kernels with version 2.6.32+.

1. Put **eleduino_ts.ko** in **/usr/lib/modules/<kernel_version>/** dirrectory. 
2. If you have **modprobed**, put **10-eleduino_ts.rules** inside **/etc/modprobe.d/**. Otherwise, add **modprobe eleduino_ts.ko** at the end of your init script.
3. For now, I have a problems with **usb-hid** kernel module (usb-hid doesn't give alligment of touchscreen for my driver). I'm also working on fixing this bug. Put **10-eleduino_ts.rules** into **/etc/udev/rules.d/** dirrectory, and after full system initialization execute:

```
	# echo -n '0eef 0005' > /sys/bus/usb/drivers/eleduino_ts/new_id
```

# Tasks
- [x] Basic logic. Support left mouse button click at the position of finger touch (v 0.0.1)
- [ ] Multitouch support (v 0.1.0)
- [ ] Gesture identification (v 0.3.0)
- [ ] Custom gestures (v 0.5.0)
- [ ] Simple building and installation scripts (v 1.0.0)