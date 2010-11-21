/*
*
 Author Johannes Petrick - 2010

This application is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License 3 as
published by the Free Software Foundation, either version 2.1 of the
License, or (at your option) any later version.

This application is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __TEMPER_H__
#define __TEMPER_H__

//The LibUSB (ver 0.1.x)
#include <usb.h>

//List of supported/known Vendor/Product IDs
#include "vendors.h"

//some limits from the sonsor 
#include "sensor.h"

// Connection Timeout for a USB Request
#define CONNECTION_TIMEOUT 60

// Know the functions
/**
* Looks for a device 
*/
usb_dev_handle *find_device();

int command (usb_dev_handle *dev, int size, char a, char b, char c, char d, char e, char f, char g);
char* read_device (usb_dev_handle *dev, char *data);
void claim_interfaces(usb_dev_handle *dev);
void claim_interface(usb_dev_handle *dev, int interface);
void free_interface(usb_dev_handle *dev, int interface);
int read_temperature(usb_dev_handle *udev);
char* get_device_info(usb_dev_handle *udev);
void init ();
int get_celsius();
int get_fahrenheit();
void list_supported_devices();
void cleanup(usb_dev_handle *udev);

/**/
struct usb_config {
  unsigned int vendor;
  unsigned int product;
//  char *bus;
//  char *device;
};

char *c_bus;
char *device;

struct usb_config *u_config;

int debug;

#endif
