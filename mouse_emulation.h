/*
 *  Copyright (c) 2015 ArhiChief
 *
 *  Eleduino 7 inch tft display USB touchscreen driver. Gesture processor: mouse emulation configuration.

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

 #ifndef MOUSE_EMULATION_H
 #define MOUSE_EMULATION_H

/* Gestures configuration 
 *
 *	LMB - left mouse button;
 *  RMB - right mouse button;
 *	MMB - midle mouse button (mouse wheel).
 *
 *  All delays are defined in milliseconds (ms).
 */

/* Delay betwean touch events what will recognized as LMB click */
#define LMB_DOUBLE_CLICK_DELAY				800
/* Delay betwean touch events what will recognized as RMB click */
#define RMB_CLICK_DELAY						1500
/* 
 * This values defines horizontal/vertical area width/heights where same gestures will be
 * interpreted as mouse wheel scrolling. (Same as touchpad scroll area)
 */
#define HORIZ_WHEEL_AREA 					50
#define VERT_WHEEL_AREA 					50
/* =============================== */

 #endif 	/* MOUSE_EMULATION_H */