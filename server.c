#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
 
int main(void)
{
  int listenfd = 0,connfd = 0;
  
  struct sockaddr_in serv_addr;
 
  char sendBuff[1024];  
  char recvBuff[1024];
 
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("socket retrieve success\n");
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff));
      
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  int port = 8081;
  serv_addr.sin_port = htons(port); 

  printf("listen to port %d\n",port);   
 
  bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
  
  if(listen(listenfd, 10) == -1){
      printf("Failed to listen\n");
      return -1;
  }

  while(1)
    {
      connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
      //read(connfd, recvBuff, sizeof(recvBuff)-1);
      //strcpy(sendBuff, recvBuff);
      char input[1024] = "ubuntu-16.04.3-desktop-amd64.iso";
      strcpy(sendBuff, input);
      printf("Sending %s\n",input);
      write(connfd, sendBuff, sizeof(sendBuff)-1);
      char md5[33];
      FILE *pipe;
      int len;
      char cmd[256] = "md5sum ";
      strcat(cmd,input);
      pipe = popen(cmd, "r");
      if (NULL == pipe) {
        perror("pipe");
        exit(1);
      }
      fgets(md5, 33, pipe);
      pclose(pipe);
      printf("md5: %s\n",md5);
      strcpy(sendBuff, md5);
      write(connfd, sendBuff, sizeof(sendBuff)-1);
      FILE *inputFile = fopen(input, "r");
      fseek(inputFile, 0, SEEK_END);
      long int fileSize = ftell(inputFile);
      long int pointer,remain;
      fseek(inputFile, 0, SEEK_SET);
      int bytesRead;
      if (fileSize < 1024){
        bytesRead = fread(sendBuff, 1, fileSize, inputFile);
      }else{
        bytesRead = fread(sendBuff, 1, 1024, inputFile);
      }
      while(!feof(inputFile)){
        send(connfd, sendBuff, bytesRead, 0);
        pointer = ftell(inputFile);
        remain = fileSize - pointer;
        if(remain==0){
          bytesRead = fread(sendBuff, 1, 1, inputFile);
        }else if(remain<1024){
          bytesRead = fread(sendBuff, 1, remain, inputFile);
        }else{
          bytesRead = fread(sendBuff, 1, 1024, inputFile);
        }
      }
      fclose(inputFile);
      close(connfd);
      printf("Send finished\n");
      sleep(1);
    }

  return 0;
}
