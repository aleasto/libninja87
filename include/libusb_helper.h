#include <stdio.h>
#include <libusb-1.0/libusb.h>

libusb_device_handle* usb_open(char* vid_pid, int intf) {
    libusb_device_handle* handle = NULL;
    int err;

    err = libusb_init(NULL);
    if (err < 0) {
        printf("Failed to initialize libusb");
        return NULL;
    }

    unsigned int vid, pid;
    sscanf(vid_pid, "%x:%x", &vid, &pid);
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if( handle == NULL) {
        printf("Cannot open device %04x:%04x. Do you have the rights to open?\n", vid, pid);
        return NULL;
    }

    // Print device info
    //print_device(libusb_get_device(handle), 1 /* verbose */);

    if (libusb_kernel_driver_active(handle, intf) == 1) {
        libusb_detach_kernel_driver(handle, intf);
    }

    err = libusb_claim_interface(handle, intf);
    if (err < 0) {
        printf("Error claiming interface %d: %s\n", intf, strerror(-err));
    }

    return handle;
}
