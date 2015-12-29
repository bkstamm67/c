/**************************
 * Program Filename:  keygen.c
 * Author:  Brian Stamm
 * Date:  8.14.15
 * Description:  This will create a key for assignment 4.  User selects a length,
 * and the program randomly generates a key that length + 1 (end character).
 * Notes:  None, really.
 * Citation:  None really for this program.  Fairly straight forward.
 * ***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
   
   srand(time(NULL));		//Used for random numbers
   int total, i, j;
   
   //Array of characters
   char *key[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", 
   "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", 
   "W", "X", "Y", "Z", " "};
     
   char str[20];		//String used to get the user number

   strcpy(str, argv[1]);	//Copies user argument into string

   total = atoi(str);		//Changes string to int
   char* output[total+1];	//Creates string, to be filled

   for(i = 0; i < total; i++){	//For loop fills string with random characters
      j = rand() % 27;
      output[i] = key[j];
   }

   output[total] = "\n";	//Adds newline at end of character

   //For loop to print out random string to stdout
   for(i = 0; i < total+1; i++){
      fprintf(stdout, "%s", output[i]);
   }

   return 0;
}
