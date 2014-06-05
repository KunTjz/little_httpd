#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <syslog.h>
#include <map>

#include "sys_define.h"
#include "http_process.h"
#include "http_channel.h"
#include "http_epoll.h"
#include "http_socket.h"
#include "http_connection.h"

#define WORKER_NUM	4

pid_t master_pid;
process workers[WORKER_NUM];

extern std::map<int, socket_buf> sb_array;

void worker_main(int fd[2])
{
	close(fd[0]);
		
	worker_loop(fd[1]);
}

int start_worker(int i)
{		
	int fd[2];
	pid_t pid;

	if(socketpair( AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
		perror( "socketpair()" );
		return L_HTTP_FAIL;
	}
	if(set_nonblocking(fd[0]) == L_HTTP_FAIL ||
			set_nonblocking(fd[1]) == L_HTTP_FAIL)
		return L_HTTP_FAIL;

	if ((pid = fork ()) == 0) {
		printf("fork success: %d\n", getpid());
		worker_main(fd);
		exit(1);
	}
	else if(pid < 0){
		syslog (LOG_INFO, "fork error");
		printf("fork error");
		return L_HTTP_FAIL;
	}

	workers[i].pid = pid;
	workers[i].slot = i;
	workers[i].fd[0] = fd[0];
	workers[i].fd[1] = fd[1];
	close(fd[1]);

	return L_HTTP_SUCCESS;
}

int start_workers()
{
	int i = 0;
//	int fd[2];
//	pid_t pid;

	master_pid = getpid();

	for(i = 0; i < WORKER_NUM; i++){
		/*if(socketpair( AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
			perror( "socketpair()" );
			return L_HTTP_FAIL;
		}
		if(set_nonblocking(fd[0]) == L_HTTP_FAIL ||
				set_nonblocking(fd[1]) == L_HTTP_FAIL)
			return L_HTTP_FAIL;

		if ((pid = fork ()) == 0) {
			printf("fork success: %d\n", getpid());
			worker_main(fd);
			exit(1);
		}
		else if(pid < 0){
			syslog (LOG_INFO, "fork error");
			printf("fork error");
			return L_HTTP_FAIL;
		}

		workers[i].pid = pid;
		workers[i].slot = i;
		workers[i].fd[0] = fd[0];
		workers[i].fd[1] = fd[1];
		close(fd[1]);
		*/
		start_worker(i);
	}

	return L_HTTP_SUCCESS;
}

int get_worker_fd()
{
	static int slot = 0;
	int i;

	if(slot < 0 || slot >= WORKER_NUM)
		slot = 0;
	
	if(workers[slot].pid != -1) {
		syslog(LOG_INFO, "getworker:%d", slot);
		return workers[slot++].fd[0];
	}

	i = slot + 1;
	while(i != slot) {
		if(i >= WORKER_NUM)
			i = 0;
		if(workers[i].pid != -1) {
			slot = i;
			syslog(LOG_INFO, "getworker:%d", slot);
			return workers[slot++].fd[0];
		}
		i++;
	}

	return L_HTTP_FAIL;
}

int reset_worker(pid_t pid)
{
	int i = 0;
	for(i = 0; i < WORKER_NUM; i++) {
		if(workers[i].pid == pid) {
			workers[i].pid = -1;
			
			close(workers[i].fd[0]);
			workers[i].fd[0] = -1;
			workers[i].fd[1] = -1;

			syslog(LOG_INFO, "proc:%d reset", pid);
			break;
		}
	}

	if(i == WORKER_NUM)
		return L_HTTP_FAIL;
	
	//todo: restart a worker
	return start_worker(i);
}

void server_shutdown(int signo)
{
	// sent signal
	channel ch;
	int i = 0;	
	pid_t pid;
	
	syslog(LOG_INFO, "in worker_shutdown");

	// 可能sigchld会打断执行过程，
	// 而本函数后面会处理子进程退出的问题
	// 所以这里先把它屏蔽掉
	if(signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		printf("set SIGINT error\n");
	}	

	ch.cmd = HTTP_COMMAND_EXIT;
	ch.fd = -1;
	
	for(; i < WORKER_NUM; i++) {
		if(workers[i].pid == -1)
			continue;
		
		if(send_fd(workers[i].fd[0], &ch) == L_HTTP_FAIL) {
			printf("send COMMAND_EXIT to %d fail\n", workers[i].pid);
			continue;
		}

		workers[i].pid = -1;
	}

	// wait child 
	while(1) {
		if((pid = waitpid(-1, NULL, 0)) < 0) {
			printf("no more child\n");
			break;
		}
	}

	exit(0);
}

void worker_shutdown(int epoll_fd)
{
	// shutdown all the connection
	std::map<int, socket_buf>::iterator it = sb_array.begin();
	int fd = -1;
	
	for(; it != sb_array.end(); it++) {
		fd = it->first;
		if(fd == -1)
			continue;

		epoll_del_event(epoll_fd, fd);

		printf("debug: start free %d\n", fd);
		sb_array.erase(sb_array.find(fd));
		free_socket_buf(it->second);

		close(fd);
	}

	exit(0);
}
