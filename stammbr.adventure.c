/*********************
 * Program Filename:  stammbr.adventure.c
 * Author:  Brian Stamm
 * Date:  7.10.15
 * Description:  Based off of the assignment description, this is a simple
 * adventure game, which has 7 connected rooms, one of which is a start room 
 * and one is an end room.  The user has to get to the end room.  The program
 * creates the rooms and writes them to files.  It also keeps track of the 
 * path of the user and reports it back at the completion of the game.  
 * 
 * Notes:  None really.  Not optimized as much as possible, but fits within
 * the requirements.
 * Citation:  Myself, my brain, Canvas discussion board, stacks.  The Canvas
 * discussion board really helped, especially when thinking about the randomization
 * of the connections to the rooms.
 * *****************/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Structure room, used for creating the rooms
struct room {
   char* name;
   int c;			//Total connections, used later for determining total connects made
   int co;			//Connections made
   struct room* connections[6];
   char* rmTp;
};

//Array of the room types, order is important for random assignment later
char* roomType[] = {"MID_ROOM\n", "START_ROOM\n", "END_ROOM\n"};

//Room List, used to create the room for game, order is unimportant
char *roomList[] = {"Vulcan\n", "Solaria\n", "Aurora\n", "Gethen\n", "Arraki\n", "Kobol\n", 
   "Tatooine\n", "Magrathea\n", "Pluto\n", "Gallifrey\n"};


/*******************
 * Function:  _fileMaker
 * Variables:  an array of room structs, its size and the PID
 * Outcome:  It creates a directory, writes files within said directory,
 * and writes the information about the rooms to the various files.  Sets
 * room 1 and 2 as start and end rooms, respectively.
 * ***************/
void _fileMaker(struct room** dungeon, int size, int pid){
   int i, j, k, m;
   char direct[18];	//String for the directory name
   snprintf(direct, 18, "stammbr.room.%d", pid);  //Creates the string

   if(mkdir(direct, 0777) == -1){	//Creates the actual directory, with full permissions
      printf("Directory Error.\n");	//If it doens't work, error
      exit(1);
   }
   m = 3;				//Used for numbering rooms, except start and end
   for(i = 0; i < size; i++){		//Goes through the array of structs
      int filedescriptor;
      struct room* temp = dungeon[i];	//Assigns the struct to temp
      if(temp->rmTp == "MID_ROOM\n"){	//If it's type if a 'mid room', then creates and numbers
	 int w = temp->c - temp->co;		//the file based on the m varriable
	 char file[25];			//file string is used to write the file name
	 snprintf(file, 25, "%s/room%d", direct, m);	//Creates the string, included the directory name
	 filedescriptor = open(file, O_WRONLY|O_CREAT, 0644);	//Creates the file
	 if(filedescriptor == -1){
	    printf("Goo\n");
	 }
	 FILE *f = fopen(file, "w");			//Then writes values to file, per assignment
	 fprintf(f, "ROOM NAME: %s", temp->name);
	 j=1;
	 for(k = 0; k < w; k++){			//For loop to go through array in struct.  W was
	    fprintf(f, "CONNECTION %d: %s", j, temp->connections[k]->name);  //Created above, subtracts array
	    j++;					//total from items in to get the final value in array
	 }
	 fprintf(f, "ROOM TYPE: %s", temp->rmTp);
	 fclose(f);
	 m++;
      }
      else if(temp->rmTp == "START_ROOM\n"){	//Same as previous block, but start room is always room1
	 char file[25];				//Done for ease of programming
	 snprintf(file, 25, "%s/room1", direct);
	 filedescriptor = open(file, O_WRONLY|O_CREAT, 0644);
	 if(filedescriptor == -1){
	    printf("Goo\n");
	 }
	 int w = temp->c - temp->co;	
	 FILE *f = fopen(file, "w");
	 fprintf(f, "ROOM NAME: %s", temp->name);
	 j=1;
	 for(k = 0; k < w; k++){
	    fprintf(f, "CONNECTION %d: %s", j, temp->connections[k]->name);
	    j++;
	 }
	 fprintf(f, "ROOM TYPE: %s", temp->rmTp);
	 fclose(f);
      }
      else{
	 char file[25];				//Last block, same as other two, but this would have to be
	 snprintf(file, 25, "%s/room2", direct);	//end room.  Always room2, again for ease.
	 filedescriptor = open(file, O_WRONLY|O_CREAT, 0644);
	 if(filedescriptor == -1){
	    printf("Goo\n");
	 }
	 int w = temp->c - temp->co;
	 FILE *f = fopen(file, "w");
	 fprintf(f, "ROOM NAME: %s", temp->name);
	 j=1;
	 for(k = 0; k < w; k++){
	    fprintf(f, "CONNECTION %d: %s", j, temp->connections[k]->name);
	    j++;
	 }
	 fprintf(f, "ROOM TYPE: %s", temp->rmTp);
	 fclose(f);
      }
   }
}

/*******************
 * Function:  _typeAssign
 * Variables:  An array of struct rooms and it's size.  Also needs the Global array
 * of roomType as well.
 * Outcome:  What it does is randomly assign a roomType to each member of the array 
 * of struct rooms, but only 1 start and end room.
 * ***************/
void _typeAssign(struct room** dungeon, int size){
   int i;
   int total = 0;  //Used to see if end or start room has been assigned
   int room;
   for(i = 0; i < size; i++){		//For loop, to go thru array of struct rooms
      if(total == 0 && i < 5){		//Checks total value and if still less than two rooms
	 room = rand() % 3;		//left to assign, gets random number btw 0 and 2
	 total += room;			//Adds the value to total
	 dungeon[i]->rmTp = roomType[room];	//Assigns the value to the room
      }
      else if(total == 0 && i == 5){	//If only mid rooms been assign and only two rooms left
	 room = rand() % 2;		//then randomly assigns either start or end room
	 if(room == 0){
	    dungeon[i]->rmTp = roomType[1];
	    total += 1;
	 }
	 else{
	    dungeon[i]->rmTp = roomType[2];
	    total += 2;
	 }
      }
      else if(total == 1 && i < 6){	//If start room has been assigned and still have more than
	 room = rand() % 2;		//two rooms to assign, then random between end and mid room
	 if(room == 0){
	    dungeon[i]->rmTp = roomType[0];
	 }
	 else{
	    dungeon[i]->rmTp = roomType[2];
	    total += 2;
	 }
      }
      else if(total == 1 && i == 6){	//If only one left to assign and start has, but not end.  End
	 dungeon[i]->rmTp = roomType[2];	//is then assigned
      }
      else if(total == 2 && i < 6){	//Same set up as previous, but flip start and end room
	 room = rand() % 2;
	 if(room == 0){
	    dungeon[i]->rmTp = roomType[0];
	 }
	 else{
	    dungeon[i]->rmTp = roomType[1];
	    total += 1;
	 }
      }
      else if(total == 2 && i == 6){
	 dungeon[i]->rmTp = roomType[1];
      }
      else if(total == 3){		//Last condition, if both start and end room have already been
	 dungeon[i]->rmTp = roomType[0];	//assigned, no matter how many rooms are left
      }
   }
}

/*******************
 * Function:  _connectionTime
 * Variables:  Takes an array of struct rooms.
 * Outcome:  Randomly determines the number of connections and then
 * connects each room.
 * ***************/
void _connectionTime(struct room** dungeon){
   int i = 0;
   int j;
   int error = 1;

   //Used to determine the number of random connections.  Since game has 7 rooms, at least 3 connections
   //per with a max of 6, that means at min have 11 connections and at most 22
   int total = rand() % 11 + 11;	
   
   while(i < total){
      int x = rand() % 7;	//Gets random number btw 0 and 6 (used for position in array)
      int y = x;		//Assigns same value and then does loop to get different number
      while (y == x){
	 y = rand() % 7;
      }
      struct room* tempX = dungeon[x];	//Creates temp pointers to pointer stored in array at that value
      struct room* tempY = dungeon[y];
      if(tempX->co != 0 && tempY->co != 0){	//Checks how many connections are available for both
	 char* yName = tempY->name;		//selected, if not zero, goes through one of the struct's
	 for(j = 0; j<6; j++){			//connect arrays to see if they are already connected
	    struct room* tempZ = tempX->connections[j];
	    if( tempZ != NULL){
	       char* xName = tempZ->name;
	       if(xName == yName){
		  error = -1;			//Error value, used if connected already
		  break;			//Breaks out of the loop
	       }
	    }
	 }
	 if(error != -1){			//If not connected, connects both, using the array
	    x = tempX->c - tempX->co;		//position calculated by minusing c from co
	    y = tempY->c - tempY->co;
	    tempX->connections[x] = tempY;
	    tempY->connections[y] = tempX;
	    tempX->co--;			//Subtracts possible connections
	    tempY->co--;
	    i++;				//Increases i
	 }
	 else{					//Otherwise, it was already connected and resets the 
	    error = 1;				//error value.
	 }
      }
   }
   //This is a check to make sure every room has at least 3 connections
   //If total is greater than 17, then all the room must have at least 3 connections at min.
   int w, k;
   if(total < 17){
      for(i = 0; i < 7; i++){		//Goes through the array
	 j = 0;
	 error = 1;
	 while(dungeon[i]->co >= 4){	//If it has 2 or less connections, then goes back through
	    struct room* tempI = dungeon[i];	//and assigns it to another room
	    if( j == i){		//if j and i are equal, then it's the same room
	       j++;
	    }
	    else if( j != i){		//Then runs through same check as above and assigns
	       if(dungeon[j]->co != 0){
		  int s = tempI->c;
		  s -= tempI->co;
		  for(w =0; w < s; w++){
		     if(tempI->connections[w]->name == dungeon[j]->name){
			error = -1;
			break;
		     }
		  }
		  if(error != -1){
		     struct room* tempK = dungeon[j];
		     k = tempK->c - tempK->co;
		     tempI->connections[s] = tempK;
		     tempK->connections[k] = tempI;
		     tempI->co--;
		     tempK->co--;
		     j++;
		     }
		  else{
		     error = 1;
		     j++;
		  }
	       }
	       else{
		  j++;
	       }
	    }
	 }
	 }
      }
}

/*******************
 * Function:  _randomRoom
 * Variables:  Takes an array of numbers and its size.
 * Outcome:  Fills the array with random numbers, each being unique
 * between 0 and 9.
 * ***************/ 
void _randomRoom(int arr[], int size){
   int error = -1;
   int w = 0;
   while (w < size){		//While loop
      int i = rand() % 10;	//Ranom number btw 0 and 9
      int b = 0;
      while (b <= w){
	 if(arr[b] != i)	//Makes sure number hasn't been assigned
	    b += 1;		//yet
	 else{
	    error = 1;		//If so, then assigns new error number and breaks out of loop
	    break;
	 }
      }
      if (error == -1){		//If error value is the same as first, then writes value into array
	 arr[w] = i;
	 w +=1;
      }
      else
	 error = -1;
   }
}

/*******************
 * Function:  _Shiva
 * Variables:  Takes an array of struct rooms
 * Outcome:  Destroy!
 * ***************/
void _Shiva(struct room **dungeon, int size){
   int i;
   for(i = 0; i <size; i++){
      free(dungeon[i]);
   }
}

/***********************
 * Function: _structInitializer
 * Variables: an array of struct rooms, array off numbers, size of the arrays (both habe 
 * to be the same size), and the max number of connections between the rooms
 * Outcome: It creates pointers of struct rooms and initializes their values.
 * ********************/
void _structInitializer(struct room **dungeon, int aRAssign[], int size, int max){
   int n = 0;

   while (n < size){
      struct room* newRoom = malloc(sizeof(struct room));
      assert(newRoom);

      int r = aRAssign[n];	//used random number stored in array to get name of room
      int c = max;		//Max connections
      newRoom->name = roomList[r];
      newRoom->c = c;
      newRoom->co = c;
      dungeon[n] = newRoom;
      int i;
      for(i=0; i<6; i++){
	 newRoom->connections[i] = NULL;	//Sets all connections to null
      }
      n++;
   }
}

/*******************
 * Function:  gameStart
 * Variables:  takes an array of struct rooms, its size, the max number of connections
 * and the PID (for file naming reasons).
 * Outcome:  Creates an array to hold the random numbers used to select the room names,
 * creates the struct of rooms, placing them in the struct room array, randomize the connections,
 * and randomly assigns a room Type.  Once structs are completed, then writes each one to a file
 * and then deletes it.
 * ***************/
void gameStart(int size, int maxConnection, int pid){
   
   int aRAssign[size];
   struct room* dungeon[size];
   _randomRoom(aRAssign, size);
   _structInitializer(dungeon, aRAssign, size, maxConnection);
   _connectionTime(dungeon);
   _typeAssign(dungeon, size);
   _fileMaker(dungeon, size, pid);
   _Shiva(dungeon, size);

}
/*******************
 * Function:  instructions
 * Variables:  n/a
 * Outcome:  Prints basic instructions to the screen.
 * ***************/
void instruction(){
   printf("This is a game.  You will have fun.  Be sure to enter the\n");
   printf("name of the room exactly as printed.  Enjoy.\n\n");
}

int main(int argc, char* argv[]){
   
   srand(time(NULL));			//Seeds random
   int pid = getpid();			//Gets the PID
   gameStart(7, 6, pid);		//Passes values to write files
   char path[100][20];			//Max amount of rooms one can check.

   instruction();			//Prints instructions
   
   char direct[18];
   snprintf(direct, 18, "stammbr.room.%d", pid); 	//Directory name stored
   int i, j, m;

   char compareArray[7][20];		//Creates an array of the room names used to compare
   i=1;					//user inputs.  'i' is used for room numbers
   while(i<8){
      char file[25];
      snprintf(file, 25, "%s/room%d", direct, i);
      FILE *f = fopen(file, "r");	//opens the file
      char wasteE1[20];
      char wasteE2[20];
      char compareRoom[20];		//Uses two strings, so can put "\n" on to compare
      char anotherRoom[20];
      fscanf(f, "%s %s %s", wasteE1, wasteE2, compareRoom); //Gets first line written, done
      snprintf(anotherRoom, 20, "%s\n", compareRoom);
      strcpy(compareArray[i-1], anotherRoom);
      fclose(f);
      i++;
   }
   
   char file[25];				//string of file name
   snprintf(file, 25, "%s/room1", direct);	//Know first room is start room, so get information
   FILE *f = fopen(file, "r");
   char waste1[20];
   char waste2[20];
   char keep[20];
   char fileArray[8][20];		//An array of strings, for name & all connections
   m=0;
   while(42){				//Meaning of life is truth
      int ret = fscanf(f, "%s %s %s", waste1, waste2, keep);	//Gets important information
      if(ret == EOF){			//if end of file gotten to, breaks
	 break;
      }
      else{
	 strcpy(fileArray[m], keep);	//Else, puts the value / string in the array
	 m++;
      }
   }
   fclose(f);

   char currentRoom[20];			//Used for while loop
   char endRoom[20];
   strcpy(currentRoom, compareArray[0]); 
   strcpy(endRoom, compareArray[1]);
   //printf("Start room: %s\n", currentRoom);	//Used for checking, not deleted in case of doing run throughs
   //printf("End room: %s\n", endRoom);

   int journey = 0;			//Counter to see how many steps
   char connectArray[6][20];
   int n = 0;
   while(strcmp(currentRoom, endRoom) !=0){
      if (journey == 99){
	 printf("Wow.  That's been a journey for you.\n");
	 printf("Let's call it quits for now.\n'");
	 break;
      }
      printf("CURRENT LOCTION: %s\n", fileArray[0]);	//Prints current location
      printf("POSSIBLE CONNECTIONS: ");			//for loop to print connections
      
      for(i = 1; i < m-1; i++){
	 snprintf(connectArray[n], 20, "%s\n", fileArray[i]);
	 n++;
	 if( i == (m - 2)){				//Last connection, prints period at end
	    printf("%s.\n", fileArray[i]);		//M is total line numbers
	 }
	 else{
	    printf("%s, ", fileArray[i]);		//Otherwise, prints connection w comma
	 }
      }
      int error = 1;
      while(error == 1){
	 char *compare;					//Creates string variable for comparision
	 size_t MAXSIZE = 25;
	 size_t userRoom;
	 compare = (char*)malloc(MAXSIZE * sizeof(char));
	 if( compare == NULL){
	    printf("Goo.");
	 } 
	 
	 printf("WHERE TO?> ");				//Prompts and saves value from user
	 userRoom = getline(&compare, &MAXSIZE, stdin);	
	 for(j = 0; j < n; j++){	
	    char temp[20];
	    strcpy(temp, connectArray[j]);		//Compares value to connections
	    if(strcmp(temp, compare) == 0){		//If comparison works
	       strcpy(path[journey], temp); 		//Stores value into the path
	       journey++;				//Increment journey
	       strcpy(currentRoom, temp);		//Changes currentRoom to the user selected room
	       error = 0;				//Reset some defaults
	       n=0;		
	       j=0;
	       for(i = 0; i<7; i++){			//Used to find the room number, so can open correct file
		  if(strcmp(temp, compareArray[i]) == 0){
		     j = i;
		     break;
		  }
	       }
	       char fileloop[25];			//Gets room document
	       snprintf(fileloop, 25, "%s/room%d", direct, (j+1));
	       FILE *f = fopen(fileloop, "r");		//Gets all values from file and stores in fileArray
	       char wasteloop1[20];
	       char wasteloop2[20];
	       char keep[20];
	       m=0;
	       while(42){
		  int ret = fscanf(f, "%s %s %s", wasteloop1, wasteloop2, keep);
		  if(ret == EOF){
		    break; 
		  }
		  else{
		     strcpy(fileArray[m], keep);
		     m++;
		  }
	       }
	       fclose(f);
	       break;
	    }
	 }
	 printf("\n");					//If it doesn't match, then prints this, doesn't increment
	 if(error == 1){				//and stays in loop, no values change.
	    printf("HUH?  I DON'T UNDERSTAND THAT ROOM.  TRY AGAIN.\n\n");
	 }
	 free(compare);
	 }
      }
   
   //End is reached, congrats.   
   printf("YOU HAVE FOUND THE END ROOM.  CONGRATULATIONS!\n");
   printf("YOU TOOK %d STEPS.  YOUR PATH TO VICTORY WAS:\n", journey);		//Prints out step amount
   
   for(i=0; i < journey; i++){							//And the array of the journey
      printf("%s", path[i]);
   }
 
   return 0;
}
