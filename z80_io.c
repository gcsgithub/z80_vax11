#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>

static int  TT=-1;


#define getcon(x)               read(TT,x,1)
#define putcon(x)               write(TT,x,1)

struct  itimerval       ITIMSTART;
struct  itimerval       ITIMPREV;
//struct  termios tsave, tset;

char    cmdbuf[1024];   /* input for parser */
int     cmdptr;

void init()
{
int err;
        if ( (TT = open("z80$term",O_RDWR|O_NONBLOCK) ) == -1 ) {
                err=errno;
                //fprintf(stderr, "error opening terminal:%s\n", strerror(err));
                fprintf(stderr, "error opening terminal:%d\n", err);
                exit(err);
        }
//        tcgetattr(TT, &tsave);
//        bcopy(&tsave,&tset,sizeof(tset));
 //       tset.c_lflag &= ~ISIG;
  //      tset.c_lflag &= ~ICANON;
   //     tset.c_lflag &= ~ECHO;
    //    tcsetattr(TT,TCSANOW, &tset);

     //   bzero(cmdbuf, sizeof(cmdbuf));
}



int z80_in(int port)
{
        int c;
        if (TT<0) init();
        getcon(&c);
        return(c);
	//return('A');
}

int z80_out(int val, int port)
{
  char c;
        if (TT<0) init();

      c = val;
       putcon(&c);
       return(0);
}
