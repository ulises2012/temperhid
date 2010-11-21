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

#include "overall.h"
#include "temper.h"

/*
* lists the supported devices
*/
void list_supported_devices(){
  struct usb_bus *bus;
  struct usb_device *dev;
      printf("bus:dev name      (vendor:product)\n");
  for (bus = usb_busses; bus; bus= bus->next){
    for (dev = bus->devices; dev; dev = dev->next){
      if (dev->descriptor.idVendor == HidTEMPer_VENDOR && dev->descriptor.idProduct == HidTEMPer_PRODUCT){
        printf("%s:%s TEMPerHID (%04X:%04X)\n",bus->dirname, 
                                dev->filename,dev->descriptor.idVendor,dev->descriptor.idProduct);
      }
      if (dev->descriptor.idVendor == TEMPER_VENDOR && dev->descriptor.idProduct == TEMPER_PRODUCT){
        printf("%s:%s TEMPerV1 (%04X:%04X)\n",bus->dirname, 
                                dev->filename,dev->descriptor.idVendor,dev->descriptor.idProduct);
      }
    }
  }
}

/*
* finds the needed USB device
*/
usb_dev_handle *find_device(){
  struct usb_bus *bus;
  struct usb_device *dev;

  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      usb_dev_handle *udev;
      /* Skip the USB Bus we don't need */
      if (strcmp(bus->dirname, c_bus) && strcmp(dev->filename, device)){
        continue;
      }
      if (debug>2) printf("%s:%s %04X:%04X\n",bus->dirname,
                       dev->filename,dev->descriptor.idVendor,dev->descriptor.idProduct);
      /* If we found the Vendor and the Product => read the temperature */
      if (dev->descriptor.idVendor == u_config->vendor && dev->descriptor.idProduct == u_config->product){
        if (debug>2) printf("Found\n");
        udev = usb_open(dev);
        if (udev){
          if (debug>2) printf("Opened Device\n");
          claim_interfaces(udev);
          return udev;
        }
      }
    }
  }
  return NULL;
}

/*
* releses the USB interfaces und closes the USB communication
*/
void cleanup(usb_dev_handle *udev){
  usb_release_interface(udev, 1);
  usb_release_interface(udev, 0);
  usb_close(udev);
}

/*
* Claims the Interface 0 and 1
* and, if this is a Windows System, we will set the usb configuration to 1
*/
void claim_interfaces(usb_dev_handle *dev){
  #if defined(_WIN32) || defined(__WINDOWS__)
    usb_set_configuration(dev,1);
  #endif
  claim_interface(dev,0);
  claim_interface(dev,1);
}

/*
* Claim the Interface
*/
void claim_interface(usb_dev_handle *dev, int interface){
  int c;
  int retry = 3;
  while ( ( c=usb_claim_interface(dev,interface) != 0 )  &&  ( retry-- > 0 ) ){
    fprintf(stdout, "Warning: Problem: %s\n", usb_strerror());
    #ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
      usb_detach_kernel_driver_np(dev, interface);
      fprintf(stderr, "Warning: could not detach kernel HID driver: %s\n", usb_strerror());
    #endif
   }
}

/*
* Free the USB Device
*/
void free_interface(usb_dev_handle *dev, int interface){
  usb_release_interface(dev, interface);
}

/**
* Writes a command to the USB device and returns the result
* @return int restult set
*/
int command (usb_dev_handle *dev, int size, char a, char b, char c, char d, char e, char f, char g){
  int result;
  char tmp[size];
  bzero(tmp,size);
  
  tmp[0] = a;
  tmp[1] = b;
  tmp[2] = c;
  tmp[3] = d;
  tmp[4] = e;
  tmp[5] = f;
  tmp[6] = g;

  result = usb_control_msg(
    dev,
    0x21,
    0x9,
    0x200,
    0x1,
    tmp,
    size,
    CONNECTION_TIMEOUT
  );
  if (result != size){
    if (result < size){
      fprintf(stderr,"[command] The device returned less bytes than expected: %d | %d - %s\n",result, size,usb_strerror());
    } else {
      fprintf(stderr,"[command] The device returned more bytes than expected\n");
    }
    result = -1;
  }
  return result;
}

/*
* reads the device
*/
char* read_device (usb_dev_handle *dev, char *data){
  int chksum = 0;
  int i;
  char temp[32];
  bzero(temp,32);
  temp[0] = data[0];
  data=temp;
  chksum += command(dev, 32, 0xA, 0xB, 0xC, 0xD, 0x0, 0x0, 0x2);
  if (debug>2)printf("%d\n",chksum);
  chksum += command(dev, 32, data[0], 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
  for (i=0; i<7; i++){
    chksum += command(dev, 32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 );
  }
  chksum += command(dev, 32, 0xA, 0xB, 0xC, 0xD, 0x0, 0x0, 0x1);

  if (chksum != 320){
    if (chksum > 320){
      fprintf(stderr,"[read] The device returned more bytes than expected\n");
    } else {
      fprintf(stderr,"[read] The device returned less bytes than expected %d | 320 - %s\n",chksum ,usb_strerror());
    }
    return NULL;
  }

  chksum = usb_control_msg(dev, 0xA1, 0x1, 0x300, 0x01, data, 32, CONNECTION_TIMEOUT);

  if (chksum != 32){
    fprintf(stderr,"[read] Error while reading the device - %s\n", usb_strerror());
    return NULL;
  }
  return data;
}

/*
* Return the temperatur in Celsius after two readings
*/
int read_temperature(usb_dev_handle *udev){
  char request[]  = {0x54,0x00};
  char *result;
  int tmp;
 
  if (debug>2)printf("read_temperatur\n");
  result = read_device(udev,request);
  tmp = result[0] + (result[1]/256); //TODO On device error result[0] & result[1] will be 0
  if (debug>2)printf("return values: %d - %d - %d\n",result[0], result[1], result[1] <<8);
  usleep(400);
  result = read_device(udev,request);
  tmp += result[0] + (result[1]/256);

  if (debug>2) printf("inner: %d\n",tmp /2);
  return tmp / 2 ;
}

/*
* @return returns the temperature in celsius
*/
int get_celsius(){
  struct usb_dev_handle *udev;
  int result;
  udev = find_device(); 
  if (udev != NULL){ 
    result = read_temperature(udev);
    cleanup(udev);
  } else {
   result = -400; 
  }
  return result;
}

/*
* @return returns the temperature in fahrenheit
*/
int get_fahrenheit(){
  struct usb_dev_handle *udev;
  int c;
  udev = find_device();
  if (udev != NULL){
    c = read_temperature(udev);
    cleanup(udev);
    return ( ( c * 9) / 5) + 32; 
  } else {
    return -400;
  }
}

/*
* Returns a Information Array of the device
* return[0] => Device ID
* return[1] => Calibration 1 Internal Sensor
* return[2] => Calibration 2 Internal Sendor
* return[3] => Calibration 1 External Sensor
* return[4] => Calibration 2 Exertnal Sensor
* The rest of the Result is unkown and unused
*/
char* get_device_info(usb_dev_handle *udev){
  char request[] = {0x52};
  return read_device(udev,request);
}


/*
* init the USB stuff to be ready to go!
*/
void init (){
 usb_init();
 if (debug >2) usb_set_debug(debug);
 usb_find_busses();
 usb_find_devices();

}
