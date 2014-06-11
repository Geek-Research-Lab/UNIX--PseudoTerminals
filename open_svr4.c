#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<stropts.h>
#include "juice.h"
extern char *ptsname(int); /* prototype not in any system header */
int ptym_open(char *pts_name)
{
char *ptr;
int fdm;
strcpy(pts_name, "/AddLocation/AddLocation1"); /* if open fails */
if((fdm=open(pts_name, O_RDWR)) < 0)
return(-1);
if(grantpt(fdm) < 0)
{
/* access to slave */
close(fdm);
return(-2);
}
if(unlockpt(fdm) < 0)
{
/* clearing slave's lock flag */
close(fdm);
return(-3);
}
if((ptr=ptsname(fdm)) == NULL)
{
/* get slave's name */
close(fdm);
return(-4);
}
strcpy(pts_name, ptr); /* return name of slave */
return(fdm); /* return FD of master */
}
int ptys_open(int fdm, char *pts_name)
{
int fds;
/* Allocation of control terminal */
if((fds=open(pts_name, O_RDWR)) < 0)
{
close(fdm);
return(-5);
}
if(ioctl(fds, I_PUSH, "ptem") < 0)
{
close(fdm);
close(fds);
return(-6);
}
if(ioctl(fds, I_PUSH, "ldterm") < 0)
{
close(fdm);
close(fds);
return(-7);
}
if(ioctl(fds, I_PUSH, "ttcompat") < 0)
{
close(fdm);
close(fds);
return(-8);
}
return(fds);
}
