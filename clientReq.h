#ifndef CLIENTREQ_H
#define CLIENTREQ_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utils.h"
#include "myconfiglib.h"

using namespace std;

class ClientReq
{
  
public:
  // server_select var
  int  reqSocket;
  int  waitingFD;
  int  waitType;
  int  nextAction;
  int  lastRead;
  int  readFileFd;
  
  int  reqStatusPage;
  int  noFile_exe;
  int  dataWriten;
  int  dataLeft;
  int  fileSize;
  int  reqResult;


  struct stat fileInfo;
    
  string ip;  
  string httpVer;
  string clientRequest;
  string replyHeader;
  string reqFileName;  
  string contentType;
  string statusCode;  
  string startTime;  
  string statusPageReply;
  string serverHostName;
  string readInFileData;
  
  ClientReq();
  virtual ~ClientReq()// destructor
  {
  }

  int  parseClientRequest   (configfile &cfg);
  void setContentType       (configfile &cfg, string ext);
  void createReplyHeader    (int contentLt, configfile &cfg);
  void createStatusErrorPage();  
  void createShutDownPage   ();
};

#endif
