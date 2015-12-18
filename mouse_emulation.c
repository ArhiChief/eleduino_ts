/*
 *  Copyright (c) 2015 ArhiChief
 *
 *  Eleduino 7 inch tft display USB touchscreen driver. Gesture processor: mouse emulation implementation.

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

#include <linux/types.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

#include "gesture_processor.h"
#include "mouse_emulation.h"
#include "eleduino_ts.h"
#include "misc.h"

/*
 *  Determines whenever point [x;y] is in rectangle {[x2;y2], [x3;y3]}
 */
#define POINT_IN_RECT(x, y, x1, x2, x3, x4) (x > x1 && y > y1 && x < x2 && y < y2)

#define POINT_IN_VWEEL_AREA(x, y) \
 POINT_IN_RECT(x, y, TOUCHSCREEN_MIN_X, TOUCHSCREEN_MIN_Y, TOUCHSCREEN_MIN_X + VERT_WHEEL_AREA, TOUCHSCREEN_MAX_Y) || \
 POINT_IN_RECT(x, y, TOUCHSCREEN_MAX_X - VERT_WHEEL_AREA, TOUCHSCREEN_MIN_Y, TOUCHSCREEN_MAX_X, TOUCHSCREEN_MAX_Y)

#define POINT_IN_HWEEL_AREA(x, y) \
 POINT_IN_RECT(x, y, TOUCHSCREEN_MIN_X, TOUCHSCREEN_MIN_Y, TOUCHSCREEN_MAX_X, TOUCHSCREEN_MIN_Y + HORIZ_WHEEL_AREA) || \
 POINT_IN_RECT(x, y, TOUCHSCREEN_MIN_X, TOUCHSCREEN_MAX_Y - HORIZ_WHEEL_AREA, TOUCHSCREEN_MAX_X, TOUCHSCREEN_MAX_Y)

enum fsm_states {
	initial = 0,
	final
};

enum fsm_signal {
	sig_1,
	sig_2
};

typedef void(*emit_event_t)(struct input_dev *input_dev, int p1, int p2);

typedef struct 
{
	emit_event_t emit_event;
	enum fsm_states odl_state;
	enum fsm_states new_state;
} transition_t;


/* Event emiters */

 // Generates Mouse Horisontal Wheel Scrolling event (REL_HWHEEL)
static void hweel_event(struct input_dev* input_dev, int p1, int p2){
	input_report_rel(input_dev, REL_HWHEEL, p1);
}

// Generates Mouse Vertical Wheel Scrolling event (REL_WHEEL)
static void vweel_event(struct input_dev* input_dev, int p1, int p2){
	input_report_rel(input_dev, REL_WHEEL, p1);
}

// Generates Rigth Mouse Button Click event (BTN_RIGHT)
static void rmb_event(struct input_dev* input_dev, int p1, int p2){
	input_report_key(input_dev, BTN_RIGHT, p1);
}

// Generates Left Mouse Button Click event (BTN_LEFT)
static void lmb_event(struct input_dev* input_dev, int p1, int p2){
	input_report_key(input_dev, BTN_LEFT, p1);
}

// Generates Touch event (BTN_TOUCH)
static void btn_event(struct input_dev* input_dev, int p1, int p2){
	input_report_key(input_dev, BTN_TOUCH, p1);
}

// Generates Absolute Position event (ABS_X and ABS_Y)
static void abs_pos_event(struct input_dev* input_dev, int p1, int p2){
	input_report_abs(input_dev, ABS_X, p1);
	input_report_abs(input_dev, ABS_Y, p2);
}

int process_gesture(const eleduino_ts_event_t *event, struct input_dev *input_dev) {
 	return 0;
}