

#ifndef SHELL_SYSCALLSWRAPPER_H
#define SHELL_SYSCALLSWRAPPER_H
#include <sys/types.h>
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
pid_t Fork(void);
void unixError(const char *msg);

#endif
