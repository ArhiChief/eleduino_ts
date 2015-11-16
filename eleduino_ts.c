/*
 *  Copyright (c) 2015 ArhiChief
 *
 *  Eleduino 7 inch tft display USB touchscreen driver

 *
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
 * e-mail - mail your message to <arhichief@gmail.com>.
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

/* Put message into kernel journal */
#define KMSG( alert_lvl, fmt, args... ) printk(alert_lvl KBUILD_MODNAME ": " fmt, ##args);

#define KMSG_INFO(fmt, args...) KMSG(KERN_INFO, fmt, ##args)
#define KMSG_WARN(fmt, args...) KMSG(KERN_WARNING, fmt, ##args)
#define KMSG_ALERT(fmt, args...) KMSG(KERN_ALERT, fmt, ##args)
#define KMSG_ERROR(fmt, args...) KMSG(KERN_ERR, fmt, ##args)

#ifndef PDEBUG
 #define KMSG_DEBUG(fmt, args...) KMSG(KERN_DEBUG, "%s::%i " fmt, __FUNCTION__, __LINE__,  ##args)
#else
 #define KMSG_DEBUG(fmt, args...)
#endif /* PDEBUG */



#define URB_PACKET_SIZE 0x20

// #ifndef USE_MULTITOUCH
//  #define USE_MULTITOUCH 1
//  #define URB_PACKET_SIZE 0x20
// #else
//  #define USE_MULTITOUCH 0
//  #define URB_PACKET_SIZE 0x6
// #endif

#define TOUCHSCREEN_MIN_X 0
#define TOUCHSCREEN_MIN_Y 0
#define TOUCHSCREEN_MAX_X 800
#define TOUCHSCREEN_MAX_Y 480
#define TOUCHSCREEN_MIN_PRESSURE 50
#define TOUCHSCREEN_MAX_PRESSURE 200

/*
 *  The packet, that device sends to host has next structure. For more details see FT5206.pdf
 *
 *    ---------------------------------------------------------------------------------------------------
 *    | Address | Register Name   | Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 | Host Access |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 00h | DEVICE_MODE     |      | Device_mode[2:0]   |                           | R / W       |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 01h | GEST_ID         | Gesture ID[7:0]                                       | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 02h | TD_STATUS       |                           | Number of touchpoints[3:0]| R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 03h | TOUCH1_XH       |1 Event Flag |             | 1st Touch Position[11:8]  | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 04h | TOUCH1_XL       | 1st Touch X Position [7:0]                            | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 05h | TOUCH1_YH       | 1st Touch ID[0:3]         | 1st Touch Position[11:8]  | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 06h | TOUCH1_YL       | 1st Touch Y Position [7:0]                            | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 07h |                 |                                                       | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    | Op, 08h |                 |                                                       | R           |
 *    ---------------------------------------------------------------------------------------------------
 *    |           4 other touchpoint has same structure and it's placed from 09h to 20h.               |
 *    ---------------------------------------------------------------------------------------------------
 *    
 */

/* The structure of the device */
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

  struct usb_eleduino_ts *eleduino_ts = urb->context;
  struct input_dev *dev = eleduino_ts->input_dev;
  u8 *data = eleduino_ts->data;
  int x, y, touchpoints, status;

  touchpoints = data[2];
  
  KMSG_DEBUG("touchpoints %i:", touchpoints);

  KMSG_DEBUG("print data: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8],
    data[9], data[10], data[11], data[12], data[13], data[14], data[15], data[16], data[17],
    data[18], data[19], data[20], data[21], data[22], data[23], data[24], data[25], data[26],
    data[27], data[28], data[29], data[30], data[31]);


  x = ((unsigned int)(data[3] & 0xFF) << 8) | ((unsigned int)data[4]);
  y = ((unsigned int)(data[5] & 0xFF) << 8) | ((unsigned int)data[6]);

  if (x != 0){
    KMSG_DEBUG("Coordinates: x:%i y:%i", x, y);
    input_report_abs(dev, ABS_X, x);
    input_report_abs(dev, ABS_Y, y);
    input_report_abs(dev, ABS_PRESSURE, 200);
  }

  if (touchpoints > 0){
    KMSG_DEBUG("Touched");
    input_report_abs(dev, BTN_TOUCH, 1);
  }
  else{
    KMSG_DEBUG("UnTouched");
    input_report_abs(dev, BTN_TOUCH, 0);
  }

  input_sync(dev);

  memset(data, 0, URB_PACKET_SIZE);

  status = usb_submit_urb(urb, GFP_ATOMIC);

  if (status && &eleduino_ts->usb_dev->dev)
    KMSG_ERROR("can't resubmit intr, %s-%s/input0, status %d\n",
      eleduino_ts->usb_dev->bus->bus_name,
      eleduino_ts->usb_dev->devpath, status);
}

static int usb_eleduino_ts_open(struct input_dev *dev){
  struct usb_eleduino_ts *eleduino_ts = input_get_drvdata(dev);

  eleduino_ts->irq->dev = eleduino_ts->usb_dev;
  if(usb_submit_urb(eleduino_ts->irq, GFP_KERNEL)){
    KMSG_ERROR("can't submit URB");
    return -EIO;
  }

  return 0;
}

static void usb_eleduino_ts_close(struct input_dev *dev){
  struct usb_eleduino_ts *eleduino_ts = input_get_drvdata(dev);

  usb_kill_urb(eleduino_ts->irq);
}

static void inline usb_eleduino_ts_configure_input_dev(struct input_dev *input_dev) {
  set_bit(ABS_PRESSURE, input_dev->absbit);
  set_bit(BTN_TOUCH, input_dev->keybit);

  input_set_abs_params(input_dev, ABS_X, TOUCHSCREEN_MIN_X, TOUCHSCREEN_MAX_X, 0, 0);
  input_set_abs_params(input_dev, ABS_Y, TOUCHSCREEN_MIN_Y, TOUCHSCREEN_MAX_Y, 0, 0);
  input_set_abs_params(input_dev, ABS_PRESSURE, 0, 200, 0, 0);

  set_bit(EV_ABS, input_dev->evbit);
  set_bit(EV_KEY, input_dev->evbit);
}

static int usb_eleduino_ts_probe(struct usb_interface *intf, const struct usb_device_id *id){
  struct usb_device *dev = interface_to_usbdev(intf);
  struct usb_host_interface *interface = intf->cur_altsetting;
  struct usb_endpoint_descriptor *endpoint;
  struct usb_eleduino_ts *eleduino_ts;
  struct input_dev *input_dev;
  int pipe, maxp;
  int err = -ENOMEM;

  if (interface->desc.bNumEndpoints != 2){
    KMSG_ERROR("no device found");
    return -ENODEV;
  }

  endpoint = &interface->endpoint[0].desc;

  if (!usb_endpoint_is_int_in(endpoint)){
    KMSG_ERROR("endpoint is not int");
    return -ENODEV;
  }

  pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
  maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));

  eleduino_ts = kzalloc(sizeof(struct usb_eleduino_ts), GFP_KERNEL);
  input_dev = input_allocate_device();

  if (!eleduino_ts || !input_dev){
    KMSG_ERROR("cannot allocate device");
    err =- ENOMEM;
    goto fail1;
  }

  eleduino_ts->data = usb_alloc_coherent(dev, URB_PACKET_SIZE, GFP_KERNEL, &eleduino_ts->data_dma);
  if (!eleduino_ts->data){
    KMSG_ERROR("cannot allocate device data");
    err = -ENOMEM;
    goto fail1;
  }
  memset(eleduino_ts->data, 0, URB_PACKET_SIZE);

  eleduino_ts->irq = usb_alloc_urb(0, GFP_KERNEL);
  if (!eleduino_ts->irq) {
    KMSG_ERROR("cannot allocate device irq");
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

  input_set_drvdata(input_dev, eleduino_ts);

  input_dev->open = usb_eleduino_ts_open;
  input_dev->close = usb_eleduino_ts_close;


  usb_eleduino_ts_configure_input_dev(input_dev);


  if (!input_dev->absinfo)
    input_dev->absinfo = kcalloc(ABS_CNT, sizeof(struct input_absinfo), GFP_KERNEL);
  if (!input_dev->absinfo)
    KMSG_WARN("%s(): kcalloc() failed?\n", __FUNCTION__);

  

  usb_fill_int_urb(eleduino_ts->irq, dev, pipe, eleduino_ts->data, maxp > 8 ? 8 : maxp, usb_eleduino_ts_irq, eleduino_ts, endpoint->bInterval);

  eleduino_ts->irq->transfer_dma = eleduino_ts->data_dma;
  eleduino_ts->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

  err = input_register_device(eleduino_ts->input_dev);

  if (err) goto fail2;

  usb_set_intfdata(intf, eleduino_ts);

  return 0;

fail2: usb_free_coherent(dev, URB_PACKET_SIZE, eleduino_ts->data, eleduino_ts->data_dma);
fail1: kfree(input_dev->absinfo);
  input_free_device(input_dev);
  kfree(eleduino_ts);
  KMSG_ERROR("no device found");
  return err;
}

static void usb_eleduino_ts_disconnect(struct usb_interface *intf){
  struct usb_eleduino_ts *eleduino_ts = usb_get_intfdata(intf);
  struct input_dev *dev = eleduino_ts->input_dev;

  usb_set_intfdata(intf, NULL);
  if (eleduino_ts){
    kfree(dev->absinfo);
    usb_kill_urb(eleduino_ts->irq);
    input_unregister_device(eleduino_ts->input_dev);
    usb_free_urb(eleduino_ts->irq);
    usb_free_coherent(interface_to_usbdev(intf), URB_PACKET_SIZE, eleduino_ts->data, eleduino_ts->data_dma);
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

  KMSG_DEBUG("inside init");

  if (result == 0) {
    KMSG_INFO(DRIVER_VERSION "." DRIVER_DESC "\n");
  }
  return result;
}

static void __exit usb_eleduino_ts_exit(void){
  usb_deregister(&usb_eleduino_ts_driver);
}

module_init(usb_eleduino_ts_init);
module_exit(usb_eleduino_ts_exit);