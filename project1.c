/*	
 *	project1.c
 * 	Tim Nicholas
 *	1-31-2023
 *	Purpose: To examine a file and validate it for compiling.
*/
#include <stdio.h>
#include <string.h>

#define STR_SIZE 256
#define DEBUG 0
#define EMPTY_STRING ""
#define APOSTROPHE '\''
#define QUOTATION '\"'
#define BACKSLASH '\\'
#define EMPTY_STACK_INDEX -1

/* Error Codes */
#define END_WITHOUT_EMPTY_STACK 0
#define MISSING_BEGINNING_BRACKET 1
#define MISSING_BEGINNING_PARENTHESE 2
#define MISSING_BEGINNING_SQ_BRACKET 3
#define UNEXPECTED_ENDING_BRACKET 4
#define UNEXPECTED_ENDING_PARENTHESE 5
#define UNEXPECTED_ENDING_SQ_BRACKET 6

void userPrompt(char *filename);
void compileFile(FILE *input, char *filename);
int verifyLine(const char buffer[], char stack[], int *top, int *long_comment_mode);
void stringCharCase(char stack[], int *top, char quoteMark);
void push(char stack[], int *top, char pushee);
void pop(char stack[], int *top);
void printError(char stack[], int *top, int error_code, int line_num);
char expectedCharacter(char input);

/* Main function, which doesn't take any command line arguments. */
int main(void) {
	char filename[STR_SIZE] = EMPTY_STRING;
	FILE *infile = NULL;
	
	/* Prompt the user for the file they want this application to open and compile. */
	userPrompt(filename);
	infile = fopen(filename, "r");
	
	/* If infile is null, then we know the file was misnamed or doesn't exist. */
	if(infile == NULL) {
		printf("Error opening file: %s\n", filename);
		return 1; // return with error
	}
	if(DEBUG) printf("%s has been found!\n", filename);
	
	/* Once the file has been verified to exist, start the compiling process.*/
	compileFile(infile, filename);
	fclose(infile);
	return 0;
}

/* Prompt the user for the name of a file they want to compile. */
void userPrompt(char *filename) {
	char buffer[STR_SIZE] = EMPTY_STRING;
	printf("Enter file name: ");
	fgets(buffer, sizeof(buffer), stdin);
	sscanf(buffer, "%s", filename);
}

/* Iterate through the given file line by line and verify it to be compiled or not. */
void compileFile(FILE *input, char *filename) {
	char buffer[STR_SIZE] = EMPTY_STRING;
	char stack[STR_SIZE] = EMPTY_STRING;
	int top = EMPTY_STACK_INDEX, line_num = 1, result = 0, long_comment_mode = 0;
	/* Iterate through the file line by line. */
	while(fgets(buffer, sizeof(buffer), input)) {
		/* Will return 1 if valid and an error code if the code failed to compile. */
		result = verifyLine(buffer, stack, &top, &long_comment_mode);
		if(result != 0) {
			if(DEBUG) printf("Throwing error.\n");
			/* If we fail to compile, then we print error and return to end the program. */
			printError(stack, &top, result, line_num);
			return;
		}
		if(DEBUG) printf("Line %d is valid.\n\n", line_num);
		line_num++;
	}
	/* If the top isn't at -1, throw an error because there was a character missing along the way. */
	if(top > EMPTY_STACK_INDEX) {
		if(DEBUG) printf("Stack is not empty!\n");
		printError(stack, &top, END_WITHOUT_EMPTY_STACK, line_num);
		return;
	}
	if(DEBUG) printf("The file was successfully verified and compiled!\n");
}

/* Iterate through each character in the given line from the file and verify its validity. */
int verifyLine(const char buffer[], char stack[STR_SIZE], int *top, int *long_comment_mode) {
	int i = 0, testResult = 0, earlyTermination = 0;
	char currentChar, topOfStack;
	if(DEBUG) printf("Current Line: %s\n", buffer);
	/* Iterate through each character in the line. */
	for(i = 0; i < strlen(buffer); i++) {
		currentChar = buffer[i];
		topOfStack = stack[*top];
		// If long comment mode is active and the next two characters aren't */, 
		// then move onto the next character.
		if(*long_comment_mode) {
			if(currentChar == '*' && i < strlen(buffer) - 1) {
				*long_comment_mode = buffer[i + 1] != '/';
				/* If long comment mode is no longer active, then skip the next character. */
				if(!*long_comment_mode) i++; 
			}
		}
		/* If not in long comment mode, then check for quote characters before anything else. */
		else if(currentChar == APOSTROPHE || currentChar == QUOTATION || currentChar == BACKSLASH) {
			stringCharCase(stack, top, currentChar);
		}
		/* If a backslash is on the top of the stack, pop it off and move on to the next char. */
		else if(topOfStack == BACKSLASH) {
			pop(stack, top);
		}
		/* Don't allow next step if the top of the stack contains a quote character. */
		else if(topOfStack != APOSTROPHE && topOfStack != QUOTATION) {
			switch(currentChar) {
				/* In case of /, if the top of the stack is also / then pop, otherwise push it onto the stack. */
				case '/':
					if(currentChar == topOfStack) {
						pop(stack, top);
						/* If previous 2 characters are "//", then ignore the rest of the line. */
						earlyTermination = buffer[i - 1] == buffer[i];
						break;
					}
					push(stack, top, currentChar);
					break;
				case '*':
					if(topOfStack == '/') {
						*long_comment_mode = 1;
						pop(stack, top);
					}
					break;
				/* If the current character is a starting character, push it onto the stack. */
				case '{': case '(': case '[':
					push(stack, top, currentChar);
					break;
				/* If the current character is a ending character, validate the stack before popping. */
				case '}':
					if(currentChar != expectedCharacter(topOfStack)) {
						if(DEBUG) printf("Error detected.\n");
						/* If the stack isn't valid, then give an error code based on the input. */
						testResult = (*top == EMPTY_STACK_INDEX) ? MISSING_BEGINNING_BRACKET : UNEXPECTED_ENDING_BRACKET;
					} else {
						pop(stack, top);
					}
					break;
				case ')':
					if(currentChar != expectedCharacter(topOfStack)) {
						if(DEBUG) printf("Error detected.\n");
						testResult = (*top == EMPTY_STACK_INDEX) ? MISSING_BEGINNING_PARENTHESE : UNEXPECTED_ENDING_PARENTHESE;
					} else {
						pop(stack, top);
					}
					break;
				case ']':
					if(currentChar != expectedCharacter(topOfStack)) {
						if(DEBUG) printf("Error detected.\n");
						testResult = (*top == EMPTY_STACK_INDEX) ? MISSING_BEGINNING_SQ_BRACKET : UNEXPECTED_ENDING_SQ_BRACKET;
					} else {
						pop(stack, top);
					}
					break;
			}
		}
		/* If the test result produces an error code, then we want to break the loop immediately. */
		if(testResult != 0 || earlyTermination) break;
	}
	/* Return the results of the verification process. 0 for false and 1 for true. */
	return testResult;
}

/* String characters are a special case. */
void stringCharCase(char stack[], int *top, char quoteMark) {
	/* If the top of the stack is the same or is a backslash, then pop it off. 
	   Otherwise, push the character onto the stack. */
	if(quoteMark == stack[*top] || stack[*top] == BACKSLASH) {
		pop(stack, top);
	} else {
		push(stack, top, quoteMark);
	}
}

/* Push the inputted character unto the stack and increment the top by 1. */
void push(char stack[], int *top, char pushee) {
	*top = *top + 1;
	stack[*top] = pushee;
	if (DEBUG) printf("Pushed %c onto the stack.\n", stack[*top]);
}

/* Pop the top character from the stack by decrementing top by 1. */
void pop(char stack[], int *top) {
	if (DEBUG) printf("Popped %c from the stack.\n", stack[*top]);
	*top = *top - 1;
}

/* Return the expecpted character to go with the input. */
char expectedCharacter(char input) {
	switch(input) {
		case '{': return '}';
		case '(': return ')';
		case '[': return ']';
		default:  return ' ';
	}
}

/* Take the line number and the error code and print the appropriate error with the line it occurred on. */
void printError(char stack[], int *top, int error_code, int line_num) {
	switch(error_code) {
		case END_WITHOUT_EMPTY_STACK:
			printf("ERROR: missing a }, ), or ], line EOF.\n");
			break;
		case MISSING_BEGINNING_BRACKET:
			printf("ERROR: missing { or extra }, line %d.\n", line_num);
			break;
		case MISSING_BEGINNING_PARENTHESE:
			printf("ERROR: missing ( or extra ), line %d.\n", line_num);
			break;
		case MISSING_BEGINNING_SQ_BRACKET:
			printf("ERROR: missing [ or extra ], line %d.\n", line_num);
			break;
		case UNEXPECTED_ENDING_BRACKET:
			printf("ERROR: found a }, but expected %c, line %d.\n", expectedCharacter(stack[*top]), line_num);
			break;
		case UNEXPECTED_ENDING_PARENTHESE:
			printf("ERROR: found a ), but expected %c, line %d.\n", expectedCharacter(stack[*top]), line_num);
			break;
		case UNEXPECTED_ENDING_SQ_BRACKET:
			printf("ERROR: found a ], but expected %c, line %d.\n", expectedCharacter(stack[*top]), line_num);
			break;
		default:
			printf("ERROR: Unknown code has possibly been executed!\n");
			break;
	}
}
