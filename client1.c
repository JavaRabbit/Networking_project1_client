#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>


int main(int argc, char * argv[]){
  printf("hello\n");

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;


  portno = 50050;

  sockfd = socket(AF_INET, SOCK_STREAM,0);

  if(sockfd < 0){

   perror("Errro");
   exit(1);
  }
  return 0;

}
