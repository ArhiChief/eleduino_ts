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
 * e-mail - mail your message to <varhichief@gmail.com>.
 */


#ifndef _ELEDUINO_TS_H_
#define _ELEDUINO_TS_H_

#include <linux/kernel.h>

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

#define USB_VENDOR_ID   0x0eef
#define USB_DEVICE_ID   0x0005



struct eleduino_ts_event {
	u16 x1;
	u16	y1;
	u16 x2;
	u16	y2;
	u16 x3;
	u16	y3;
	u16 x4;
	u16	y4;
	u16 x5;
	u16	y5;
	u16 preasure;
	u8 touch_point;
};

struct usb_eleduino_ts {
  char name[128];
  char phys[64];
  struct usb_device *usb_dev;
  struct input_dev *input_dev;
  struct urb *irq;
  
  struct eleduino_ts_event *data;
  dma_addr_t data_dma;
};


#endif /* _ELEDUINO_TS_H_ */