#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>

int main(int argc, char * argv[]){

  // verify the number of arguments to be 3
  if (argc !=3){
    printf("Usage example: ./client flip1.engr.oregonstate.edu <port number>\n");
    exit(1);
  }  


  // a string to get user's handle
  char userHandle[11];
  printf("hello, enter your name:\n");
  fgets(userHandle, 10, stdin);

  // trim off new line character
  int newLineLocation = strlen(userHandle) - 1;
  userHandle[newLineLocation] = '\0';

  // say hi to user
  printf("hi %s\n", userHandle);
 

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server, *gethostbyname();
  struct in_addr ipv4addr;

  portno = atoi(argv[2]);
  // portno = 4300; // 50050

  // spec: program should be able to send message up to 500 chars
  char buffer[500];

  sockfd=socket(AF_INET, SOCK_STREAM,0);
  bzero(&serv_addr, sizeof serv_addr);

  if(sockfd < 0){
   perror("Errro");
   exit(1);
  }

  
  //server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
   
  server = gethostbyname(argv[1]);
  if(server == NULL){
    fprintf(stderr, "No such host was found\n");
    exit(1);
  }
  
  // new line. see added gethostname hosttent
  bcopy(server->h_addr, &(serv_addr.sin_addr.s_addr), server->h_length);
  

  serv_addr.sin_family = AF_INET;
  
  serv_addr.sin_port = htons(portno);
  //serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  //  new line

  // middle argument used to be "127.0.0.1"
  inet_pton(AF_INET, server, &(serv_addr.sin_addr));
  
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0){
   perror("error conecting. Port number on server may not be in use.\n");
   exit(1);
  } 


  char response[245];

  while(true){
    printf("Please enter message:");
    bzero(buffer, 500);
    bzero(response, 245);
    fgets(buffer, 499, stdin);
    
    // here is where we check if user entered quit
    if(strcmp(buffer, "quit\n") ==0 ){
      printf("you wanted to quit\n");
      // send a message here to the server that you are closing
      write(sockfd, buffer, strlen(buffer));
      close(sockfd);
      exit(0);
    }


    n = write(sockfd, buffer, strlen(buffer));
    printf("gets here %d\n");

 
    recv(sockfd, response, 44,0);

    if(n < 0){
      perror("error reading from socket\n");
    }

    // check if server said to quit. if so. close this sockfd, tell user, and exit
    if(strcmp(response, "quit") ==0){
      printf("server decided to close connection\n");
      printf("good bye\n");
      close(sockfd);
      exit(0);
    } 

    printf("%lu %s\n", strlen(response), response); // used to be buffer
  
  }
  close(sockfd);

  return 0;

}
