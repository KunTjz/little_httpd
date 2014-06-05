#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "sys_define.h"
#include "http_signal.h"
#include "http_process.h"
#include "http_connection.h"

// from apue p265
sigfunc signal(int signo, sigfunc sf)
{
	struct sigaction act, oact;
	
	act.sa_handler = sf;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if(signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	
	if(sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	
	return (oact.sa_handler);
}

int init_signals()
{
	if(signal(SIGINT, server_shutdown) == SIG_ERR) {
		printf("set SIGINT error\n");
		return L_HTTP_FAIL;
	}
	
	if(signal(SIGQUIT, server_shutdown) == SIG_ERR) {
		printf("set SIGQUIT error\n");
		return L_HTTP_FAIL;
	}
	
	if(signal(SIGCHLD, on_worker_breakdown) == SIG_ERR) {
		printf("set SIGCHLD error\n");
		return L_HTTP_FAIL;
	}

	return L_HTTP_SUCCESS;
}

