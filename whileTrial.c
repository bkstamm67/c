#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
   int i;

   printf("First while, <=.\n");

   i =0;
   while(i<= 5){
      printf("i:  %d\n", i);
      i++;
   }

   printf("Again.  just <");
   i=0;
   while(i<5){
      printf("i:  %d\n", i);
      i++;
   }

   printf("And done.\n");

}
