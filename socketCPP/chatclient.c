/***********************
 * Program Filename:  chatclient.c
 * Author:  Brian Stamm
 * Date:  11.1.15  
 * Notes:  Refer to README.txt for any notes.
 * *********************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

/**********
 * Function:  error
 * Variables:  string and int
 * Outcome:  It prints the string as an error message (perror), and passes
 * the int to exit (setting the exit status).
 * ********/
void error(const char *msg, int x){
   perror(msg);
   exit(x);
}

/**********
 * Function:  toSend
 * Variables:  socketFile, array of chars, and its size
 * Outcomes:  It connects to the socket and sends the array and its size
 * through the socket.
 * ********/
void toSend(int socket, char message[], int size){
   int bytes = 0;

   while(bytes != size){
      int current;
      current = send(socket, message, size, 0);
      if(current == -1){
	 error("Error in writing.\n", 6);
      }
      bytes += current;
   }
}

/***************
 * Function:  toReceive
 * Variables:  socketFile, array of chars, and its size
 * Outcomes:  It connects to the socket and fills the array up to
 * its size.  
 * ************/ 
void toReceive(int socket, char* message, int size){
   int bytes = 0;

   while(bytes != size){
      int current;
      current = recv(socket, message, size, 0);
      if(current == -1){
	 error("Error in reading.\n", 7);
      }
      bytes += current;
   }
}

/***************
 * Function:  reset
 * Variables:  array of chars and its size
 * Outcome:  It fills the array with \0's, clearing
 * out its old data.
 * ************/
void reset(char* message, int size){
  int i; 
   for(i = 0; i< size; i++){
      message[i] = '\0';
   }
}

/**************
 * Function:  checker
 * Variables:  two array of chars
 * Outcome:  It takes the first 5 char of the message char
 * array and compares it to the truth array.  If they are equal,
 * it returns 1, if not, returns 0.
 * ***********/
int checker(char* message, char* truth){
   char checker[5];
   int i;
   for(i = 0; i<5; i++ ){
      checker[i] = message[i];
   }
   for(i = 0; i<5; i++){
      if(checker[i] != truth[i]){
	 return 0;
      }
   }
   return 1;
}

/*****************
 * Function:  stripper
 * Variables:  array of chars and its size
 * Outcome:  It goes through the array of chars and strips off
 * the newline character, replacing it with an end character.
 * **************/
void stripper(char* message, int size){
   int i;
   for(i = 0; i<size; i++){
      if(message[i] == '\n'){
	 message[i] = '\0';
      }
   }
}

//Main.
int main(int argc, char **argv){
   
   char messageSend[500];	//Array for message to be sent
   char messageRec[500];	//Array for message to be received
   int truth = 1;		//Used in the loop when a connection is made
   char myName[10];		//Used to get user name
   char yourName[10];		//Used for host name
   char ender[5] = {'\\', 'q', 'u', 'i', 't'};   //used in checker function
   int flip;
   
   //Program needs port number and flip number
   if(argc < 3){
      error("Usage wrong:  client [flip #] [port#].", 2);
   }
   
   printf("User name: ");
   fgets(myName, 10, stdin);
   stripper(myName, 10);

   //Initial variables for socket...
   int sockfd, portno;
   struct sockaddr_in serv_addr;
   struct hostent *server;
      
   portno = atoi(argv[2]);	//Port number, gotten via command line
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0){
      error("Socket error\n", 3);	//Exit value 2
   }
   
   //Gets server from commandline
   flip = atoi(argv[1]);
   if(flip == 1){
      server = gethostbyname("flip1.engr.oregonstate.edu");
   }
   else if(flip == 2){
      server = gethostbyname("flip2.engr.oregonstate.edu");
   }
   else if(flip == 3){
      server = gethostbyname("flip3.engr.oregonstate.edu");
   }
   else{
      error("Error with Flip number\n", 4);
   }

   if (server == NULL){
      error("Host problems.\n", 5);
   }

   //Zeros out serv_addr and initializes values
   memset((char*)&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);  	//Sets port number
   
   //Establishes a connection to program.
   if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
      error("Error in connecting, client.\n", 6);
   }
   
   //Sends and receives names to program connected to
   toSend(sockfd, myName, sizeof(myName));
   toReceive(sockfd, yourName, sizeof(yourName));

   //Takes off the \n at the end of the names
   stripper(myName, 10);
   stripper(yourName, 10);
   
   //Loop allowing chat
   while(truth){
      //First gets user's message
      printf("%s:  ", myName);
      fflush(stdout);
      fgets(messageSend, 500, stdin);
      
      //Checks if it's the \quit command, and ends loop
      if(checker(messageSend, ender)){
	 toSend(sockfd, messageSend, sizeof(messageSend));
	 truth = 0;
      }
      else{
	 //Sends and receives
	 toSend(sockfd, messageSend, sizeof(messageSend));
	 
	 toReceive(sockfd, messageRec, sizeof(messageRec));
	 if(checker(messageRec, ender)){
	    truth = 0;
	 }
	 printf("%s:  %s", yourName, messageRec);
	 reset(messageSend, 500);		//Resets the values to be filled
	 reset(messageRec, 500);
      }
   }

   //Ends connection
   close(sockfd);
   return 0;
}

