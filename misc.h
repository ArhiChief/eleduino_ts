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

#ifndef MISC_H
#define MISC_H

#define COORD(data, pos) (u16)(data[pos] && 0xFF) | ((u16)data[pos + 1])
#define COORDS(data, pos) COORD(data, pos), COORD(data, pos + 2)

#define EXTRACT_COORDS(x, y, data, from) \
 x = COORD(data, from); \
 y = COORD(data, from + 2)


/* Put message into kernel journal */
#define KMSG( alert_lvl, fmt, args... ) printk(alert_lvl KBUILD_MODNAME ": " fmt, ##args);

#define KMSG_INFO(fmt, args...) KMSG(KERN_INFO, fmt, ##args)
#define KMSG_WARN(fmt, args...) KMSG(KERN_WARNING, fmt, ##args)
#define KMSG_ALERT(fmt, args...) KMSG(KERN_ALERT, fmt, ##args)
#define KMSG_ERROR(fmt, args...) KMSG(KERN_ERR, fmt, ##args)

#ifndef PDEBUG
 #define KMSG_DEBUG(fmt, args...) KMSG(KERN_DEBUG, "%s::%i " fmt, __FUNCTION__, __LINE__,  ##args)

 #ifdef ELEDUINO_TS_USE_MULTITOUCH
 	/* Print touch information */
 	#define PRINT_COORDS(data) printk("touched=%d; points=%d; p1[%d;%d]; p2[%d;" \
        "%d]; p3[%d;%d]; p4[%d;%d]; p5[%d;%d];\n", data[0x01], data[0x07], \
        COORDS(data, 0x02), COORDS(data, 0x08), COORDS(data, 0x0C), \
        COORDS(data, 0x10), COORDS(data, 0x15))
 #else
 	#define PRINT_COORDS(data) printk("touched=%d; p[%d;%d]\n", data[0x01], COORDS(data, 0x02))
 #endif
 	
#else
 #define KMSG_DEBUG(fmt, args...)
 #define PRINT_COORDS(data)
#endif /* PDEBUG */








#endif	/* MISC_H */