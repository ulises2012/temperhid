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

#include <getopt.h>

#include "main.h"
#include "overall.h"
#include "temper.h"

int main (int argc, char **argv){
  unsigned int c, index;
  int critical, warning;
  opterr = 0;
  char *value = NULL;
  char *ptr;
  char mode = 'r';
  short fahrenheit = 0;
  
  init_struct();

  while ((c = getopt(argc,argv,"hv:b:d:w:c:lVF")) != -1){
    switch (c){
      case 'h':/*-h Help*/
        printUsage(argv[0]);
        break;
      case 'b':/*Bus:Device*/
        value = optarg;
        ptr = strtok(value,":");
        c_bus = ptr;
        ptr = strtok(NULL, ":");
        device = ptr;
        break;
      case 'd':/*debug 0..3*/
        value = optarg;
        debug = atoi(value);
        if (debug>2)printf("=> debug level: %d\n",debug);
        break;
      case 'c':/*Nagios: critical value*/
        value = optarg;
        critical = atoi(value);
        break;
      case 'w':/*Nagios: warning value*/
        value = optarg;
        warning = atoi(value);
        break;
      case 'l':/*List all known TEMPer devices*/
        mode = 'l';
        break;
      case 'V':/*Print version of this programm*/
        printf("USB Thermometer Nagios Plugin in Version: %s\n",VERSION);
        leave(3);
        break;
      case 'F':/*Set the output to fahrenheit*/
        fahrenheit = 1;
        break;
      case 'v':/*define the vendor and product to use*/
        value = optarg;
        if (!strcmp(value,"TEMPerV1")){
          u_config->vendor = TEMPER_VENDOR;
          u_config->product = TEMPER_PRODUCT;
          if (debug >2)printf("not implented yet\n");
          leave(3);
        } else if (!strcmp(value,"TEMPerHID")){
          u_config->vendor = HidTEMPer_VENDOR;
          u_config->product = HidTEMPer_PRODUCT;
        } else if (strstr(value,":")){
          ptr = strtok(value,":");
          u_config->vendor = strtol(ptr, NULL, 16);
          ptr = strtok(NULL,":");
          u_config->product = strtol(ptr, NULL, 16);
          if (debug>2)printf("vendor: %04X; product: %04X\n", u_config->vendor, u_config->product);
        } else {
          printf("Please define the VENDOR:PRODUCT like 1130:660C or give the "
                 "pre-defined Vendors (please have a look to the manual)\n");
          leave(3);
        }
        break;
      case '?':
        if (optopt == 'd' || optopt == 'c' || optopt == 'w'){
          fprintf(stderr, "Option -%c needs an argument.\n", optopt);
        } else if (isprint (optopt)){
          fprintf(stderr, "Unknown option -%c.\n", optopt);
        } else {
          fprintf(stderr, "Unknown character \\x%x.\n", optopt);
        }
        leave(3);
        break;
      default:
        printUsage(argv[0]);
    }
    
    for (index = optind; index < argc; index++){
         //printf ("Non-option argument %s\n", argv[index]);
    }
  }
  //TODO: struct testen und los laufen
  switch (mode){
    case 'l':
      init();
      list_supported_devices();
      break;
    case 'r':
      if (u_config->vendor == 0 && u_config->product == 0){
        printUsage(argv[0]);
      }
      if (c_bus == NULL && device == NULL){
        printUsage(argv[0]);
      }
      if (warning == 0 && critical == 0){
        printUsage(argv[0]);
      }
      init();
      //usbdev = find_device();
      if (fahrenheit){
        c = get_fahrenheit();
      } else {
        c = get_celsius();
      }
      check_threshold(c, warning, critical);
      break;
    default:
      printUsage(argv[0]);
  }
  //TODO: check -c && -w 
  leave(3);
  return 3;
}

void printUsage(char *prog){
  printf("The usage of the programm: \n%s -w N -c N -b NN:NN [ [ -v NN:NN ] [ -d 0..3 ] ]\n\n",prog);
  printf("\t-b \t set the BUS:DEVICE address of the device\n");
  printf("\t-d \t set the debug mode (0..3)\n");
  printf("\t-h \t this help\n");
  printf("\t-v \t set the VENDOR:PRODUCT of the device or predefined vendors\n");
  printf("\t-V \t version of this programm\n");
  printf("\t-l \t list all supported and connected devices\n");
  printf("\t-w \t set the warning value\n");
  printf("\t-c \t set the critical value\n");
  leave(3);
}

void init_struct(){

  u_config = (struct usb_config *) malloc(sizeof(struct usb_config *));
  
  u_config->vendor = HidTEMPer_VENDOR;
  u_config->product = HidTEMPer_PRODUCT;

  debug = 0;
} 

void leave(int exitCode){
  free(u_config);
  exit(exitCode);
}

void check_threshold(int temperature, int warning, int critical){
  int exitCode = 0;
  char *color = "\033[m";
  if (temperature >= critical){
    exitCode = 2;
    color = "\033[31m";
  } else if (temperature >= warning) {
    exitCode = 1;
    color = "\033[33m";
  }
  #if defined __coloredout__
    printf("%s%d\n\033[m" ,color ,temperature);
  #else
    printf("%d\n" ,temperature);
  #endif
  leave(exitCode);
}
