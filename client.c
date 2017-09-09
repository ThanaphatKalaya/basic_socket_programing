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
  read(sockfd, recvBuff, sizeof(recvBuff)-1);
  long int fileSize = atol(recvBuff);
  printf("File size: %ld bytes\n",fileSize);
  FILE *outputFile = fopen(outputPath, "w");
  long int bytesReceived = 0;
  long int totalReceived = 0;
  unsigned char md5[MD5_DIGEST_LENGTH];
  MD5_CTX ctx;
  MD5_Init(&ctx);
  while(totalReceived<fileSize)
  {
    bytesReceived = recv(sockfd, recvBuff, 1, 0);
    fwrite(recvBuff, bytesReceived, 1, outputFile);
    totalReceived += bytesReceived;
    MD5_Update(&ctx, recvBuff, 1);
    //printf("Reciving %ld/%ld\n",totalReceived,fileSize);
  }
  MD5_Final(md5, &ctx);
  fclose(outputFile);
  
  char md5sv[32];
  read(sockfd, recvBuff, sizeof(recvBuff)-1);
  strcpy(md5sv, recvBuff);
  md5sv[32] = '\0';
  close(sockfd);
  printf("md5[sv]: %s\n",md5sv);
  
  char result_md5[32] = "";
  char temp[2] = "";
  for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
    sprintf(temp, "%02x", md5[i]);
    strcat(result_md5,temp);
  }
  printf("md5[cl]: %s\n",result_md5);
  return 0;
}
