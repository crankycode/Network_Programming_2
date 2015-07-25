#include "clientReq.h"

ClientReq::ClientReq()
{
  
}

int 
ClientReq::parseClientRequest(configfile& cfg)
{
  size_t found;
  size_t ending;
  string line;
  string rawReqFileName;
  string ext;
  unsigned int    pos;
  int    testHdl;
  
  // Check if HTTP/1.0 is in the get request header
  found = clientRequest.find("HTTP/1.1");
  
  if ( clientRequest.length() >= 4 + serverHostName.length()) {
      if (serverHostName == clientRequest.substr(4,serverHostName.length())) {
      //cout << httpVer << endl;
        httpVer = "HTTP/1.0";
      }
  }
  if (found == string::npos) {
      httpVer = "HTTP/1.0";
  }
  if (found != string::npos){
      httpVer = "HTTP/1.1";
cout << httpVer << endl;
  }
  
  // Check if its request method is using GET
  if (clientRequest.substr(0,3) != "GET")
  {
    statusCode = "405 Method Not Allowed";
    setContentType(cfg, ".html");
    createStatusErrorPage();    
    createReplyHeader(statusPageReply.length(), cfg);    
    return FALSE;
  }  
  
  // Check if header got Host: and if the host address is for this server
  found = clientRequest.find("Host:");
  
  if (clientRequest.substr(found+6,serverHostName.length()) != serverHostName ||
     found == string::npos)
  {
    if (found == string::npos) {
      statusCode = "400 Bad Request";
      setContentType(cfg, ".html");
      createStatusErrorPage();
      createReplyHeader(statusPageReply.length(), cfg);      
      return FALSE;
    }
    else {
      statusCode = "305 Use Proxy";
      createStatusErrorPage();
      createReplyHeader(statusPageReply.length(), cfg);      
      return FALSE;
    }
  }
  
  // Retrieve file request into
  found  = clientRequest.find(" ");
  ending = clientRequest.find(" ",found + 1); 
 
  // Store filename from req header, with %20  
  rawReqFileName = clientRequest.substr(found + MISSING,ending - EXTRA_5);

  // Convert %20 to space
  pos = rawReqFileName.find("%20");

  while( pos != string::npos)
  {
	  rawReqFileName.replace(pos, 3, " ");
	  pos = rawReqFileName.find("%20",pos + 1);
  }
  
  // Find if its a shutdown_command "kill signal no."
  found = rawReqFileName.find((string)cfg.shutdown_command);
  
  // Check if its a shutdown request
  if (found != string::npos) {
    statusCode = "200 OK";
    setContentType(cfg, ".html");    
    createShutDownPage();
    createReplyHeader(statusPageReply.length(), cfg);   
    return SHUTDOWN;   
  }
  
  // Find if its a shutdown_request " /config/shutdown.htm
  found = rawReqFileName.find((string)cfg.shutdown_request);
  
  if (found != string::npos) {
    statusCode = "200 OK";
    setContentType(cfg, ".html");    
    createShutDownPage();
    createReplyHeader(statusPageReply.length(), cfg);   
    return SHUTDOWN;
  }
  
  // Check if its a request for status page
  if (rawReqFileName == (string)cfg.status_request) {
      setContentType(cfg,".htm");
      return STATUS_REQ;
  }
  // Combine filename with rootpath
  reqFileName.append(rawReqFileName);
  
  // Check if file exist
  testHdl = open(reqFileName.c_str(), O_RDONLY);

  if (testHdl == -1)
  {
    statusCode = "404 Not Found";
    setContentType(cfg, ".html");    
    createStatusErrorPage();
    createReplyHeader(statusPageReply.length(), cfg);
cout << "File don't exist" << endl;

    return FALSE;
  } 
  close(testHdl);
  statusCode = "200 OK";
  // Set content type
  setContentType(cfg, reqFileName);  
  return TRUE;
}

void
ClientReq::setContentType(configfile &cfg, string filename)
{
  string cfgExt;
  string fileExt;
  size_t found;
  
  found   = filename.find_last_of(".");
  fileExt = filename.substr(found + EXTRA_1);  
  if (found != string::npos) {
    for (int i = 0; i < cfg.noFile_exe; i++) 
    {
      cfgExt = (string)(cfg.file_exe[i][0] + 5);
      if (cfgExt == fileExt) {
        contentType = (string)(cfg.file_exe[i][1]);
        return;
      }
    }
  }
  // Unknown extension, set default contentType
  contentType = "text/plain";
}

void 
ClientReq::createReplyHeader(int contentLt, configfile& cfg)
{
  ostringstream oss;
  string        date;
  
  time_t t;
  struct tm* timeGMT;
  time(&t);
  timeGMT = gmtime(&t);
  
  // Get GMT time
  oss << day[timeGMT->tm_wday][FIRST_ELE]  << ", " << timeGMT->tm_mday << " " <<
         month[timeGMT->tm_mon][FIRST_ELE] << " "  << 
         timeGMT->tm_year + EXTRA_YEAR     << " "  << timeGMT->tm_hour << ":" <<
         timeGMT->tm_min                   << ":"  << timeGMT->tm_sec  <<" GMT";
  date = oss.str();
  
  // Clear oss buffer
  oss.str(std::string());
  
  // Create replayheader 
  oss << httpVer   << " "   << statusCode  << "\r\n" <<
         "Host: "           << cfg.host    << "\r\n" <<
         "Date: "           << date        << "\r\n" <<
         "Content-Length: " << contentLt   << "\r\n" <<
         "Content-Type: "   << contentType << "\r\n" <<
         "Connection: "     << "close"     << "\r\n" <<
         "Server: "         << cfg.server  << "\r\n" <<
                                              "\r\n";
 replyHeader = oss.str();   
}

void 
ClientReq::createStatusErrorPage()
{
  ostringstream oss;
  
  oss << "<html>\r\n";
  oss << "<head>\r\n";
  oss << "<title> " << statusCode << " </title>\r\n";  
  oss << "</head>\r\n";
  oss << "<body>\r\n";
  oss << "<h1>"     << statusCode << "</h1>\r\n";
  oss << "</body>\r\n";
  oss << "</html>\r\n";
  
  statusPageReply = oss.str();
}

void 
ClientReq::createShutDownPage()
{
  ostringstream oss;
  
  oss << "<html>\r\n";
  oss << "<head>\r\n";
  oss << "<title> " << "Server Shutting Down" << " </title>\r\n";  
  oss << "</head>\r\n";
  oss << "<body>\r\n";
  oss << "<h1>"     << "Server Shutting Down" << "</h1>\r\n";
  oss << "</body>\r\n";
  oss << "</html>\r\n";
  
  statusPageReply = oss.str();
}

