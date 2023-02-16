/* extra_credit.c --> special cases that should all pass ok
 * August 15, 2012
 * Jennifer Coy
 * Purpose:  practice working with files in C.
 */
#include <stdio.h>

#define STR_SIZE 256	/* size of a string */

int main(void)
{  char letter=' ';		// a single letter

   /* Here is a SPECIAL CASE: */
   /* Should not generate a syntax error */
   /* {([})}"' in a comment */
   // {([})}"' in a comment */

   /* Here is a SPECIAL CASE: */
   /* Should not generate a syntax error */
   printf("I like escape characters like \', \", \\. \n");
   printf("Also, I like to print {, [, (, }, ], and ).\n");
  
   /* Here is a SPECIAL CASE: */
   /* Should not generate a syntax error */
   if (letter == '\'') // a single quote stored in a letter
   {  // do nothing
   }
   else if (letter == '{')
   {  // do nothing
   }
 
   /* exit */
   return 0;
}

