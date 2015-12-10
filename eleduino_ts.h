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

#ifndef ELEDUINO_TS_H
#define ELEDUINO_TS_H

#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/usb/input.h>
#include <linux/usb.h>

/* ===== Driver onfiguration ===== */

/* Display resolution parametrs */
#define TOUCHSCREEN_MIN_X 0 			
#define TOUCHSCREEN_MIN_Y 0
#define TOUCHSCREEN_MAX_X 800
#define TOUCHSCREEN_MAX_Y 480

/* Touchscreen pressure parametrs */
#define TOUCHSCREEN_MIN_PRESSURE 50		
#define TOUCHSCREEN_MAX_PRESSURE 200

/* Gestures configuration 
 *
 *	LMB - left mouse button;
 *  RMB - right mouse button;
 *	MMB - midle mouse button (mouse wheel).
 *
 *  All delays are defined in milliseconds (ms).
 */

/* Delay betwean touch events what will recognized as LMB click */
#define LMB_DOUBLE_CLICK_DELAY				1000
/* Delay betwean touch events what will recognized as RMB click */
#define RMB_CLICK_DELAY						1500
/* =============================== */


/* 
 * Information about touch point stores in firts 3 bytes of message in 
 * singletouch configuration 
 */
#ifndef ELEDUINO_TS_USE_MULTITOUCH
	#define URB_PACKET_SIZE 0x03	
	#define URB_PACKET_OFFSET 0x00
#else
	#define URB_PACKET_SIZE 0x03	
	#define URB_PACKET_OFFSET 0x00
#endif


/* Structure of the device */
struct usb_eleduino_ts {
  char name[128];
  char phys[64];
  struct usb_device *usb_dev;
  struct input_dev *input_dev;
  struct urb *irq;
  
  u8 *data;
  dma_addr_t data_dma;
};

/* Structure of the touchevent */
struct eleduino_ts_event
{
	u16 x1;
	u16 y1;
#ifdef ELEDUINO_TS_USE_MULTITOUCH
	u16 x2;
	u16 y2;
	u16 x3;
	u16 y3;
	u16 x4;
	u16 y4;
	u16 x5;
	u16 y5;
	u8 points;
#endif
	struct timeval time;	/* Time when event rises */
};

#endif	/* ELEDUINO_TS_H */