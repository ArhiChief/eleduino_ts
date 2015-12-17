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
#include <linux/hid.h>
#include <linux/time.h>

#include <linux/list.h>

#include "eleduino_ts.h"
#include "misc.h"


#define DRIVER_VERSION "v0.1.0"
#define DRIVER_DESC    "Eleduino tft 7inch display. USB touchscreen driver."
#define DRIVER_LICENSE "GPL"
#define DRIVER_AUTHOR  "arhichief"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

#define USB_VENDOR_ID   0x0eef
#define USB_DEVICE_ID   0x0005

static eleduino_ts_event_t* eleduino_ts_events = NULL;

static void usb_eleduino_ts_irq(struct urb *urb){
  usb_eleduino_ts_t *eleduino_ts = urb->context;
  eleduino_ts_event_t* event;
  u8* data = eleduino_ts->data;
  int status;

  PRINT_COORDS(data);

  event = kzalloc(sizeof(eleduino_ts_event_t), GFP_ATOMIC);
  if (event){
    event->touched = data[0x01];
    do_gettimeofday(&event->time);
    EXTRACT_COORDS(event->x1, event->y1, data, 0x02);
#ifdef ELEDUINO_TS_USE_MULTITOUCH
    event->points = data[0x07];
    EXTRACT_COORDS(event->x2, event->y2, data, 0x08);
    EXTRACT_COORDS(event->x3, event->y3, data, 0x0C);
    EXTRACT_COORDS(event->x4, event->y4, data, 0x10);
    EXTRACT_COORDS(event->x5, event->y5, data, 0x15);
#endif

    list_add(&event->list, &eleduino_ts_events->list);
  }

  status = usb_submit_urb(urb, GFP_ATOMIC);
  if (status && &eleduino_ts->usb_dev->dev)
    KMSG_ERROR("can't resubmit intr, %s-%s/input0, status %d\n",
      eleduino_ts->usb_dev->bus->bus_name,
      eleduino_ts->usb_dev->devpath, status);
}

static int usb_eleduino_ts_open(struct input_dev *dev){
  usb_eleduino_ts_t *eleduino_ts = input_get_drvdata(dev);

  eleduino_ts->irq->dev = eleduino_ts->usb_dev;
  if(usb_submit_urb(eleduino_ts->irq, GFP_KERNEL)){
    KMSG_ERROR("can't submit URB");
    return -EIO;
  }

  return 0;
}

static void usb_eleduino_ts_close(struct input_dev *dev){
  usb_eleduino_ts_t *eleduino_ts = input_get_drvdata(dev);

  usb_kill_urb(eleduino_ts->irq);
}

static void inline usb_eleduino_ts_configure_input_dev(struct input_dev *input_dev) {
#ifndef ELEDUINO_TS_USE_MULTITOUCH
  /* Use singletouch configuration. Configure device for mouse emulation */
  input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
  input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
  input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
  input_dev->keybit[BIT_WORD(BTN_MOUSE)] |= BIT_MASK(BTN_SIDE) | BIT_MASK(BTN_EXTRA);
  input_dev->relbit[0] |= BIT_MASK(REL_WHEEL);
#else
  /* Use multitouch configuration based on Linux MT protocol */
#endif
}

static int usb_eleduino_ts_probe(struct usb_interface *intf, const struct usb_device_id *id){
  struct usb_device *usb_dev = interface_to_usbdev(intf);
  struct usb_host_interface *interface;
  struct usb_endpoint_descriptor *endpoint;
  usb_eleduino_ts_t *eleduino_ts;
  struct input_dev *input_dev;
  int pipe, maxp;
  int error = -ENOMEM;

  interface = intf->cur_altsetting;

  if (interface->desc.bNumEndpoints != 2) {
    KMSG_ERROR("no device found");
    return -ENODEV;
  }

  endpoint = &interface->endpoint[0].desc;
  if (!usb_endpoint_is_int_in(endpoint))
    return -ENODEV;

  pipe = usb_rcvintpipe(usb_dev, endpoint->bEndpointAddress);
  maxp = usb_maxpacket(usb_dev, pipe, usb_pipeout(pipe));

  eleduino_ts = kzalloc(sizeof(usb_eleduino_ts_t), GFP_KERNEL);
  input_dev = input_allocate_device();

  if (!eleduino_ts || !input_dev){
    KMSG_ERROR("can't allocate device");
    goto fail1;
  }

  eleduino_ts->data = usb_alloc_coherent(usb_dev, URB_PACKET_SIZE, GFP_KERNEL, &eleduino_ts->data_dma);
  if (!eleduino_ts->data)
    goto fail1;

  eleduino_ts->irq = usb_alloc_urb(0, GFP_KERNEL);
  if (!eleduino_ts->irq) 
    goto fail2;

  eleduino_ts->usb_dev = usb_dev;
  eleduino_ts->input_dev = input_dev;

  if (usb_dev->manufacturer)
    strlcpy(eleduino_ts->name, usb_dev->manufacturer, sizeof(eleduino_ts->name));

  if (usb_dev->product) {
    if (usb_dev->manufacturer)
      strlcat(eleduino_ts->name, " ", sizeof(eleduino_ts->name));
    strlcat(eleduino_ts->name, usb_dev->product, sizeof(eleduino_ts->name));
  }

  usb_make_path(usb_dev, eleduino_ts->phys, sizeof(eleduino_ts->phys));
  strlcat(eleduino_ts->phys, "/input0", sizeof(eleduino_ts->phys));

  input_dev->name = eleduino_ts->name;
  input_dev->phys = eleduino_ts->phys;
  usb_to_input_id(usb_dev, &input_dev->id);
  input_dev->dev.parent = &intf->dev;

  usb_eleduino_ts_configure_input_dev(input_dev);

  input_set_drvdata(input_dev, eleduino_ts);

  input_dev->open = usb_eleduino_ts_open;
  input_dev->close = usb_eleduino_ts_close;

  usb_fill_int_urb(eleduino_ts->irq, usb_dev, pipe, eleduino_ts->data,
    (maxp > 8 ? 8 : maxp), usb_eleduino_ts_irq, eleduino_ts, endpoint->bInterval);

  eleduino_ts->irq->transfer_dma = eleduino_ts->data_dma;
  eleduino_ts->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

  error = input_register_device(eleduino_ts->input_dev);
  if (error)
    goto fail3;

  usb_set_intfdata(intf, eleduino_ts);
  return 0;

fail3:
  usb_free_urb(eleduino_ts->irq);
fail2:
  usb_free_coherent(usb_dev, URB_PACKET_SIZE, eleduino_ts->data, eleduino_ts->data_dma);
fail1:
  input_free_device(input_dev);
  kfree(eleduino_ts);
  return error;
}

static void usb_eleduino_ts_disconnect(struct usb_interface *intf){
  usb_eleduino_ts_t *eleduino_ts = usb_get_intfdata(intf);
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

  if (result == 0)
    KMSG_INFO(DRIVER_VERSION "." DRIVER_DESC "\n");

  eleduino_ts_events = kzalloc(sizeof(eleduino_ts_event_t), GFP_KERNEL);

  INIT_LIST_HEAD(&eleduino_ts_events->list);

  if (!eleduino_ts_events)
    result = -ENOMEM;

  return result;
}

static void __exit usb_eleduino_ts_exit(void){
  usb_deregister(&usb_eleduino_ts_driver);
}

module_init(usb_eleduino_ts_init);
module_exit(usb_eleduino_ts_exit);