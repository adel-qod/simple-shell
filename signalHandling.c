/*
****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "signalHandling.h"
#include "processHandling.h"
#include "syscallsWrapper.h"
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

extern int processReturnStatus;

void sigchldHandler(int signum)
{
	//printf("We caugt a singal\n");
	assert(signum == SIGCHLD);
	pid_t pid;
	while((pid = waitpid(-1, &processReturnStatus, WNOHANG | WUNTRACED)) > 0){	
		if(WIFEXITED(processReturnStatus))
			removeFromJobList(pid);
		else if(WIFSIGNALED(processReturnStatus)){
			printf("The offending signal is %d\n", WTERMSIG(processReturnStatus));//this is not signal safe..you should use write instead
			removeFromJobList(pid);
		}
		if(WIFSTOPPED(processReturnStatus)){
			kill(0, SIGCONT);//Just to make sure myShell is not paused()
			int index = findProcessIndex(pid);
			jobList[index].state = ST;	
		}
	}
}

void sigtstpHandler(int signum)
{
	assert(signum == SIGTSTP);
	pid_t pid = findForegroundProcessID();
	if(pid != -1)
		kill(-pid, SIGTSTP);
}

void sigintHandler(int signum)
{
	assert(signum == SIGINT);
	int pid = findForegroundProcessID();
	if(pid != -1)
		kill(-pid, SIGINT);
}
