

#ifndef _SYSCALLSWRAPPER_H_
#define _SYSCALLSWRAPPER_H_
#include <sys/types.h>
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
pid_t Fork(void);
void unixError(const char *msg);

#endif
