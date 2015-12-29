/***********************
 * Program Filename:  otp_enc.c
 * Author:  Brian Stamm
 * Date:  8.14.15
 * Description:  This program connects to its sister program and encodes a string
 * using a key.  It connects to its sister through a socket.  This program also checks
 * to make sure the string contains the correct characters and length.  If not, it exits.
 * Notes:  None.
 * Citation:  Most of the citation is done in otp_enc_d.c, but the code for the sockets
 * was taken from the lectures and example code provided.  Also, much help was gathered
 * from Canvas posts.
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

/************
 * Function:  validator
 * Variables:  an int
 * Outcome:  It checks the int passed it and compares it to known ASCII values;
 * if the int is not in the right range, returns a -1, otherwise, returns a 1.
 * *********/
int validator(int x){
   if(x < 65){				//Value for "A", if less than, bad char
      if((x == 10) || (x == 32)){	//Either new line or space is fine
	 return 1;
      }
      return -1;
   }
   else if(x > 90){			//Value for "Z", if greater than, bad
      return -1;
   }
   else{				//Or its between A and Z, good.
      return 1;
   }
}

//Main.
int main(int argc, char **argv){

   //initial variables
   FILE *file1, *file2;
   struct stat fStat1, fStat2;
   int fSize1, fSize2;
   char string1[15];
   char string2[15];
   char header[10];
   
   //basic error checking to ensure correct # of variables passed in
   if(argc < 4){
      error("Syntax is wrong.  Boo.", 1);
   }
   //Passes in args 1 and 2 to strings (which should be files)
   strcpy(string1, argv[1]);
   strcpy(string2, argv[2]);
   
   //Opens those files.
   file1 = fopen(string1, "r");
   if(file1 == NULL){
      error("Error opening file.\n", 1);
   }
   
   file2 = fopen(string2, "r");
   if(file2 == NULL){
      error("Error opening file.\n", 1);
   }
   
   //Then gets the "stats" for each file, does error checking
   if(stat(string1, &fStat1) < 0){
      error("Stat error\n", 1);
   }

   if(stat(string2, &fStat2) < 0){
      error("Stat error\n", 1);
   }
   //Gets the size of each file.
   fSize1 = fStat1.st_size;
   fSize2 = fStat2.st_size;

   //The key file has to be larger than the encoding file.  If not, then error.
   if(fSize1 > fSize2){
      error("Key not big enough.\n", 1);
   }
   
   //Moves into creating strings stored in the files
   int v, b, s = 0;
   char stringOne[fSize1];
   b = fgetc(file1);
   while(b != EOF){
      v = validator(b);			//Makes sure the encoding file has good chars
      if(v == -1){
	 error("Bad characters.\n", 1);
      }
      stringOne[s] = b;
      b = fgetc(file1);
      s++;
   }
   
   //Does same for second file, no validation because coming from keygen program
   //and cannot have a bad char from that program.
   s = 0;
   char stringTwo[fSize2];
   b = fgetc(file2);
   while(b != EOF){
      stringTwo[s] = b;
      b = fgetc(file2);
      s++;
   }
   //Creates a header of certain size, if file size larger, then just cuts off end.
   //Sister file only connects with program sending @ in header.  # seperates values
   snprintf(header, 10, "@#%d#######", fSize1);

   //Initial variables for socket...
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
      
   portno = atoi(argv[3]);	//Port number, assed in via command line
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0){
      error("Socket error\n", 2);	//Exit value 2, specified by instructions.
   }
   server = gethostbyname("localhost");	//Programs have to run in same location
   if (server == NULL){
      error("Host problems.\n", 1);
   }

   //Zeros out serv_addr and initializes values, from lecture
   memset((char*)&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);  	//Sets port number
   //Connects to sister program.
   if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
      error("Error in connecting, client.\n", 1);
   }
   //Then writes header to sister program.
   int b2write = strlen(header);
   int bWritten = 0;
   while(bWritten != b2write){		//Keeps track of bytes written, to ensure all
      int current;			//get sent.
      do{
	 current = write(sockfd, header, strlen(header));	//Does writing.
      }while((current == -1) && (errno == EINTR));

     if(current == -1){			//Error checking.
	error("Error in writing, 1\n", 1);
     }
     bWritten += current; 		//Updates written amount
   }
   
   //Two more writes, writing the strings passed in.  Same set up as previous write
   //but size to write is not predetermined   
   b2write = strlen(stringOne);
   bWritten = 0;
   int b2w2 = strlen(stringTwo);
   int bWritten2 =0;
   while(bWritten != b2write){
      int current;
      do{
	 current = write(sockfd, stringOne, strlen(stringOne));
      }while((current == -1) && (errno == EINTR));

     if(current == -1){
	error("Error in writing, 1\n", 1);
     }
     bWritten += current; 
   }
   
   //Second file written...
   while(bWritten2 != b2w2){
      int cur2;
      do{
	 cur2 = write(sockfd, stringTwo, strlen(stringTwo));
      }while((cur2 == -1) && (errno == EINTR));

     if(cur2 == -1){
	error("Error in writing, 1\n", 1);
     }
     bWritten2 += cur2; 
   }
   
   //Reads back encoded string from sister program.  Creates string and writes to it
   //Same set up as previous, but 'read''s instead of write
   char finalSet[fSize1];
   int b2read = fSize1;  
   int bRed = 0;
   while(bRed != b2read){	//Keeps track of bytes read
      int currentLoop;
      do{
	 currentLoop = read(sockfd, finalSet, fSize1);
      }while((currentLoop == -1) && (errno == EINTR));

     if(currentLoop == -1){
	error("Error in writing, 1\n", 1);
     }
     bRed += currentLoop; 
   }
   
   fprintf(stdout, "%s\n", finalSet);	//Prints out encoded string.

   close(sockfd);			//Closes socket.

   return 0;
}
