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
  printf("  -line1 \"string\": Set the first line of the LCD to this value. String must be 16 characters or less.\n");
  printf("  -line2 \"string\": Set the second line of the LCD to this value. String must be 16 characters or less.\n");
  printf("  -lines \"line1\" \"line2\": Set line1 and line2 of the LCD to these values. Strings must be 16 characters or less.\n");
  printf("  -setboot: set LCD screen nv memory to the contents of the screen. Will be done after line1 and line2 are processed, if set.\n");
  exit(1);
}
char* get_param(int idx, int argc, char* argv[]) {
  if (idx >= argc) {
    printf("ERROR: not enough arguments\n");
    exit(1);
  }
  return argv[idx];
}

//============================================================================
int main(int argc, char* argv[])
  {
  //If only 0 or 1 parameter is entered, prompt for the missing parameter(s)
/*
  if(argc < 4)
    {

    }
*/

  int baud = 19200;
  char *port = "/dev/ttyUSB0";
  int cleardisplay = 0;
  int setboot = 0;
  char *line1 = "";
  char *line2 = "";

  if (argc == 1) usage();

  for(int i =  1; i< argc ; i++) {
    if (!strcmp(argv[i], "-port")) port = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-baud")) baud = atoi(get_param(++i, argc, argv));
    else if (!strcmp(argv[i], "-clear")) cleardisplay = 1;
    else if (!strcmp(argv[i], "-setboot")) setboot = 1;
    else if (!strcmp(argv[i], "-line1")) line1 = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-line2")) line2 = get_param(++i, argc, argv);
    else if (!strcmp(argv[i], "-lines")) { line1 = get_param(++i, argc, argv); line2 = get_param(++i, argc, argv); }
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

  if (cleardisplay) {
    outgoing_response.command = 6;
    outgoing_response.data_length = 0;
    send_packet();
  }
  if (strlen(line1) > 0) {
    printf("Setting line1: `%s`\n", line1);
    outgoing_response.command = 31;
    outgoing_response.data[0]=0; //col
    outgoing_response.data[1]=0; //row
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
  if (setboot) {
    outgoing_response.command = 4;
    outgoing_response.data_length = 0;
    send_packet();
  }

  printf("Done.\n");
  Uninit_Serial();
  return 0;
  }
//============================================================================

void get_packets() {
  int k;
  int timed_out;
  timed_out = 1; //default timed_out is true
  for(k=0;k<=10000;k++)
    if(check_for_packet())
      {
      ShowReceivedPacket();
      timed_out = 0; //set timed_out to false
      break;
      }
  if(timed_out)
    printf("Timed out waiting for a response.\n");
}
