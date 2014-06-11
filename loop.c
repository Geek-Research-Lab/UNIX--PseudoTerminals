#include<sys/types.h>
#include<signal.h>
#include "juice.h"
#define BUFFSIZE 512
static void sig_term(int);
static volatile sig_atomic_t sigcaught; /* set by signal handler */
void loop(int ptym, int ignoreeof)
{
pid_t child;
int nread;
char buff[BUFFSIZE];
if((child=fork()) < 0)
{
err_sys("error");
}
else if(child==0)
{
/* child copies stdin to ptym */
for(; ;)
{
if((nread=read(STDIN_FILENO, buff, BUFFSIZE)) < 0)
err_sys("error");
else if(nread==0)
break;
if(writen(ptym, buff, nread) != nread)
err_sys("error");
}
/* termination on encountering an EOF on stdin, also notification sent to parent only if ignoreeof is 0 */
if(ignoreeof==0)
kill(getppid(), SIGTERM); /* notify parent */
exit(0);
}
/* parent copies ptym to stdout */
if(signal_intr(SIGTERM, sig_term) == SIG_ERR)
err_sys("error");
for(; ;)
{
if((nread = read(ptym, buff, BUFFSIZE)) <= 0)
break; /* signal caught or error or EOF */
if(writen(STDOUT_FILENO, buff, nread) != nread)
err_sys("error");
}
if(sigcaught==0)
{
kill(child, SIGTERM);
return; /* parent returns to caller */
}
/* the child sends us SIGTERM when it receives an EOF on pty slave or encounters read() error */
static void sig_term(int signo)
{
sigcaught=1; /*set flag and return */
return; /*interrupt read() of ptym */
}
}
