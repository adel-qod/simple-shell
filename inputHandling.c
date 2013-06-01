/*
****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* Include & defines section */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "processHandling.h"
#include "syscallsWrapper.h"
#include "stringUtils.h"
/* End include & defines */

/* Prototypes section */
static inline bool isShellCommand(const char *userInput);
static int executeCommand(char *userInput);
void printHelp(void);
static inline void handleFgCommand(char *userInput);
static inline void handleBgCommand(char *userInput);
/* End prototypes */




/* begin parseInput  */
/*Checks what the userInput is and calls appropriate functions  */
void parseInput(char *userInput)
{
	userInput[strlen(userInput)-1] = '\0';//Remove the \n that fgets read
	while(*userInput && isblank(*userInput))//Ignore leading blanks
		userInput++;

	if(executeCommand(userInput) == 0){
		return;
	}

	/*We'll create the argv now */
	char **argv = split(userInput, " \t");
	if(argv == NULL)
	{
		fprintf(stderr, "failed to allocate memory\n");
		return;
	}
	char *path = argv[0];
	int err = executeProcess(path, argv);

	switch (err){
	case PROCESS_CREATED:
		free(argv);
		break;
	case COULDNT_CREATE_PROCESS:
		fprintf(stderr, "Failed to create new processn\n");
		free(argv);
		break;
	default:
		free(argv);
	}
}
/* end parseInput */

/* begin isShellCommand */
/*Returns TRUE if userInput is a built-in shell command
	else: FALSE */
static inline bool isShellCommand(const char *userInput)
{
	/* According to the requirments, shell executes progams using
	their paths or built-in	commands; so if we don't have a path
	we simply return TRUE 
	Be careful that new requirments allow programs to be run if they're 
	in the PATH variable so paths might not be fully specified*/
	if(userInput[0] == '/'|| (userInput[0] == '.' && userInput[1] == '/'))
		return false;
	return true;
}
/* end isShellCommand */

/* begin printHelp */
void printHelp(void)
{
	printf("The commands you can enter are as follows:\n");
	printf(" fg <arguments> : continues a stopped process in Foreground\n\n");
	printf(" bg <arguments> : continues a stopped process in Background\n\n");
	printf(" jobs : prints the jobs running in the shell and their state\n");
	printf("\t status mean : 1 = Foreground ; 2 = Background; 3 = Stopped; and 0 = undefined state\n\n");
	printf(" clear : clears the shell screen \n\n");
	printf(" quit (or exit) : terminates the shell\n\n");
	printf("Process running: \n\n");
	printf(" You should specify a full path and press enter to run a job,\n\t add & to the end of the path");
	printf(" to run the job in the back ground\n\n");
	printf(" Pressing Ctrl + Z will stop the Foreground process and gives shell promt again\n\n");
	printf(" Pressing Ctrl + C will kill the Foreground process and gives shell promt again\n\n");
}
/* end printHelp */

/* begin executeCommand */
/* Executes the built-in shell commands
return 0 on success and -1 if it's not a command */
static int executeCommand(char *userInput)
{

	if(strcmp(userInput, "quit") == 0 || strcmp(userInput, "exit")== 0)
		exit(0);
	else if(strcmp(userInput, "clear") == 0)
		printf("\033[2J\033[1;1H");//Using ANSI escape charecters
		//J with parameter 2 to clear the entier screen, H with 1;1
		//to set cursor to top-left
	else if(strcmp(userInput, "jobs") == 0)
		printJobList();
	else if(strncmp(userInput, "fg", 2) == 0)
		handleFgCommand(userInput);
	else if(strncmp(userInput, "bg", 2) == 0)
		handleBgCommand(userInput);
	else if(strncmp(userInput, "help", 4) == 0)
		printHelp();
	else if(strcmp(userInput, "") == 0)
		return 0;
	else
		return -1;	
	return 0;
}
/* ends executeCommand */

/* begin handleFgCommand */
static inline void handleFgCommand(char *userInput)
{
	userInput += 2;//move past the 'fg'
	while(*userInput && isblank(*userInput))
		userInput++;//ignore in-between blanks
	if(!isgraph(*userInput)){//If it's not followed by an argument
		fprintf(stderr, "fg command need a JID or PID\n");
		fprintf(stderr, "JIDs: %%<JID> or\t PIDs: <PID>\n");
		return;
	}
	if(*userInput == '%'){//if the number is JID
		userInput++;
		sendJobToForeground(atoi(userInput));
		return;
	}
	sendProcessToForeground(atoi(userInput));
}
/* end handleFgCommand */

/* begin handleBgCommand */
static inline void handleBgCommand(char *userInput)
{
	userInput += 2;//move past the 'fg'
	while(*userInput && isblank(*userInput))
		userInput++;//ignore in-between blanks
	if(!isgraph(*userInput)){
		fprintf(stderr, "bg command need a JID or PID\n");
		fprintf(stderr, "JIDs: %%<JID> or\t PIDs: <PID>\n");
		return;
	}
	if(*userInput == '%'){//if the number is JID
		userInput++;
		sendJobToBackground(atoi(userInput));
		return;
	}
	sendProcessToBackground(atoi(userInput));
}
/* end handleBgCommand */



