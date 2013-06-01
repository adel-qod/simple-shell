/*****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "syscallsWrapper.h"
#include "processHandling.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* begin prototypes */
static inline bool isBackGround(char **argv);
static void addToJobList(pid_t pid, int state);
/* end prototypes */

/* begin global variables */
int processReturnStatus;//Filled on calls to wait and waitpid
/* end global variables */

/* begin executeProcess */
int executeProcess(char *path, char **argv)
{
	int status = 0;
	pid_t pid;
	int state = FG;
	/* Check here if path is a file */
	bool isBg = isBackGround(argv);
	if(isBg == true)
		state = BG;
	
	if((pid = Fork()) < 0)
		return COULDNT_CREATE_PROCESS;
	if(pid == 0)
	{//Child context
		if(setpgid(0, 0) < 0)//Setting group ID as the PID
			unixError("setpgid failed");
		/* now we gotta remove all blocked signals before exec*/
		sigset_t *new = NULL;
		sigfillset(new);/* new has all signals as blocked now*/
		if((sigprocmask(SIG_UNBLOCK, new, NULL)) < 0)
			unixError("sigprocmask failed"); 
		if(execvp(path, argv) < 0){
			//Due to the failer, myShell will keep at pause() although the process never execed 
			//Thus we will send myShell a SIGCONT to get it out of the pause()
			
			sleep(1);
			kill(0, SIGCONT);
			unixError("exec failed");
		}
	}//end of child context
	addToJobList(pid, state);
	if(isBg == false){
		waitpid(pid, &status, 0);
		removeFromJobList(pid);
	}
	return PROCESS_CREATED;
}
/* end executeProcess*/

/* begin addToJobList */
static void addToJobList(pid_t pid, int state)
{
	int i = 0;
	for(; i < MAX_JOBS; i++)
		if(jobList[i].jid == 0)
			break;
	if(i < MAX_JOBS){
		jobList[i].pid = pid;
		jobList[i].jid = (i+1) * 10;
		jobList[i].state = state;	
	}
	else{//we can't accept more jobs and thus need to kill this one
		fprintf(stderr, "Shell can't have more jobs, try again");
		fprintf(stderr, "when some other jobs finish\n");
		kill(-pid, SIGKILL);//Kill all processes in group abs(pid)
	}	
}
/* end addToJobList */

/* begin removeFromList */
void removeFromJobList(pid_t pid)
{
	int i = 0;
	for(; i < MAX_JOBS; i++)
		if(jobList[i].pid == pid)
			break;
	if(i < MAX_JOBS){
		jobList[i].pid = 0;
		jobList[i].jid = 0;
		jobList[i].state = 0;
	}
}
/* end removeFromList */

/* begin isBackGround */
static inline bool isBackGround(char **argv)
{
	char **argPtr = argv;

	while(*(argPtr+1) != NULL)
	argPtr++;
	if(strcmp(*argPtr, "&") == 0){
		*argPtr = NULL;
		return true;
	}
	return false;	
}
/* end isBackGround */

/* begin printJobList */
void printJobList(void)
{
	printf("Process ID: \t      Job ID:    \t Process State: \n");
	printf("********************************************************\n\n");
	for(int i = 0; i < MAX_JOBS; i++)
		if(jobList[i].jid != 0){
			printf("%d\t\t\t", jobList[i].pid);
			printf("%d\t\t\t", jobList[i].jid);
			printf("%d\n", jobList[i].state);
		}
}
/* end printJobList */

/* begin findProcessID */
pid_t findProcessID(int jid)
{
	pid_t pid = -1;
	for(int i = 0; i < MAX_JOBS; i++)
		if(jobList[i].jid == jid){
			pid = jobList[i].pid;
			break;
		}
	return pid;//if we didn't find a match, we return -1 as an error
}
/* end findProcessID */

/* begin findProcessIndex */
//Returns the index of the process in the jobList
int findProcessIndex(pid_t pid)
{
	int index = -1;
	for(int i = 0; i < MAX_JOBS; i++)
		if(jobList[i].pid == pid){
			return i;
		}
	return index;//return -1 if no match is found
}
/* end findProcessInded */

/* begin sendJobToForeground */
void sendJobToForeground(int jid)
{
	pid_t pid;
	if((pid = findProcessID(jid)) < 0){
		fprintf(stderr, "JID you entered doesn't exist\n");
		return;
	}
	if(kill(-pid, SIGCONT)<0){//Send the signal to the whole group
		fprintf(stderr, "Couldn't continue the job\n");
		return;
	}
	jobList[(jid/10)-1].state = FG;//jid starts at 1 so this is safe
	pause();//magic, don't remove the second pause
	pause();	
}
/* end sendJobToForeground */

/* begin sendJobToBackground */
void sendJobToBackground(int jid)
{
	pid_t pid;
	if((pid = findProcessID(jid)) < 0){
		fprintf(stderr, "JID you entered doesn't exist\n");
		return;
	}
	if(kill(-pid, SIGCONT)<0){//Send the signal to the whole group
		fprintf(stderr, "Couldn't continue the job\n");
		return;
	}
	jobList[(jid/10)-1].state = BG;//jid starts at 1 so this is safe
	//there's no wait in this function which means the shell 
	//will resume work and run the job in the background
}
/* end sendJobToBackground */

/* begin sendProcessToForeground */
void sendProcessToForeground(pid_t pid)
{
	int index;
	if((index = findProcessIndex(pid)) < 0){
		fprintf(stderr, "PID you entered doesn't exist\n");
		return;
	}
	if(kill(-pid, SIGCONT) <0 ){
		fprintf(stderr, "Couldn't continue the process\n");
		return;
	}
	jobList[index].state = FG;
	pause();
	pause();//again, don't remove this one, this is pure magic, we need two pauses
}
/* end sendProcessToForeground */

/* begin sendProcessToBackground */
void sendProcessToBackground(pid_t pid)
{
        int index;
        if((index = findProcessIndex(pid)) < 0){
                fprintf(stderr, "PID you entered doesn't exist\n");
                return;
        }
        if(kill(-pid, SIGCONT)<0){
        	fprintf(stderr, "Couldn't continue the process\n"); 
	       return;
	}
        jobList[index].state = BG;      
        /* No waiting here as process will run in the background */
}
/* end sendProcessToBackground */

/* begin findForegroundProcessID */
//Return -1 on error, the index of the process currently running on FG
pid_t findForegroundProcessID(void)
{
	for(int i = 0; i < MAX_JOBS; i++)
		if(jobList[i].state == FG)
			return jobList[i].pid;
	return -1;
}
/* end getForegroundProcessID */
