/*****************************COPYRIGHTS AND LICENSE****************************

Copyright (c) 2013, Mhd Adel Al Qodamni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _PROCESSHANDLING_H_
#define _PROCESSHANDLING_H_

#include <sys/types.h>//For the pid_t

/* begin prototypes section */
int executeProcess(char *path, char **argv);
void removeFromJobList(pid_t pid);
void printJobList(void);
pid_t findProcessID(int jid);/* Finds process ID of a job; -1 on error */
int findProcessIndex(pid_t pid);/* Finds process index in jobList, -1 error */
void sendJobToForeground(int jid);
void sendJobToBackground(int jid);
void sendProcessToForeground(pid_t pid);
void sendProcessToBackground(pid_t pid);
pid_t findForegroundProcessID(void);
/* end prototypes section*/

/* begin executeProcess return values */
#define PROCESS_CREATED 0x00001000
#define COULDNT_CREATE_PROCESS 0x00002000
/* end executeProcess return values */


/* public data structs */

/* begin job states */
/* a job can be either FG(foreground, at most one can be fg)
                        BG (background) using & or 
                        ST stopped(using Ctlr+Z)
                        or UD for undefined 
Transition table:
By default a process starts fg unless given &

BG -> FG : fg <job_id> 
FG -> ST : Ctrl+Z
ST -> BG : bg <job_id>
ST -> FG : fg <job_id>
*/
#define FG 1
#define BG 2
#define ST 3
#define UD 0
/* end job states */


struct job_t
{
        pid_t pid;//Process id of the job
        int jid;
        int state;//BG, FG, ST or UD
};

#define MAX_JOBS 500
struct job_t jobList[MAX_JOBS];//Standard says this's initilized to 0s
/* End public data structs */
#endif
