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
  unsigned int c, index, critical, warning;
  opterr = 0;
  char *value = NULL;
  char *ptr;
  char mode = 'r';
  
  init_struct();

  while ((c = getopt(argc,argv,"hv:b:d:w:c:lV")) != -1){
    switch (c){
      case 'h':/*-h Help*/
        printUsage(argv[0]);
        break;
      case 'b':/*Bus:Device*/
        value = optarg;
        ptr = strtok(value,":");
        config->bus = (char *) malloc(sizeof(ptr));
        strcpy(config->bus, ptr);
        ptr = strtok(NULL, ":");
        config->device = (char *) malloc(sizeof(ptr));
        strcpy(config->device, ptr);
        break;
      case 'd':/*debug 0..3*/
        value = optarg;
        debug = atoi(value);
        printf("=> debug level: %d\n",debug);
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
      case 'v':/*define the vendor and product to use*/
        printf("vendor\n");
        value = optarg;
        if (!strcmp(value,"TEMPerV1")){
          config->vendor = TEMPER_VENDOR;
          config->product = TEMPER_PRODUCT;
          printf("not implented yet\n");
          leave(3);
        } else if (!strcmp(value,"TEMPerHID")){
          config->vendor = HidTEMPer_VENDOR;
          config->product = HidTEMPer_PRODUCT;
        } else if (strstr(value,":")){
          ptr = strtok(value,":");
          config->vendor = strtol(ptr, NULL, 16);
          ptr = strtok(NULL,":");
          config->product = strtol(ptr, NULL, 16);
          printf("vendor: %04X; product: %04X\n", config->vendor, config->product);
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
         printf ("Non-option argument %s\n", argv[index]);
    }
  }
  //TODO: struct testen und los laufen
  switch (mode){
    case 'l':
      init();
      list_supported_devices();
      break;
    case 'r':
      init();
      if (config->device == 0 && config->product == 0){
        printUsage(argv[0]);
      }
      find_device();
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

  config = (struct usb_config *) malloc(sizeof(struct usb_config *));
  
  config->vendor = HidTEMPer_VENDOR;
  config->product = HidTEMPer_PRODUCT;

  debug = 0;
} 

void leave(int exitCode){
  free(config);
  exit(exitCode);
}
