/*
 *  Copyright (c) 2015 ArhiChief
 *
 *  Eleduino 7 inch tft display USB touchscreen driver
 */

 /*
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  *
  * Should you need to contact me, the author, you can do so either by
  * e-mail - mail your message to <varhichief@gmail.com>.
  */


 #include <linux/kernel.h>
 #include <linux/slab.h>
 #include <linux/module.h>
 #include <linux/init.h>
 #include <linux/usb/input.h>
 #include <linux/hid.h>


/* 
 * Debug macro 
 */



/*
 * Driver information
 */

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "ArhiChief <arhichief@gmail.com>"
#define DRIVER_DESC "Eleduino 7 inch tft display USB touchscreen driver"
#define DRIVER_LICENSE "GPL"
#define DEVICE_VID 0x0eef
#define DEVICE_PID 0x0005

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);


/*
 * Device structure
 */
struct eleduino_ts
{
	char name[128];
	char phys[64];
	
	struct usb_device *usbdev;
	struct input_dev *inputdev;
	struct urb *irq;
	
	signed char *data;
  dma_addr_t data_dma;
};

static void eleduino_ts_irq(struct urb *irq){
  printk(KERN_ALERT, "eleduino_ts_irq");
}

static int eleduino_ts_open(struct input_dev *dev){

  struct eleduino_ts *eleduino_dev = input_get_drvdata(dev);

  printk(KERN_ALERT, "eleduino_ts_open");

  eleduino_dev->irq->dev = eleduino_dev->usbdev;
  if (usb_submit_urb(eleduino_dev->irq, GFP_KERNEL))
    return -EIO;

  return 0;
}

static void eleduino_ts_close(struct input_dev *dev){

  struct eleduino_ts *eleduino_dev = input_get_drvdata(dev);

  printk(KERN_ALERT, "eleduino_ts_close");
  
  usb_kill_urb(eleduino_dev->irq);
}

static int eleduino_ts_probe(struct usb_interface *intf, const struct usb_device_id *id){

  printk(KERN_ALERT, "eleduino_ts_probe");

	return 0;
}

static void eleduino_ts_disconnect(struct usb_interface *intf){
  struct eleduino_ts *eleduino_dev = usb_get_intfdata(intf);

  printk(KERN_ALERT, "eleduino_ts_disconnect");

  usb_set_intfdata(intf, NULL);
  if (eleduino_dev){
    usb_kill_urb(eleduino_dev->irq);
    input_unregister_device(eleduino_dev->inputdev);
    usb_free_urb(eleduino_dev->irq);
    usb_free_coherent(interface_to_usbdev(intf), 8, eleduino_dev->data, eleduino_dev->data_dma);
    kfree(eleduino_dev);
  }
}

static struct usb_device_id eleduino_ts_id_table [] = {
  { USB_DEVICE(DEVICE_VID, DEVICE_PID) },
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, eleduino_ts_id_table);

static struct usb_driver eleduino_ts_driver = {
	.name			= "eleduino_ts",
	.probe 			= eleduino_ts_probe,
	.disconnect 	= eleduino_ts_disconnect,
	.id_table 		= eleduino_ts_id_table
};

module_usb_driver(eleduino_ts_driver);