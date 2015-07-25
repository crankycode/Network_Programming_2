#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cerrno>

#define TRUE          1
#define FALSE         0
#define SHUTDOWN      3
#define SUCCESS       0
#define STATUS_REQ    4
#define EXTRA_5       5
#define EXTRA_1       1
#define MISSING       2
#define HTTP10        "HTTP/1.0"
#define MAXBUF        1024
#define MAX_NAME      20
#define HOSTLABLE_LEN 6
#define EXTRA_YEAR    1900
#define LOGBUF_LEN    120
#define TIMEZONE_LEN  4
#define FIRST_ELE     0
#define IP_LEN        16
#define TIMEOUT       10
#define DISABLE       0

static const char *month[12][1] = {{"Jan"},{"Feb"},{"Mar"},{"Apr"},{"May"},
                     {"Jun"},{"Jul"},{"Aug"},{"Sep"},{"Oct"},{"Nov"},{"Dec"}};
                     
static const char *day[7][1] = {{"Mon"},{"Tue"},{"Wed"},{"Thu"},{"Fri"},
                     {"Sat"},{"Sun"}};   
                                
enum CurrAct{INIT,LISTEN,ACCEPT,REQUEST,SEND,SERVER_TYPE,ALLCLOSE};
enum WaitType{WAIT_READ, WAIT_WRITE, NOT_WAIT};
enum NextAction{ST_ACCUM_REQ,ST_WRITE_HEADER,ST_READ_DATA,ST_SEND_DATA,
                ST_WRITE_ERROR_PAGE_HEADER,ST_SEND_STATUS_PAGE,ST_CLOSE_CONN};
                
#endif
