// 633_COMMAND_LINE_TEST.cpp
// Ultra-simple 633 command-line communications example

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "typedefs.h"
#include "serial.h"
#include "cf_packet.h"
#include "show_packet.h"

void get_packets();
void usage() {
  printf("\nlcd-cmd - a tool to work with CrystalFontz CFA-533 and CFA-566 LCD screens\n\n");
  printf("Usage: lcd-cmd [options]\n");
  printf("Options:\n");
  printf("  -port: the TTY this talks to like \"/dev/ttyS0\" or \"/dev/usb/ttyUSB0\" (default: /dev/ttyUSB0)\n");
  printf("  -baud: is 19200 or 115200 (default: 19200)\n");
  printf("  -clear: clear the LCD screen. If lines to display are set, this is done before displaying them.\n");
  printf("  -brightness S T: Set the brigtness of the screen and touchpad (between 0 - 100)\n");
  printf("  -contrast VALUE: Set the contrast of the screen (between 0 - 200)\n");
  printf("  -line1 \"string\": Set the first line of the LCD to this value. String must be 16 characters or less.\n");
  printf("  -line2 \"string\": Set the second line of the LCD to this value. String must be 16 characters or less.\n");
  printf("  -lines \"line1\" \"line2\": Set line1 and line2 of the LCD to these values. Strings must be 16 characters or less.\n");
  printf("  -gpioconfigure INDEX ENALBE TYPE: Configure GPIO index to enable or disable (0|1) and the configuration type. Both are required. Index and TYPEs are described below.\n");
  printf("  -gpioset INDEX VALUE: Set GPIO index to value. Index values are described below. Value must be from 0 to 100. Note, GPIO configuration may need to be set first!\n");
  printf("  -gpioget INDEX: Get the GPIO configuration at index. Index values are described below.\n");
  printf("  -setboot: set LCD screen nv memory to the contents and configuration of the screen. Will be done after all other actions are processed.\n");
  printf("  -reboot: reboot the LCD screen. Useful for testing boot state.\n");
  printf("  -ping: ping the device for a response.\n");
  printf("\n");
  printf("GPIO notes:\n");
  printf("Index (NUM) and location:\n");
  printf("  0 = J8, Pin 7\n");
  printf("  1 = J8, Pin 6\n");
  printf("  2 = J8, Pin 5\n");
  printf("  3 = J8, Pin 4\n");
  printf("  4 = J9, Pin 2\n");
  printf("\n");
  printf("Values for GPIO set:\n");
  printf("  Set values may be from 0 to 100 with 0 being 'off' and 100 being 'on'. 1-99 are output duty cycle percentages.\n");
  printf("\n");
  printf("Configure TYPE may be one of the following:\n");
  printf("  0 - 1=Resistive Pull Up,     0=Fast, Strong Drive Down\n");
  printf("  1 - 1=Fast, Strong Drive Up, 0=Fast, Strong Drive Down\n");
  printf("  2 - Hi-Z, use for input\n");
  printf("  3 - 1=Resistive Pull Up,     0=Fast, Strong Drive Down\n");
  printf("  4 - 1=Slow, Strong Drive Up, 0=Hi-Z\n");
  printf("  5 - 1=Slow, Strong Drive Up, 0=Slow, Strong Drive Down\n");
  printf("  7 - 1=Hi-Z,                  0=Slow, Strong Drive Down\n");
  exit(1);
}
char* get_param(int idx, int argc, char* argv[]) {
  if (idx >= argc) {
    printf("ERROR: not enough arguments\n");
    exit(1);
  }
  return argv[idx];
}

int get_gpio_idx(char* idx) {
  if (!strcmp(idx, "0"))      return 0;
  else if (!strcmp(idx, "1")) return 1;
  else if (!strcmp(idx, "2")) return 2;
  else if (!strcmp(idx, "3")) return 3;
  else if (!strcmp(idx, "4")) return 4;

  printf("ERROR: GPIO index `%s` is not valid.", idx);
  usage();
  return -1; /*Silence compiler warning*/
}

//============================================================================
int main(int argc, char* argv[])
  {
  int baud = 19200;
  char *port = "/dev/ttyUSB0";
  int cleardisplay = 0;
  int setboot = 0;
  int contrast = -1;
  int brightness[2] = { -1, -1};
  char *line1 = "";
  char *line2 = "";
  int gpioset[5] = { -1,-1,-1,-1,-1 };
  int gpioget[5] = { 0, 0, 0, 0, 0 };
  int gpioconfigure[5] = { -1,-1,-1,-1,-1 };
  int reboot = 0;
  int ping = 0;

  if (argc == 1) usage();

  for(int i =  1; i< argc ; i++) {
    if (!strcmp(argv[i], "-port")) port = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-baud")) baud = atoi(get_param(++i, argc, argv));
    else if (!strcmp(argv[i], "-clear")) cleardisplay = 1;
    else if (!strcmp(argv[i], "-brightness")) { brightness[0] = atoi(get_param(++i, argc, argv)); brightness[1] = atoi(get_param(++i, argc, argv)); }
    else if (!strcmp(argv[i], "-contrast")) contrast = atoi(get_param(++i, argc, argv));
    else if (!strcmp(argv[i], "-line1")) line1 = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-line2")) line2 = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-lines")) { line1 = get_param(++i, argc, argv); line2 = get_param(++i, argc, argv); }
    else if (!strcmp(argv[i], "-gpioset")) {
      int gpio_idx = get_gpio_idx(get_param(++i, argc, argv));
      int gpio_val = atoi(get_param(++i, argc, argv));
      gpioset[gpio_idx] = gpio_val;
    }
    else if (!strcmp(argv[i], "-gpioconfigure")) {
      int gpio_idx = get_gpio_idx(get_param(++i, argc, argv));
      int gpio_enable = atoi(get_param(++i, argc, argv));
      int gpio_val = atoi(get_param(++i, argc, argv));
      char bin[5] = { gpio_enable == 0 ? '0' : '1', 0, 0, 0, '\0' };

      if (gpio_enable < 0 || gpio_enable > 1) {
        printf("ERROR: GPIO enable must be either 0 or 1\n");
        usage();
      }
      if (gpio_val < 0 || gpio_val > 7 || gpio_val == 6) {
        printf("ERROR: GPIO config type must be 0, 1, 2, 3, 4, 5, or 7\n");
        usage();
      }

      switch (gpio_val) {
        case 0:
          strcat(bin, "000"); break;
        case 1:
          strcat(bin, "001"); break;
        case 2:
          strcat(bin, "010"); break;
        case 3:
          strcat(bin, "011"); break;
        case 4:
          strcat(bin, "100"); break;
        case 5:
          strcat(bin, "101"); break;
        case 7:
          strcat(bin, "111"); break;
      }


      gpio_val = (int)strtol(bin, NULL, 2);
      printf("Converted GPIO configuration for pin %i to binary '%s' => %i\n", gpio_idx, bin, gpio_val);
      gpioconfigure[gpio_idx] = gpio_val;
    }
    else if (!strcmp(argv[i], "-gpioget")) {
      int gpio_idx = get_gpio_idx(get_param(++i, argc, argv));
      gpioget[gpio_idx] = 1;
    }
    else if (!strcmp(argv[i], "-setboot")) setboot = 1;
    else if (!strcmp(argv[i], "-reboot")) reboot = 1;
    else if (!strcmp(argv[i], "-ping")) ping = 1;
    else usage();
  }

  if (strlen(line1) > 16 || strlen(line2) > 16) {
    printf("ERROR: line cannot be be more than 16 characters\n");
    exit(2);
  }

  //Check for optional "clear" parameter and set flag if found
  if(Serial_Init(port,baud))
    {
    printf("Could not open port \"%s\" at \"%d\" baud.\n",port, baud);
    return(1);
    }
  else
    printf("\"%s\" opened at \"%d\" baud.\n",port,baud);

  //For some reason, Linux seems to buffer up data from the LCD, and they are sometimes
  //dumped at the start of the program. Clear the serial buffer.
  while(BytesAvail())
    GetByte();

  if (reboot) {
    outgoing_response.command = 5;
    outgoing_response.data[0] = 8;
    outgoing_response.data[1] = 18;
    outgoing_response.data[2] = 99;
    outgoing_response.data_length = 3;
    send_packet();

    /* If reboot is requested, other actions don't make sense, so exit here */
    get_packets();
    printf("Done.\n");
    Uninit_Serial();
    return 0;
  }
  if (ping) {
    outgoing_response.command = 0;
    outgoing_response.data[0] = 'p';
    outgoing_response.data[1] = 'i';
    outgoing_response.data[2] = 'n';
    outgoing_response.data[3] = 'g';
    outgoing_response.data_length = 4;
    send_packet();
    get_packets();
  }
  if (cleardisplay) {
    outgoing_response.command = 6;
    outgoing_response.data_length = 0;
    send_packet();
    get_packets();
  }
  if (brightness[0] >= 0) {
    outgoing_response.command = 14;
    outgoing_response.data[0] = brightness[0];
    outgoing_response.data[1] = brightness[1];
    outgoing_response.data_length = 2;
    send_packet();
    get_packets();
  }
  if (contrast >= 0) {
    outgoing_response.command = 13;
    outgoing_response.data[0] = contrast;
    outgoing_response.data_length = 1;
    send_packet();
    get_packets();
  }
  if (strlen(line1) > 0) {
    printf("Setting line1: `%s`\n", line1);
    outgoing_response.command = 31;
    outgoing_response.data[0] = 0; //col
    outgoing_response.data[1] = 0; //row
    memcpy(&outgoing_response.data[2],line1,strlen(line1));
    outgoing_response.data_length = strlen(line1) + 2;
    send_packet();
    get_packets();
  }
  if (strlen(line2) > 0) {
    printf("Setting line2: `%s`\n", line2);
    outgoing_response.command = 31;
    outgoing_response.data[0]=0; //col
    outgoing_response.data[1]=1; //row
    memcpy(&outgoing_response.data[2],line2,strlen(line2));
    outgoing_response.data_length = strlen(line2) + 2;
    send_packet();
    get_packets();
  }
  for (int i = 0 ; i<5 ; i++) {
    if (gpioconfigure[i] == -1) continue;
    printf("Configuring GPIO pin %d to: %d (binary int value)\n", i, gpioconfigure[i]);
    outgoing_response.command = 34;
    outgoing_response.data[0] = i; //Index
    outgoing_response.data[1] = 0; //Configure to low by default
    outgoing_response.data[2] = gpioconfigure[i]; //Setting
    outgoing_response.data_length = 3;
    send_packet();
    get_packets();
  }
  for (int i = 0 ; i<5 ; i++) {
    if (gpioset[i] == -1) continue;
    printf("Setting GPIO %d to: %d\n", i, gpioset[i]);
    outgoing_response.command = 34;
    outgoing_response.data[0] = i; //Index
    outgoing_response.data[1] = gpioset[i]; //Val
    outgoing_response.data_length = 2;
    send_packet();
    get_packets();
  }
  for (int i = 0 ; i<5 ; i++) {
    if (!gpioget[i]) continue;
    printf("Getting GPIO config for pin %d\n", i);
    outgoing_response.command = 35;
    outgoing_response.data[0] = i; //Index
    outgoing_response.data_length = 1;
    send_packet();
    get_packets();
  }

  if (setboot) {
    outgoing_response.command = 4;
    outgoing_response.data_length = 0;
    send_packet();
    get_packets();
  }

  printf("Done.\n");
  Uninit_Serial();
  return 0;
}
//============================================================================

void get_packets() {
  int ms_wait = 250;
  /* Doco says packet should always be delivered within 250ms */
  for (int i=0 ; i<ms_wait ; i++) {
    /* Sleep 1ms */
    usleep(1000);

    if(check_for_packet()) {
      printf("Packet delivered on loop %d (%d ms)\n", i, i);
      ShowReceivedPacket();
      return;
    }
  }

  printf("Timed out waiting for a response.\n");
}
