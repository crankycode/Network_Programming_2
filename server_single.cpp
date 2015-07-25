#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <signal.h>

#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h> 

#include "utils.h"
#include "myconfiglib.h"
#include "clientReq.h"


int logging;
int recordingReq;
ClientReq clientReqTemp; 
Configfile configSetting;     
             
string serverInfo;


void
updateLogfile(int logType,int logHdl, ClientReq& clientReq, Configfile* config)
{
  struct tm* timeLc;
  time_t t;
  
  time(&t);
  timeLc = localtime(&t);
  
  char logBuf  [LOGBUF_LEN];
  
  if (logging == TRUE) {
    switch(logType)
    {
      case SERVER_TYPE:
        sprintf(logBuf,"%d/%d/%d  %d:%d:%2d server-single 1.2.2\n",
        timeLc->tm_mday, timeLc->tm_mon, timeLc->tm_year + EXTRA_YEAR,
        timeLc->tm_hour, timeLc->tm_min, timeLc->tm_sec);
        write(logHdl,logBuf,strlen(logBuf)); 
      case INIT:
        sprintf(logBuf,"%d/%d/%d  %d:%d:%2d initialization complete\n",
        timeLc->tm_mday, timeLc->tm_mon, timeLc->tm_year + EXTRA_YEAR,
        timeLc->tm_hour, timeLc->tm_min, timeLc->tm_sec);
        write(logHdl,logBuf,strlen(logBuf));          
      break;
      case LISTEN:
        sprintf(logBuf,"%d/%d/%d  %d:%d:%2d listening on port %d\n",
        timeLc->tm_mday, timeLc->tm_mon, timeLc->tm_year + EXTRA_YEAR,
        timeLc->tm_hour, timeLc->tm_min, timeLc->tm_sec, config->port);
        write(logHdl,logBuf,strlen(logBuf));
      break;

      case ACCEPT:
        sprintf(logBuf,"%d/%d/%d %s %d:%d:%2d %s %d %s %s %d %d\n",
        timeLc->tm_mday, timeLc->tm_mon, timeLc->tm_year + EXTRA_YEAR,
        config->startTime,
        timeLc->tm_hour, timeLc->tm_min, timeLc->tm_sec,
        clientReq.ip.c_str(), config->port, clientReq.reqFileName.c_str(),
        clientReq.statusCode.c_str(), clientReq.dataWriten, errno);
        write(logHdl,logBuf,strlen(logBuf));
      break;
      
      case SHUTDOWN:
        sprintf(logBuf,"%d/%d/%d  %d:%d:%2d shutdown request\n",
        timeLc->tm_mday, timeLc->tm_mon, timeLc->tm_year + EXTRA_YEAR,
        timeLc->tm_hour, timeLc->tm_min, timeLc->tm_sec);
        write(logHdl,logBuf,strlen(logBuf)); 
      break;
    }      
  }
}

void 
setLoggingSetting(int* logHdl, int* reqLogHdl)
{
 // Set logging and request logfile flags
  if(logging == TRUE) {
    *logHdl = open(configSetting.logfile,  O_RDWR | O_CREAT | O_APPEND, 0777);
    
    if(*logHdl == -1) {
      fprintf(stderr,"Fail to open logfile\n");
      exit(1);
    }
  }
  
  if(recordingReq == TRUE) {
	 *reqLogHdl = open(configSetting.recordfile, O_RDWR | O_CREAT | O_APPEND, 0777);

	 if(*reqLogHdl == -1) {
		fprintf(stderr,"Fail to open request receive logfile\n");
		exit(1);
	 }
  }  
}

void 
setServerInfo(struct utsname& unameData)
{
  ostringstream oss;
  
  oss << unameData.sysname << "/" << unameData.release << ":" << 
         unameData.version;
  serverInfo = oss.str();
  
  strcpy(configSetting.server,serverInfo.c_str()); 
}

void 
setServerIpAddr(char* srvIp)
{
  struct in_addr address, **addrptr;
  struct hostent *answer;
  string serverIp;
  
 /* If the argument looks like an IP, assume it was one and 
  perform a reverse name lookup */ 
  if (inet_aton(configSetting.host, &address))
      answer = gethostbyaddr((char *) &address, sizeof(address),AF_INET);
  else
      answer = gethostbyname(configSetting.host);
      
  /* The hostname lookup failed :-( */
  if (!answer) {
      fprintf(stderr,"error looking up hostname\n");
      exit(1);
  }

  for (addrptr = (struct in_addr **) answer->h_addr_list; *addrptr; addrptr++)
  {    
    strcpy(srvIp,inet_ntoa(**addrptr));
    break;
  }  
  
}

void 
createStatusHTM(string& statusPg,configfile& cfg)
{
  struct tm* timeLc;
  time_t t;
  
  time(&t);
  timeLc = localtime(&t);
    
  ostringstream oss;
  
  oss << "<html>\r\n";
  oss << "<head>\r\n";
  oss << "<title> " << " " << " </title>\r\n";  
  oss << "</head>\r\n";
  oss << "<body>\r\n";
  
  oss << "<table width=\"518\" border=\"0\" align=\"center\">\r\n";
  oss << "<tr>";
  oss << "<td width=\"416\"><p><h1>Server Status Page</h1>\r\n";
  oss << "<p>"                             << cfg.server      << "<br />\r\n";
  oss << "Status at "                      << timeLc->tm_mday << " ";
  oss << month[timeLc->tm_mon][FIRST_ELE]  << " ";
  oss << timeLc->tm_year + EXTRA_YEAR      << ", ";
  oss << timeLc->tm_hour << ":" << timeLc->tm_min << ":" << timeLc->tm_sec;
  oss << "<br />\r\n";
  oss << "Active connections: "            << cfg.activeCon   << "<br />\r\n";
  oss << "Total requests: "                << cfg.totalReq    << "<br />\r\n";
  oss << "Listening port: "                << cfg.port        << "<br />\r\n";
  oss << "To shutdown, do &quot;"          << cfg.shutdown_command;
  oss << " or click <a href=\""            << cfg.shutdown_command;
  oss << "\">here</a>.<br /></p>\r\n";
  oss << "</td></tr></table>\r\n";
  oss << "</body>\r\n";
  oss << "</html>\r\n";
  
  statusPg = oss.str();
}

void
signal_Handler(int signo)
{
//cout << signo << endl;
  switch (signo)
  {
    case SIGTERM:
      cout << "shutting down" << endl;
      exit(1);
    break;
    case SIGPIPE:
      cout << "client disconnect" << endl;
    break;
    case SIGALRM:
      cout << "client timeout" << endl;
    break;
  }
}

int 
main(int argc, char** argv)
{
  struct sockaddr_in xferServer, xferClient;
  struct utsname unameData;

  int socket1,socket2;
  int addrlen;
  int returnStatus;
  int result;
  int val;
  
  int logHdl;    // Logfile handle
  int reqLogHdl; // Record request file handle
  
  time_t t;
  struct tm* timeLc;
  
  char srvIp[IP_LEN];
  string host;
  
  // Check if arguments was entered
  if(argc != 2) {
      fprintf(stderr,"<config.cfg file> needed\n");
      exit(1);
  }
  
  // Retrieve server name and version number
  result = uname(&unameData);

  if(result == -1) { 
    fprintf(stderr,
    "Could not retrieve server information\n");
    exit(1);
  }

  // Parse config file and init configSetting struct
  parseConfigFile(argv[1], &configSetting);  
  logging      = configSetting.logging;
  recordingReq = configSetting.recording;
  
  sprintf(configSetting.shutdown_command,"kill -%d %d",
          configSetting.shutdown_signal, (int)getpid());
  // Set server information
  setServerInfo(unameData);
  // Set logging and recording requst setting
  setLoggingSetting(&logHdl, &reqLogHdl);
  // Set server Ip address
  setServerIpAddr(srvIp);

  socket1 = socket(AF_INET, SOCK_STREAM, 0);
  // Update Log
  updateLogfile(SERVER_TYPE,logHdl,clientReqTemp, &configSetting);
  /* create a socket */
  if (socket1 == -1)
  {
    fprintf(stderr,
    "Could not create socket!\n");
    exit(1);
  }
  
  val = 1;
  result = setsockopt(socket1, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  if (result == -1)
  {
    fprintf(stderr,
    "Could not create socket!\n");
    exit(1);
  }
      
  xferServer.sin_family = AF_INET;
  xferServer.sin_addr.s_addr = inet_addr(srvIp);
  xferServer.sin_port = htons(configSetting.port);
  
  returnStatus = bind(socket1,(struct sockaddr*) &xferServer,
                      sizeof(xferServer));
  
  if (returnStatus == -1)
  {
    fprintf(stderr,
    "Could not bind to socket!\n");
    exit(1);
  }
  returnStatus = listen(socket1, 5);
  if (returnStatus == -1)
  {
    fprintf(stderr,
    "Could not listen on socket!\n");
    exit(1);
  }
  updateLogfile(INIT,logHdl,clientReqTemp, &configSetting);
  for(;;)
  {
    int fd;
    int i, readCounter, writeCounter;
    unsigned int found;
    char* bufptr;
    char buf[MAXBUF];
    char request[MAXBUF];
    char clientIp[IP_LEN];
    ostringstream oss;

    // Set Signals
    signal(SIGPIPE,signal_Handler);  
    signal(SIGALRM,signal_Handler);
    signal(SIGABRT,signal_Handler);  
    signal(SIGTERM,signal_Handler); 
  
    addrlen = sizeof(xferClient);
    
    // Update logfile listening on port
    updateLogfile(LISTEN,logHdl,clientReqTemp,&configSetting);
    socket2 = accept(socket1,(struct sockaddr*) &xferClient, 
                    (socklen_t*)&addrlen);
    
    if (socket2 == -1)
    {
      fprintf(stderr, "Could not accept connection!\n");
      exit(1);
    }
//sleep(5);
    // Increament active connection, total request
    configSetting.activeCon++;
    configSetting.totalReq++;
    
    // Create new clientReq obj and init some default setting
    ClientReq *clientReq      = new ClientReq();    
    clientReq->serverHostName = (string)configSetting.host;
    clientReq->reqFileName    = configSetting.root;
    clientReq->reqFileName.append("/");

    // Retrieve client ip address
    inet_ntop(AF_INET,&(xferClient.sin_addr), clientIp,INET_ADDRSTRLEN);
    clientReq->ip = (string)clientIp;
    // Update time for logging
    time(&t);
    timeLc = localtime(&t);
    oss << timeLc->tm_hour << ":" << timeLc->tm_min << ":" << timeLc->tm_sec;
    clientReq->startTime = oss.str();

    // Load client request into request buffer
    memset(request,'\0',MAXBUF);    
    i = 0;

    // Read client request
    while ((readCounter = read(socket2,request, MAXBUF)) > 0 ){    
      // Start timeout count down
      alarm(TIMEOUT);
      // Error while reading. Beakout of loop first
      if (readCounter == -1) {
         break;
      }

      clientReq->clientRequest.append(request);
      memset(request,'\0',MAXBUF);
      // End of request file
      if ((found = clientReq->clientRequest.find("\r\n\r\n")) != string::npos)
        break;
    }
    alarm(DISABLE);
    cout << "completed :\n" << clientReq->clientRequest << endl;
    
    if (readCounter == -1) {
      if (errno == SIGILL || errno == SIGWAITING) {
        cout << "Invalid Request" << endl;
        fprintf(stderr,
        "Could not read filename from socket!\n");
      }
      //Decreament active connection
      configSetting.activeCon--;
      shutdown(socket2,SHUT_RD);
      close(socket2);  
      // update log      
      updateLogfile(SHUTDOWN,logHdl, *clientReq, &configSetting);      
      delete clientReq;    
      continue;
    }
    
    if ( recordingReq == TRUE)
      write(reqLogHdl, clientReq->clientRequest.c_str(), strlen(request));
    
    // Parse client request
    clientReq->reqResult = clientReq->parseClientRequest(configSetting);

    if (clientReq->reqResult == FALSE) {
      // Client request failed
      // Write header
      writeCounter = write(socket2, clientReq->replyHeader.c_str(),
                           clientReq->replyHeader.length());
      // Write error page
      writeCounter = write(socket2, clientReq->statusPageReply.c_str(),
                           clientReq->statusPageReply.length());
      // Decreament active connection
      configSetting.activeCon--;                           
      shutdown(socket2,SHUT_RDWR);      
      close(socket2);      
      // update log      
      updateLogfile(ACCEPT,logHdl, *clientReq, &configSetting); 
      delete clientReq;      
      continue;
    }
    else if (clientReq->reqResult == SHUTDOWN) { 
      // Shutdown server
      // Decreament active connection
      // update log      
      updateLogfile(SHUTDOWN,logHdl, *clientReq, &configSetting);
      configSetting.activeCon--;      
      shutdown(socket2,SHUT_RDWR);        
      close(socket2);
      delete clientReq;
		
      kill(getpid(),configSetting.shutdown_signal);
    }
    else if (clientReq->reqResult == STATUS_REQ) {
      string statusPg; 
      createStatusHTM(statusPg, configSetting);
      clientReq->createReplyHeader(statusPg.length(),configSetting);
      write(socket2,clientReq->replyHeader.c_str(),
                         clientReq->replyHeader.length());
      write(socket2,statusPg.c_str(),statusPg.length());   
      configSetting.activeCon--;        
      shutdown(socket2,SHUT_RDWR);        
      close(socket2);
      // update log      
      updateLogfile(ACCEPT,logHdl, *clientReq, &configSetting);
      delete clientReq;
      continue;                                
                           
    }
    else if (clientReq->reqResult == TRUE) {
      // Client request successed
      // Open the file for reading
      fd = open(clientReq->reqFileName.c_str(), O_RDONLY);
      if (fd == -1)
      {
        fprintf(stderr, "Could not open file for reading!\n");
        // Decreament active connection
        configSetting.activeCon--;      
        shutdown(socket2,SHUT_RDWR);  
        close(socket2);
        // update log
        updateLogfile(ACCEPT,logHdl, *clientReq, &configSetting);
        delete clientReq;        
        continue;
      }
 
      // Calulate request filesize
      stat(clientReq->reqFileName.c_str(), &(clientReq->fileInfo));
      clientReq->fileSize = clientReq->fileInfo.st_size;
      
      // Create and send reply Header
      clientReq->createReplyHeader(clientReq->fileSize, configSetting);
      writeCounter = write(socket2,clientReq->replyHeader.c_str(),
                           clientReq->replyHeader.length());
      
      // Send client request
      while((readCounter = read(fd, buf, MAXBUF)) > 0)
      {
        writeCounter = 0;
        bufptr = buf;
        while (writeCounter < readCounter) {
          readCounter -= writeCounter;
           clientReq->dataLeft = readCounter;
          bufptr += writeCounter;
          writeCounter = write(socket2, bufptr, readCounter);
          clientReq->dataWriten +=writeCounter;
          if (writeCounter == -1) {
            fprintf(stderr,
            "Could not write file to client!\n");
           
            break;
          }
        }
        if (writeCounter == -1) {
            if (errno == EPIPE) {
            fprintf(stderr,
            "errno:%d, %s\n",
             errno,strerror(errno));
           }
         
          break;
        }        
      }
      // update log
      updateLogfile(ACCEPT,logHdl, *clientReq, &configSetting);
      close(fd);
    }
      // Decreament active connection
cout << "releasing resource" << endl;
      configSetting.activeCon--;    
      shutdown(socket2,SHUT_RDWR);
      close(socket2);
      delete clientReq;
  }
  close(socket1);
  return 0;
}
