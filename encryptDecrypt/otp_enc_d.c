/***************************
 * Program Filename:  otp_enc_d.c
 * Author:  Brian Stamm
 * Date:  8.14.15
 * Description:  This program is designed to run in the background.  When its
 * sister program connects to it via the correct port, it accepts 3 strings and 
 * writes one back.  The first string is to confirm the correct program is connecting
 * to it and the size of the other files.  If it is not the right program, it ends.  
 * If it is, it takes the size to create a 3 strings that size.  It accepts the strings, 
 * and it uses those strings to create an encoded string.  It then writes back that
 * encoded string.
 * Notes:  None, really.
 * Citation:  Alot.  I used the book alot, and I used the code supplied in lecture /
 * Canvas to build off of (liek the server & client programs).  I also reviewed 
 * Canvas postings, especially from 
 * codereview.stackexchange.com/questions/43914/client-server-implementation-in-c
 * -sending-data-files
 * ***********************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DELIMITER " #"

char KEY[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
   'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
   'Y', 'Z', ' '}; 

/*****
 * Function:  error
 * Variables:  a string and an int
 * Outcome:  It will print out the message to perror and exit
 * with the int value passed to it.
 * *****/
void error(const char *msg, int x){
   perror(msg);
   exit(x);
}

/*******
 * Function:  parse
 * Variables:  a string
 * Outcome:  It is used to parse out the header file sent first by the sister
 * program.  If it sends the correct validation character, it will return the
 * length of the files.  If it is the wrong validation character, it returns -1
 * ******/
int parse(char *str){
   char *word;
   word = strtok(str, DELIMITER);
   if(strcmp(word, "@") == 0){		//Validation character is @
      word = strtok(NULL, DELIMITER);	//DELIM. characters are space and #
      return (atoi(word));
   }
   else{
      return (-1);
   }
}

/************
 * Function:  encoder
 * Variables:  2 strings
 * Outcome:  It takes the two strings pasted to it.  It creates a third of the
 * same size, and it uses the One-Time Pads method to encode the message.  It
 * returns an encoded string.
 * ********/
char *encoder(char *str, char *kStr){
   int a, b, c, d, mod;
   int master = 27;
   char *rStr = malloc(strlen(str) *sizeof(char));
   int s = 0;
   while(str[s] != 0){
      a = str[s];
      c = kStr[s];
      if(a == 10 || c == 10){
	 break;
      }
      d = 0;
      b = 0;
      while(a != KEY[b]){
	 b++;
      }
      while(c != KEY[d]){
	 d++;
      }
      mod = (b + d) % master;
      rStr[s] = KEY[mod];
      s++;
   }
   return rStr;
}

//Main.
int main(int argc, char** argv){

   //Creates the initial variables 
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   struct sockaddr_in sAdd, cAdd;
   int w, size;

   //Checks to make sure user passes a port #, basic error checking.
   if(argc < 2){
      error("Error.  Need port.\n", 1);
   }
   
   //Creates a socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0){
      error("Error opening socket.\n", 1);
   }
   //Initializing
   memset((char*)&sAdd, 0, sizeof(sAdd));  	//fills string sAdd with 0's
   portno = atoi(argv[1]);			//Gets port number
   sAdd.sin_family = AF_INET;			//Sets values for sAdd
   sAdd.sin_addr.s_addr = INADDR_ANY;		//Accepts anyone who comes knocking
   sAdd.sin_port = htons(portno);		//Sets port
   //Sets up bind
   if (bind(sockfd, (struct sockaddr *) &sAdd, sizeof(sAdd)) < 0){
      error("Error in binding\n", 1);
   }
   //Starts listening, can take up to 5 calls at one time.
   listen(sockfd, 5);
   clilen = sizeof(cAdd);  	//Client socket
   
   //While loop for when accepting client sockets
   while(1){
      int status;		//For forking later.
      //Creates the link between client and server
      newsockfd = accept(sockfd, (struct sockaddr *) &cAdd, &clilen);
      if (newsockfd < 0){
	 error("Error on accepting\n", 1);
      }
      
      //Then forks off a process to do the work
      pid_t childPID = fork();
      if(childPID < 0){
	 error("PID ERROR!\n", 1);
      }
      
      //In child process
      if(childPID == 0){
	 close(sockfd);
	 //Creates variables for first read, gets header information to use in
	 //next reads.
	 char buffer[10];		//Predetermined size of header.
	 int b2r = 9;
	 int br = 0;
	 while(br != b2r){		//Makes sure reads total amount of bytes
	    int firC;
	    do{
	       firC = read(newsockfd, buffer, 9);
	    }while((firC == -1) && (errno == EINTR));
	    if(firC == -1){
	       error("Error in somteh.\n", 1);
	    }
	    br += firC;
	 }
	 size = parse(buffer);	//Sends string to parse func, returns size of files 
	 if(size == -1){		//If not right header, breaks off.
	    printf("Cannot connect.\n");
	 }
	 else{				//If it is, then continues to do more reading.
	    //creates variables for other reads.
	    char oneString[size];
	    char twoString[size];
	    int b2read = size;
	    int bRed = 0;
	    int b2r2 = b2read;
	    int bRed2 = 0;
	    
	    //Does a similar read as other, but uses the size passed in by sister prog
	    while(bRed != b2read){
	       int current;
	       do{
		  current = read(newsockfd, oneString, size);
	       }while((current == -1) && (errno == EINTR));
	       if(current == -1){
		  error("Error in reading, 2\n", 1);
	       }
	       bRed += current;
	    }

	    //Another read, gets second file.
	    while(bRed2 != b2r2){
	       int cur2;
	       do{
		  cur2 = read(newsockfd, twoString, size);
	       }while((cur2 == -1) && (errno == EINTR));
	       if(cur2 == -1){
		  error("Error in reading, 3\n", 1);
	       }
	       bRed2 += cur2;
	    }
	    //Encodes final string
	    char *finalString = encoder(oneString, twoString);
	    char deuxFinal[size];
	    strcpy(deuxFinal, finalString);	//Was having issues sending finalString
	    int b2write = size;			//so trouble shot, and this worked.
	    int bWritten = 0;
	    //Writes back to client, similar set up as read
	    while(bWritten != b2write){
	       int cLoop;
	       do{
		  cLoop = write(newsockfd, deuxFinal, b2write);
	       }while((cLoop == -1) && (errno == EINTR));
	       if(cLoop == -1){
		  error("In writting server.\n", 1);
	       }
	       bWritten += cLoop;
	    }
	 }
	 exit(0);	//Ends process.
      }
      else{
	 //Parent process, makes sure child finishes and no zombies.
	 pid_t endedPID = waitpid(childPID, &status, 0);
	 if(endedPID == -1){
	    error("Error with endPID", 1);
	 }
	 close(newsockfd);
      }
   }
   return 0;
}
