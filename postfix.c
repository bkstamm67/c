#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){

   int i;
   
   printf("Going to do ++i\n");

   for(i=0;i<5;++i){
      printf("i:  %d\n", i);
   }

   printf("\nNow doing it i++\n");

   for(i=0;i<5;i++){
      printf("i:  %d\n", i);
   }

   printf("And done.\n");
}
