/****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "inputHandling.h"
#include "syscallsWrapper.h"
#include "signalHandling.h"

#define MAX_LINE_LENGTH 256

/* begin prototypes */
void printHelp(void);
/* end prototypes */

int main(int argc, char *argv[])
{
	/* register all signal handlers */
	if(Signal(SIGCHLD, sigchldHandler) == SIG_ERR)
		unixError("Signal(SIGCHLD) fialed");
	if(Signal(SIGINT, sigintHandler) == SIG_ERR)
		unixError("Signal(SIGINT) failed");
	if(Signal(SIGTSTP, sigtstpHandler) == SIG_ERR)
		unixError("Signal(SIGTSTP)");
	/* end register all signal handlers */
	if(argc == 2){
		if(strcmp(argv[1], "-h") == 0)
			printHelp();
		else{
			printf("Wrong arguments\n");
			printf("Run again with -h if you need help or no arguments\n");
			return 1;
		}
	}
	setpgid(0,0);//Setting the group id of this process to be its own pid
	char userLine[MAX_LINE_LENGTH];
	while(1)
	{
		printf("$ ");
		if(fgets(userLine, MAX_LINE_LENGTH, stdin) == NULL)
			continue;
		parseInput(userLine);
	}
	return 0;
}


