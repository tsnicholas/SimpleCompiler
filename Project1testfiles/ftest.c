// test2.c --> no errors
// August 15, 2012
// Jennifer Coy
// Purpose:  practice working with files in C.

#include <stdio.h>

#define STR_SIZE 256	// size of a string 

int main(void)
{  int i=0;			// loop counter 
   char filename[STR_SIZE]="";	// filename to open 
   FILE *infile=NULL;		// file pointer 
   char buffer[STR_SIZE]="";	// string used to process file 
 
   // prompt the user for a filename 
   printf("Enter name of file to open: ");
   fgets(buffer, sizeof(buffer), stdin);
  
   // clean up the input, since c stores the enter key 
   sscanf(buffer, "%s", filename);  

   // open file, for reading 
   infile = fopen(filename, "r");
   if (infile == NULL)
   {  printf("Error opening file: %s\n", filename);
      return 1;  
   }

   // read through file, counting lines 
   while(fgets(buffer, sizeof(buffer), infile))
   {  // count this line 
      i++;  
      // print this line, with line number 
      printf("%d: %s", i, buffer); 
   }  

   // print the number of lines in the file 
   printf("=======\nThis file has %d lines.\n=======\n", i);

   // close file and exit 
   fclose(infile);
   return 0;
}

