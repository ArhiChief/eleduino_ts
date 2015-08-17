#ifndef _ELEDUINO_TS_H_
#define _ELEDUINO_TS_H_

#define DRIVER_VER "v0.01"
#define DRIVER_AUTHOR "ArhiChief (arhichief@gmail.com)"
#define DRIVER_DESC "USB HID TouchScreen driver for Eleduino 7'' TFT LCD Display with touchscreen (http://www.eleduino.com/7-0-inch-Hdmi-touch-with-USB-touch-Display-Support-Raspberry-pi-Bnana-PI-Banana-Pro-Beaglebone-bone-p10442.html)"
#define DRIVER_LICENSE "GPL"


#define ELEDUINO_USB_VENDOR_ID				0x0eff
#define ELEDUINO_USB_PRODUCT_ID 			0x0005
#define USB_ELEDUINO_TS_MINOR_BASE 			192


struct eleduino_ts_dev {
	char name[128];
	char phys[64];
	struct usb_device *usb_dev;
	struct input_dev *input_dev;
	struct urb *dev_interrupt;

	signed char *data;
	dma_addr_t dma_data;
	int x, y;
};


static struct usb_device_id eleduino_ts_id_table[] = {
	{ USB_DEVICE(ELEDUINO_USB_VENDOR_ID, ELEDUINO_USB_PRODUCT_ID) },
	{ }
};

#endif