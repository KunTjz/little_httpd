#ifndef __HTTP_PROCESS_H__
#define __HTTP_PROCESS_H__

#include <sys/types.h>

typedef struct
{
	int pid;
	int master_pid;
	unsigned int slot;
	int fd[2];
} process; 

void worker_main(int fd[2]);
int start_workers();
int get_worker_fd();
int reset_worker(pid_t pid);

void server_shutdown(int signo);
void worker_shutdown(int epoll_fd);


#endif
