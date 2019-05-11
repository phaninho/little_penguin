#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/usb.h> 	  /* Needed for usb */
#include <uapi/linux/hid.h> /* Needed for USB_INTERFACE macros */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("This module is for HID detection!");

static struct usb_device_id kb_table [] = {
        { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,    \
		USB_INTERFACE_SUBCLASS_BOOT,     \
                USB_INTERFACE_PROTOCOL_KEYBOARD) },
        { }                      /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, kb_table);

static int kb_probe(struct usb_interface *interface,
    const struct usb_device_id *id)
{
    	printk(KERN_INFO "pass into kb_probe !\n");
        return 0;
}

static void kb_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "keyboard disconnect\n");
}

static struct usb_driver kb_driver = {
	.name        = "kb_module",
        .probe       = kb_probe,
        .disconnect  = kb_disconnect,
        .id_table    = kb_table,
};

static int __init kb_init(void)
{
        int result;

        result = usb_register(&kb_driver);
        if (result < 0) {
	    	printk(KERN_INFO "usb_register failed!\n");
                return -1;
        }
	printk(KERN_INFO "kb_init OK!!\n");
        return 0;
}

static void __exit kb_exit(void)
{
	printk(KERN_INFO "kb_exit OK!!\n");
        usb_deregister(&kb_driver);
}

module_init(kb_init);
module_exit(kb_exit);
