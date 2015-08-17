#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/usb/input.h>

#include "eleduino_ts.h"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VER);


static struct eleduino_ts_dev eleduino_ts_dev; 

static void eleduino_ts_disconnect(struct usb_interface *intf){
}

static int eleduino_ts_probe(struct usb_interface *intf, const struct usb_device_id *id){
	return 0;
}



static int eleduino_ts_init(void){
	printk(KERN_ALERT "Hello, world!\n");
	return 0;
}

static void eleduino_ts_exit(void){
	memset(&eleduino_ts_dev, 0, sizeof(struct eleduino_ts_dev));
	printk (KERN_ALERT "goodbye\n");
}


static struct usb_driver eleduino_ts_driver = {
    .name          = "eleduino_ts",
    .probe         = eleduino_ts_probe,
    .disconnect    = eleduino_ts_disconnect,
    .id_table      = eleduino_ts_id_table,
};



module_init(eleduino_ts_init);
module_exit(eleduino_ts_exit);