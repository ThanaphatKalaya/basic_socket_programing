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
#include <openssl/md5.h>
 
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
      char input[1024] = "test.png";
      strcpy(sendBuff, input);
      printf("Sending %s\n",input);
      write(connfd, sendBuff, sizeof(sendBuff)-1);
      FILE *inputFile = fopen(input, "r");
      fseek(inputFile, 0, SEEK_END);
      long int fileSize = ftell(inputFile);
      printf("File size: %ld bytes\n",fileSize);
      sprintf(sendBuff, "%ld", fileSize);
      write(connfd, sendBuff, sizeof(sendBuff)-1);
      long int pointer,remain;
      unsigned char md5[MD5_DIGEST_LENGTH];
      MD5_CTX ctx;
      MD5_Init(&ctx);
      fseek(inputFile, 0, SEEK_SET);
      int bytesRead;
      if (fileSize < 1024){
        bytesRead = fread(sendBuff, 1, fileSize, inputFile);
        MD5_Update(&ctx, sendBuff, fileSize);
      }else{
        bytesRead = fread(sendBuff, 1, 1024, inputFile);
        MD5_Update(&ctx, sendBuff, 1024);
      }
      while(!feof(inputFile)){
        send(connfd, sendBuff, bytesRead, 0);
        pointer = ftell(inputFile);
        remain = fileSize - pointer;
        if(remain==0){
          bytesRead = fread(sendBuff, 1, 1, inputFile);
          MD5_Final(md5, &ctx);
        }else if(remain<1024){
          bytesRead = fread(sendBuff, 1, remain, inputFile);
          MD5_Update(&ctx, sendBuff, remain);
        }else{
          bytesRead = fread(sendBuff, 1, 1024, inputFile);
          MD5_Update(&ctx, sendBuff, 1024);
        }
      }
      fclose(inputFile);
      char result_md5[32] = "";
      char temp[2] = "";
      for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(temp, "%02x", md5[i]);
        strcat(result_md5,temp);
      }
      write(connfd, result_md5, 32);
      close(connfd);
      printf("md5[sv]: %s\n",result_md5);
      printf("Send finished\n");
      sleep(1);
    }

  return 0;
}
