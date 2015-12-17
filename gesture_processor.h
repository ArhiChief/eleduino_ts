/*
 *  Copyright (c) 2015 ArhiChief
 *
 *  Eleduino 7 inch tft display USB touchscreen driver. Gesture processor header file

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

#ifndef GESTURE_PROCESSOR_H
#define GESTURE_PROCESSOR_H

#include "eleduino_ts.h"

/* process points queue of gesture */
int process_gesture(eleduino_ts_event_t *, const struct input_dev *);

#endif	/* GESTURE_PROCESSOR_H */