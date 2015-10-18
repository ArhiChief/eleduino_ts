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


/*
 *  The packet, that device sends to host has next structure
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
 *    |           4 other touch point has same structure and it's placed from 09h to 20h.               |
 *    ---------------------------------------------------------------------------------------------------
 *    
 *     Device Mode:
 *       * 000b - Normal operating mode;  
 *       * 001b - System Information Mode (Reserved);
 *       * 100b - Test Mode - read raw data (Reserved).
 *
 *     Gesture Id. This register describes the gesture of valid touch.
 *       * 0x10 - Move UP;
 *       * 0x14 - Move Left;
 *       * 0x18 - Move Down;
 *       * 0x1C - Move Right;
 *       * 0x48 - Zoom In;
 *       * 0x49 - Zoom Out;
 *       * 0x00 - No Gesture;
 *       
 *    Number of touchpoints. 1-5 is valid
 *
 *    TOUCHn_XH. This register describes MSB of X coordinate of the nth touch point and corresponding event flag
 *       * Event Flag
 *       	- 00b - Put Down
 *       	- 01b - Put Up
 *       	- 10b - Contact
 *       	- 11b - Reserved
 *       * Touch Position[11:8] - MSB of Touch X Position in pixels
 *       
 *    TOUCHn_XH. This Register describes LSB If the X coordinate of nth touch point
 *       * Touch X Position[7:0] LSB of the Touch X Position in pixels
 *
 *    TOUCHn_YH. This register describes the MSB of the Y coordinate of the nth touch point and corresponding touch ID
 *       * Touch Id - TopuchID of Touch Point
 *       * Touch Y Position - MSB of Y Position in pixels 
 *
 *    TOUCHn_YH. This Register describes LSB If the Y coordinate of nth touch point
 *       * Touch Y Position[7:0] LSB of the Touch Y Position in pixels
 *
 */

struct eleduino_ts_event {
	
} __atribute__ ((packed));



/* The structure of the device */
struct usb_eleduino_ts {
  char name[128];
  char phys[64];
  struct usb_device *usb_dev;
  struct input_dev *input_dev;
  struct urb *irq;
  
  struct eleduino_ts_event  *data;
  dma_addr_t data_dma;
};


#endif /* _ELEDUINO_TS_H_ */