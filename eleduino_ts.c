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
#include <linux/usb.h>

#define DRIVER_VERSION "v0.0.1"
#define DRIVER_DESC    "Eleduino tft 7inch display. USB touchscreen driver."
#define DRIVER_LICENSE "GPL"
#define DRIVER_AUTHOR  "arhichief"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

#define USB_VENDOR_ID   0x0eef
#define USB_DEVICE_ID   0x0005

#define FT5206_REGISTERS_NUM 7
#define SCREEN_MAX_X 800
#define SCREEN_MAX_Y 480
#define PRESS_MAX  200

// #define CONFIG_USE_MULTITOUCH


struct usb_eleduino_ts {
  char name[128];
  char phys[64];
  struct usb_device *usb_dev;
  struct input_dev *input_dev;
  struct urb *irq;

  u8 *data;
  dma_addr_t data_dma;
};


static void usb_eleduino_ts_irq(struct urb *urb){
  
  int status;

  struct usb_eleduino_ts *eleduino_ts = urb->context;
  u8 *data = eleduino_ts->data;
  struct input_dev *dev = eleduino_ts->input_dev;
  int x, y, pressure, touchpoints;

  printk(KERN_ALERT "usb_eleduino_ts_irq");


  touchpoints = data[2] & 0x07;
  printk(KERN_ALERT "touchpoints %d", touchpoints);

  x = (int)(data[3] & 0x0F) << 8 | (int)data[4];
  y = (int)(data[5] & 0x0F) << 8 | (int)data[6];
  pressure = 200;

  input_report_abs(dev, ABS_X, x);
  input_report_abs(dev, ABS_Y, y);
  input_report_abs(dev, ABS_PRESSURE, pressure);
  input_report_key(dev, BTN_TOUCH, 1);

  input_sync(dev);

  status = usb_submit_urb(urb, GFP_ATOMIC);
  if (status)
    dev_err(&eleduino_ts->usb_dev->dev,
      "can't resubmit intr, %s-%s/input0, status %d\n",
      eleduino_ts->usb_dev->bus->bus_name,
      eleduino_ts->usb_dev->devpath, status);

}


static int usb_eleduino_ts_open(struct input_dev *dev){
  struct usb_eleduino_ts *eleduino_ts = input_get_drvdata(dev);
  printk(KERN_ALERT "usb_eleduino_ts_open");

  eleduino_ts->irq->dev = eleduino_ts->usb_dev;
  if(usb_submit_urb(eleduino_ts->irq, GFP_KERNEL)){
    printk(KERN_ALERT "usb_eleduino_ts_open: -EIO");
    return -EIO;
  }

  return 0;
}

static void usb_eleduino_ts_close(struct input_dev *dev){
  struct usb_eleduino_ts *eleduino_ts = input_get_drvdata(dev);

  printk(KERN_ALERT "usb_eleduino_ts_open");

  usb_kill_urb(eleduino_ts->irq);
}

static int usb_eleduino_ts_probe(struct usb_interface *intf, const struct usb_device_id *id){
  struct usb_device *dev = interface_to_usbdev(intf);
  struct usb_host_interface *interface = intf->cur_altsetting;
  struct usb_endpoint_descriptor *endpoint;
  struct usb_eleduino_ts *eleduino_ts;
  struct input_dev *input_dev;
  int pipe, maxp;
  int err = -ENOMEM;

  printk(KERN_ALERT "usb_eleduino_ts_probe");  

  if (interface->desc.bNumEndpoints != 2){
    printk(KERN_ALERT "usb_eleduino_ts_probe: 1 no device found");
    return -ENODEV;
  }

  endpoint = &interface->endpoint[0].desc;

  if (!usb_endpoint_is_int_in(endpoint)){
    printk(KERN_ALERT "usb_eleduino_ts_probe: endpoint is not int");
    return -ENODEV;
  }

  pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
  maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));

  eleduino_ts = kzalloc(sizeof(struct usb_eleduino_ts), GFP_KERNEL);
  input_dev = input_allocate_device();

  if (!eleduino_ts || !input_dev){
    printk(KERN_ALERT "usb_eleduino_ts_probe: cannot allocate device");
    err =- ENOMEM;
    goto fail1;
  }

  eleduino_ts->data = usb_alloc_coherent(dev, FT5206_REGISTERS_NUM, GFP_KERNEL, &eleduino_ts->data_dma);
  if (!eleduino_ts->data){
    printk(KERN_ALERT "usb_eleduino_ts_probe: cannot allocate device data");
    err = -ENOMEM;
    goto fail1;
  }

  eleduino_ts->irq = usb_alloc_urb(0, GFP_KERNEL);
  if (!eleduino_ts->irq) {
    printk(KERN_ALERT "usb_eleduino_ts_probe: cannot allocate device irq");
    err = -ENOMEM;
    goto fail2;
  }

  eleduino_ts->usb_dev = dev;
  eleduino_ts->input_dev = input_dev;

  if (dev->manufacturer){
    strlcpy(eleduino_ts->name, dev->manufacturer, sizeof(eleduino_ts->name));
  }

  if (dev->product){
    if (dev->manufacturer)
      strlcat(eleduino_ts->name, " ", sizeof(eleduino_ts->name));
    strlcat(eleduino_ts->name, dev->product, sizeof(eleduino_ts->name));
  }

  usb_make_path(dev, eleduino_ts->phys, sizeof(eleduino_ts->phys));
  strlcat(eleduino_ts->phys, "/input0", sizeof(eleduino_ts->phys));

  input_dev->name = eleduino_ts->name;
  input_dev->phys = eleduino_ts->phys;
  usb_to_input_id(dev, &input_dev->id);
  input_dev->dev.parent = &intf->dev;

  printk(KERN_ALERT "usb_eleduino_ts_probe: device found: [%s][%s]", eleduino_ts->name, eleduino_ts->phys);

  input_set_drvdata(input_dev, eleduino_ts);

  input_dev->open = usb_eleduino_ts_open;
  input_dev->close = usb_eleduino_ts_close;

#ifdef CONFIG_USE_MULTITOUCH
  printk(KERN_ALERT "use multitouch configuration]");
#else
  printk(KERN_ALERT "use onetouch configuration]");
  set_bit(ABS_PRESSURE, input_dev->absbit);
  set_bit(BTN_TOUCH, input_dev->keybit);

  input_set_abs_params(input_dev, ABS_X, 0, SCREEN_MAX_X, 0, 0);
  input_set_abs_params(input_dev, ABS_Y, 0, SCREEN_MAX_Y, 0, 0);
  input_set_abs_params(input_dev, ABS_PRESSURE, 0, PRESS_MAX, 0, 0);
#endif

  set_bit(EV_ABS, input_dev->evbit);
  set_bit(EV_KEY, input_dev->evbit);

  // input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
  // input_dev->relbit[0] = 0;
  // input_dev->absbit[0] = BIT_MASK(ABS_X) | BIT_MASK(ABS_Y);

  // input_dev->keybit[BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);

  if (!input_dev->absinfo)
    input_dev->absinfo = kcalloc(ABS_CNT, sizeof(struct input_absinfo), GFP_KERNEL);
  WARN(!input_dev->absinfo, "%s(): kcalloc() failed?\n", __func__);

  input_dev->absinfo[ABS_X].fuzz = 6;
  input_dev->absinfo[ABS_Y].fuzz = 6;

  usb_fill_int_urb(eleduino_ts->irq, dev, pipe, eleduino_ts->data, maxp > 8 ? 8 : maxp, usb_eleduino_ts_irq, eleduino_ts, endpoint->bInterval);

  eleduino_ts->irq->transfer_dma = eleduino_ts->data_dma;
  eleduino_ts->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

  err = input_register_device(eleduino_ts->input_dev);

  if (err) goto fail2;

  usb_set_intfdata(intf, eleduino_ts);

  return 0;

fail2: usb_free_coherent(dev, FT5206_REGISTERS_NUM, eleduino_ts->data, eleduino_ts->data_dma);
fail1: kfree(input_dev->absinfo);
  input_free_device(input_dev);
  kfree(eleduino_ts);
  printk(KERN_ALERT "usb_eleduino_ts_probe: 2 no device found");
  return err;

}

static void usb_eleduino_ts_disconnect(struct usb_interface *intf){
  struct usb_eleduino_ts *eleduino_ts = usb_get_intfdata(intf);
  struct input_dev *dev = eleduino_ts->input_dev;

  printk(KERN_ALERT "usb_eleduino_ts_disconnect");

  usb_set_intfdata(intf, NULL);
  if (eleduino_ts){
    kfree(dev->absinfo);
    usb_kill_urb(eleduino_ts->irq);
    input_unregister_device(eleduino_ts->input_dev);
    usb_free_urb(eleduino_ts->irq);
    usb_free_coherent(interface_to_usbdev(intf), FT5206_REGISTERS_NUM, eleduino_ts->data, eleduino_ts->data_dma);
    kfree(eleduino_ts);
  }
}

static struct usb_device_id usb_eleduino_ts_id_table[] = {
  { USB_DEVICE(USB_VENDOR_ID, USB_DEVICE_ID), .driver_info = 0 },
  { }
};

MODULE_DEVICE_TABLE(usb, usb_eleduino_ts_id_table);

static struct usb_driver usb_eleduino_ts_driver = {
  .name       = "eleduino_ts",
  .probe      = usb_eleduino_ts_probe,
  .disconnect = usb_eleduino_ts_disconnect,
  .id_table   = usb_eleduino_ts_id_table
};


static int __init usb_eleduino_ts_init(void){
  int result = usb_register(&usb_eleduino_ts_driver);

  printk(KERN_ALERT "usb_eleduino_ts_init");

  if (result == 0)
    printk(KERN_INFO KBUILD_MODNAME ": " DRIVER_VERSION ":" DRIVER_DESC "\n");

  return result;
}

static void __exit usb_eleduino_ts_exit(void){
  usb_deregister(&usb_eleduino_ts_driver);

  printk(KERN_ALERT "usb_eleduino_ts_exit");
}

module_init(usb_eleduino_ts_init);
module_exit(usb_eleduino_ts_exit);