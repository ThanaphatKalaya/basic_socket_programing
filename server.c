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
      char input[1024] = "test.pdf";
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
      FILE *inputFile = fopen("test.pdf", "r");
      int bytesRead = fread(sendBuff, 1, 1, inputFile);
      while(!feof(inputFile))
      {
        send(connfd, sendBuff, bytesRead, 0);
        bytesRead = fread(sendBuff, 1, 1, inputFile);
      }
      fclose(inputFile);
      close(connfd);
      sleep(1);
    }

  return 0;
}
