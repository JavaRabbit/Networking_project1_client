#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>


/*  method prototypes */
void getUserHandle();
void chat();
void startUpConnection(int, char* []);

/*  variables */
char userHandle[11];

/* Variables to store socket connection information  */
struct sockaddr_in serv_addr;
int sockfd, portno;
struct hostent *server, *gethostbyname();


/* Method to get user handle */
void getUserHandle(){
  printf("Please enter your name:\n");
  fgets(userHandle, 10, stdin);
  
  // trim off new line character
  int newLineLocation = strlen(userHandle) - 1;
  userHandle[newLineLocation] = '\0';
 
  // greet the user
  printf("Welcome to the chat client %s\n", userHandle);
}



/* Main method will make a call to get user handle
 * Then will set up socket connection with server
 * After connection has been made, main method will
 * call the chat method to start chat with server
 */

int main(int argc, char * argv[]){

  
  // verify the number of arguments to be 3
  if (argc !=3){
    printf("Usage example: ./client flip1.engr.oregonstate.edu <port number>\n");
    exit(1);
  }

  //  start up the connection to the server.
  startUpConnection(argc, argv);

  // after socket connection variables have been verified, ask user for handle
  getUserHandle();
  
  // initiate the chat between client and server
  chat();  

} // end main




/* startUpConnection will set up and validate the connection
 * to the server. It will check that the port number is valid
 * and the host is valid. 
 * When completed, it will return to the main method, where
 * the main method can then call the chat method
 */
void startUpConnection(int argc, char *argv[]){


  /* Save the port number from the command line 
   *  If the port number is not valid, show usage
   *  to user and exit 
   */
  portno = atoi(argv[2]);
  if( portno < 256 || portno > 65535){
    printf("Invalid port number. Please try again from 256- 65535.\n");
    printf("Usage example:   ./chatclient flip1.engr.oregonstate.edu  50050\n");
    exit(1);
  }



  sockfd=socket(AF_INET, SOCK_STREAM,0);
  bzero(&serv_addr, sizeof serv_addr);

  if(sockfd < 0){
   perror("Errro");
   exit(1);
  }

  
  /* Set the server name as specified by user on command line */   
  server = gethostbyname(argv[1]);
  if(server == NULL){
    fprintf(stderr, "No such host was found\n");
    exit(1);
  }
  
  // new line. see added gethostname hosttent
  bcopy(server->h_addr, &(serv_addr.sin_addr.s_addr), server->h_length);
  

  serv_addr.sin_family = AF_INET;
  
  serv_addr.sin_port = htons(portno);

  inet_pton(AF_INET, server, &(serv_addr.sin_addr));
  
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0){
   perror("error conecting. Port number on server may not be in use.\n");
   exit(1);
  } 
 

} // end of startUpMethod


/* Chat method:
 * The goal of this method is to run a while loop to 
 * get a message from the user
 * and send this message to the server.
 * In addition, this method will wait for the reply
 * from the server, and display the server reply to the user. 
 * 
 * Precondition:  the server connection is set up
 * The chat will end when either the server or this client
 * has entered "quit".
 */
void chat(){

  // variables to hold string for user message 
  // and server response
  char response[500];
  char buffer[500];

  /* While loop will run the send / receive cycle */
  while(true){
    printf("%s>", userHandle);
    bzero(buffer, 500);
    bzero(response, 500);
    fgets(buffer, 499, stdin);
    
    // Check  using string compare if user entered quit
    // if so, send message to the server, and exit this client.
    if(strcmp(buffer, "quit\n") ==0 ){
      // send a message to the server that you are closing
      write(sockfd, buffer, strlen(buffer));
      
      // close this connection, and exit this client application.
      close(sockfd);
      exit(0);
    }

    // Send the user message to the server.
    int n = write(sockfd, buffer, strlen(buffer));
   
    // Await server's response 
    recv(sockfd, response, 500, 0);

    // verify that no errors occured during socket reading
    if(n < 0){
      perror("error reading from socket\n");
    }

    // Check if server has decided to close connection.
    // Use strcmp using the server response for this check. 
    // If so,  close this connection, notify user, and exit this application
    if(strcmp(response, "quit") ==0){
      printf("Server decided to close connection\n");
      printf("Now closing this client application.\n");
      close(sockfd);
      exit(0);
    } 

    // Otherwise, server did not close the connection, thus
    // Display the servers response to the client
    printf("%s\n", response); // used to be buffer
  
  }
  close(sockfd);


}
