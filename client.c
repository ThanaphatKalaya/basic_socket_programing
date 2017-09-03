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
  int sockfd = 0;
  char recvBuff[1024] = "";
  char sendBuff[1024] = ""; 
  struct sockaddr_in serv_addr;

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
 
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8081);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
      printf("\n Error : Connect Failed \n");
      return 1;
    }

  //char massage[1024] = "";
  //printf("Get file from server : ");
  //fgets(massage,1024,stdin);
  //strcpy(sendBuff, massage);
  //write(sockfd, sendBuff, sizeof(sendBuff)-1);
  read(sockfd, recvBuff, sizeof(recvBuff)-1);
  printf("reciving %s\n",recvBuff);
  char outputPath[1024] = "output/";
  strcat(outputPath, recvBuff);
  printf("saving file to \"%s\"\n",outputPath);
  FILE *outputFile = fopen(outputPath, "w");
  read(sockfd, recvBuff, sizeof(recvBuff)-1);
  char md5[33];
  strcpy(md5, recvBuff);
  printf("md5[sv]: %s\n",md5);
  int bytesReceived = recv(sockfd, recvBuff, 10, 0);
  while(bytesReceived != 0)
    {
      fwrite(recvBuff, bytesReceived, 1, outputFile);
      bytesReceived = recv(sockfd, recvBuff, 10, 0);
    }
  fclose(outputFile);
  close(sockfd);
  
  char md5cl[33];
  FILE *pipe;
  int len;
  char cmd[256] = "md5sum ";
  strcat(cmd,outputPath);
  pipe = popen(cmd, "r");
  if (NULL == pipe) {
    perror("pipe");
    exit(1);
  }
  fgets(md5cl, 33, pipe);
  pclose(pipe);
  printf("md5[cl]: %s\n",md5cl);
  
  return 0;
}
