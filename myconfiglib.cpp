#include "myconfiglib.h"

void toLowerCase(char *str)
{
  char *temp = str;
  int  i     = 0;
  for(i = 0; i < (unsigned char)strlen(str); i++) {
   /* remove any '\n' or '\r' */
	 if(str[i] == '\n' || str[i] == '\r')
		str[i] = '\0';
	  str[i] = tolower(str[ i ]);
  }
  str = temp;
}

void ckTabAndNewLine(char *str)
{
  if (str[strlen(str) - 1] == '\n')
      str[strlen(str) - 1] = '\0';
     
  if (str[strlen(str) - 1] == '\r')
      str[strlen(str) - 1] = '\0';
}

void parseConfigFile(char* filename, Configfile* cfg) 
{
  FILE *pFile;
  char line[90];
  char *tok;
  char *newline;
  cfg->noFile_exe = 0;
  /* set configfile to null */
  memset(cfg,'\0',sizeof(Configfile));
  /* open file */
  if((pFile =fopen(filename,"r"))==NULL)
  {
      fprintf(stderr,"Could not open '%s'.\n",filename);  
      exit(0);
  }

  /* readin line by line */
  while(fgets(line,sizeof(line),pFile)!=NULL)
  {
	 tok = strtok(line,DELIM);
	 toLowerCase(tok);
      while(tok!=NULL)
      {
        if(strchr(tok,'#')!=NULL || strcmp(tok,"\r\n") == SUCCESS ||
			  strcmp(tok,"") == SUCCESS) {
          break;
        }
        else if(strcmp(tok,"server_address") == SUCCESS ||
                strcmp(tok,"address") == SUCCESS) {
           tok = strtok(NULL, DELIM);
			  ckTabAndNewLine(tok);

			  if((newline = strstr(tok,"\r\n")) != NULL)
				   newline[0] = '\0';
           strcpy(cfg->address,tok);
           break;
        }
        else if(strcmp(tok,"server_name") == SUCCESS) {
           tok = strtok(NULL, DELIM);
           ckTabAndNewLine(tok);
			  strcpy(cfg->address,tok);
           break;
        }
        else if(strcmp(tok,"server_port") == SUCCESS ||
                strcmp(tok,"port") == SUCCESS ) {
           tok = strtok(NULL, DELIM);
			  ckTabAndNewLine(tok);
           cfg->port = atoi(tok);
           break;
        }
        else if(strcmp(tok,"print_message_details") == SUCCESS) {
           tok = strtok(NULL, DELIM);
           toLowerCase(tok);
           ckTabAndNewLine(tok);
			  if(strcmp(tok,"on") == SUCCESS) {
            cfg->print_message_details = TRUE;
            break;
           }
           else if(strcmp(tok,"off") == SUCCESS) {
            cfg->print_message_details = FALSE;
            break;
           }
           
            fprintf(stderr,"please use 'ON' or 'OFF' ");
            fprintf(stderr,"for 'print_message_details'\n");
            break;
        }
        else if(strcmp(tok,"request_count") == SUCCESS) {
           tok = strtok(NULL, DELIM);
			  ckTabAndNewLine(tok);
           cfg->request_count = atoi(tok);
           break;
        }
        else if(strcmp(tok,"request_timeout") == SUCCESS) {
           tok = strtok(NULL, DELIM);
           cfg->request_timeout = atoi(tok);
           break;
        }
        else if(strcmp(tok,"support_timeout") == SUCCESS) {
           tok = strtok(NULL, DELIM);
           toLowerCase(tok);
			  ckTabAndNewLine(tok);
           
			  if(strcmp(tok,"on") == SUCCESS) {
            cfg->support_timeout = TRUE;
            break;
           }
           else if(strcmp(tok,"off") == SUCCESS) {
            cfg->support_timeout = FALSE;
            break;
           }
           
            fprintf(stderr,"please use 'ON' or 'OFF' ");
            fprintf(stderr,"for 'support_timeout'\n");
            break;
        }
        else if(strstr(tok,"type") != NULL) {
            strcpy(cfg->file_exe[cfg->noFile_exe][0],tok);
            tok = strtok(NULL, DELIM);
            ckTabAndNewLine(tok);
            strcpy(cfg->file_exe[cfg->noFile_exe][1],tok);
            cfg->noFile_exe++;
            break;
        }
        else if(strcmp(tok,"host") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->host, tok);
          break;
        }
        else if(strcmp(tok, "root") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->root, tok);
          break;
        }
        else if(strcmp(tok, "shutdown-signal") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          cfg->shutdown_signal = atoi(tok);
          break;
        }
        else if(strcmp(tok, "shutdown-request") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->shutdown_request, tok);
          break;
        }
        else if(strcmp(tok, "shutdown-file") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->shutdown_file, tok);
          break;
        }
        else if(strcmp(tok, "status-request") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->status_request, tok);
          break;
        }
        else if(strcmp(tok, "logfile") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          memcpy(cfg->logfile, tok,strlen(tok));
          break;
        }
        else if(strcmp(tok, "logging") == SUCCESS) {
          toLowerCase(tok);
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);

          if(memcmp(tok,"yes",YES_LEN) == SUCCESS) {
            cfg->logging = TRUE;
          break;
          }
          else if(memcmp(tok,"no",NO_LEN)  == SUCCESS) {
            cfg->logging = FALSE;
          break;
          }
        }
        else if(strcmp(tok, "recordfile") == SUCCESS) {
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);
          strcpy(cfg->recordfile, tok);
          break;
        }
        else if(strcmp(tok, "recording") == SUCCESS) {
          toLowerCase(tok);
          tok = strtok(NULL, DELIM);
          ckTabAndNewLine(tok);

          if(strcmp(tok,"yes") == SUCCESS) {
            cfg->recording = TRUE;
          break;
          }
          else if(strcmp(tok,"no") == SUCCESS) {
            cfg->recording = FALSE;
          break;
          }
        }
        break; 
      }
  }  
  fclose(pFile);
}

