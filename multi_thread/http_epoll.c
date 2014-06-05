#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "http_epoll.h"
#include "sys_define.h"
#include "http_socket.h"
#include "http_connection.h"

#define MAX_EVENTS 1024
#define MAX_CONNECTION 1000
#define MAX_LINE 4096
#define EPOLL_DELAY  500

int epoll_init()
{
	int epoll_fd = epoll_create(MAX_EVENTS);
	if(epoll_fd < 0) {
		perror("epoll_create fail");
		return L_HTTP_FAIL;
	}
	
	return epoll_fd;
}

int epoll_add_event(int epoll_fd, uint32_t op, int fd)
{
	printf("add fd:%d\n", fd);

	struct epoll_event ev;

	ev.data.fd = fd;
	ev.events = op;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		perror("epoll_ctl fail");
		return L_HTTP_FAIL;
	}

	return L_HTTP_SUCCESS;
}

int epoll_modify_mod(int epoll_fd, uint32_t mod, int fd)
{
	struct epoll_event ev;
	
	ev.data.fd = fd;
	ev.events = mod;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
		perror("epoll_ctl fail");
		return L_HTTP_FAIL;
	}

	return L_HTTP_SUCCESS;
}

int epoll_del_event(int epoll_fd, int fd)
{
	struct epoll_event ev;

	ev.data.fd = fd;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) < 0) {
		perror("epoll_ctl fail");
		return L_HTTP_FAIL;
	}
	
	return L_HTTP_SUCCESS;
}

int epoll_start(int epoll_fd, unsigned int max_events, int listen_sock)
{
	int nfds;
	struct epoll_event events[max_events];

	nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_DELAY);
	if (nfds == -1) {
		perror("epoll_wait");
		return L_HTTP_FAIL;
	}

	for(int n = 0; n < nfds; ++n) {
		if (events[n].data.fd == listen_sock) {
			if(on_accept(epoll_fd, listen_sock) == L_HTTP_FAIL)
				continue;
		}
		else if(events[n].events & EPOLLIN)//如果是已经连接的用户，并且收到数据，那么进行读入。
		{
			if(on_read(epoll_fd, events[n].data.fd) == L_HTTP_FAIL) {
				on_close(epoll_fd, events[n].data.fd);
				continue;
			}

		}
		else if(events[n].events & EPOLLOUT) // 如果有数据发送
		{
			on_write(epoll_fd, events[n].data.fd);
			on_close(epoll_fd, events[n].data.fd);
		}
	}

	return L_HTTP_SUCCESS;
}
