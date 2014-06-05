#ifndef __HTTP_SIGNAL_H__
#define __HTTP_SIGNAL_H__

typedef void (*sigfunc)(int);

sigfunc signal(int sig, sigfunc sf);

int init_signals();


#endif
