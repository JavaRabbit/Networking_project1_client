/**********************************
 * Project: Networking project 1
 * Program: Chat client using sockets
 * Language:  C
 * Author: Bonnie Kwong
 * Description:  A chat client that can communicate with a server using 
 * network sockets.
 * Date: May 7, 2017
 * ***********************************/



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


/* Function to get user handle */
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
 * the main function can then call the chat method
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
  
  // sets all values in buffer to zero
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
  
  // set struct serv_addr to symbolic constant AF_INET
  serv_addr.sin_family = AF_INET;
  
  // set the struct serv_addr port number
  serv_addr.sin_port = htons(portno);

  inet_pton(AF_INET, server, &(serv_addr.sin_addr));
  
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0){
   perror("error conecting. Port number on server may not be in use.\n");
   exit(1);
  } 
 

} // end of startUpMethod


/* Chat function:
 * The goal of this function is to run a senc/recv cycle in a  while loop to 
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
  char response[500]={};
  char buffer[500]={};

  /* While loop will run the send / receive cycle */
  while(true){
    printf("%s>", userHandle);
    bzero(buffer, 500);
    bzero(response, 500);
    fgets(buffer, 500, stdin);
    
    // Check  using string compare if user entered quit
    // if so, send message to the server, and exit this client.
    if(strcmp(buffer, "quit\n") ==0 ){
      // send a message to the server that you are closing
      write(sockfd, buffer, strlen(buffer));
      
      // close this connection, and exit this client application.
      // print message to user 
      printf("\nYou indicated that you want to quit this client.");
      printf("\nNow closing this chat client application. Good bye\n");
      close(sockfd);
      exit(0);
    }

    // variable to contain full string to be sent to server
    // This appends the "user handle" to be sent to the server
    
    char buffer1[500]={};
    char buffer2[1001] = {};
    char fullBuffer[2100] ={}; 
    bzero(fullBuffer,2100);
    bzero(buffer1, 500);
    bzero(buffer2, 1001);   

    char greaterThan[] = {"> "};
    //strcpy(buffer1, userHandle);
    //strcpy(buffer2, buffer1);
    //strcat(buffer2, greaterThan);
    //strcat(fullBuffer, buffer2);
    //strcat(fullBuffer, buffer);
    sprintf(fullBuffer, "%s %s %s", userHandle, greaterThan, buffer);
    
    // Send the user message to the server.
    int n = write(sockfd, fullBuffer, strlen(buffer));
   
    // Await server's response 
    recv(sockfd, response, 500, 0);

    // verify that no errors occured during socket reading
    if(n < 0){
      perror("error reading from socket\n");
    }

    // initialize substring to check if server wanted to quit  
    char substring[5];

    // Check if server has decided to close connection.
    // Use strcmp using the server response for this check. 
    // If so,  close this connection, notify user, and exit this application
    if(strlen(response) > 5){
      // get the substring from the server response
      int len = strlen(response)- 4;
      memcpy(substring, response+ len, 4);  // 4 one less than bb length
      //printf("\n%s\n",substring);
      // null terminate thesubstring
      substring[4] = '\0';
    } 
  
    // if the substring matches "quit", then we close connetion
    // as well as close this application
    if(strcmp(substring, "quit") ==0){
      printf("\nServer decided to close connection\n");
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
