#ifndef _MYCONFIGLIB_H
#define _MYCONFIGLIB_H

/* System-wide header files. */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define DELIM          " "
#define FALSE          0
#define TRUE           1
#define SUCCESS        0
#define YES_LEN        3
#define NO_LEN         2
#define EXTRA_CHAR     1

typedef struct configfile {
  char file_exe         [8][2][20]; 
  char address          [50];
  char status_request   [50];
  char shutdown_request [50];
  char shutdown_file    [50];
  char recordfile       [25];
  char logfile          [20];
  char root             [50];
  char host             [50];
  char server           [30];
  char date             [35];
  char filename         [35];
  char ip               [16];
  char startTime        [15];
  char shutdown_command [20];
  
  int  logging;
  int  recording;
  int  shutdown_signal;
  int  noFile_exe;
  int  activeCon;
  int  totalReq;
  
  int  dataWriten;
  int  dataLeft;
  int  statusCode;
  int  print_message_details;
  int  port;
  int  request_count;
  int  request_timeout;
  int  support_timeout;
} Configfile;

void parseConfigFile(char* filename, Configfile* cfg);
void toLowerCase(char* str);
#endif
