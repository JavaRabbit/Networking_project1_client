#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char * argv[]){
  printf("hello\n");

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;


  //portno = atoi(argv[2]);
  portno = 4000; // 50050

  char buffer[256];

  sockfd=socket(AF_INET, SOCK_STREAM,0);
  bzero(&serv_addr, sizeof serv_addr);

  if(sockfd < 0){
   perror("Errro");
   exit(1);
  }


  server = gethostbyname(argv[1]);
  if(server == NULL){
    fprintf(stderr, "no such host\n");
    exit(1);
  }

  serv_addr.sin_family = AF_INET;
  
  serv_addr.sin_port = htons(portno);

  inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr));
  
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0){
   perror("error conecting\n");
  } 
  printf("Please enter message:");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);
  n = write(sockfd, buffer, strlen(buffer));
  
  if(n < 0){
    perror("error reading from socket\n");
  }

  printf("%s\n", buffer);
  close(sockfd);
 




  return 0;

}
