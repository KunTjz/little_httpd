#ifndef __HTTP_CONNECTION_H__
#define __HTTP_CONNECTION_H__

#include "http_socket.h"

int tcp_listen();
int handle_connection(socket_buf sb);
int on_accept(int epoll_fd, int listen_fd);
int on_read(int epoll_fd, int fd);
int on_write(int epoll_fd, int fd);
int on_close(int epoll_fd, int fd);
int on_channel(int epoll_fd, int fd);
void on_worker_breakdown(int signo);

void master_loop();
void worker_loop(int channel_fd);

#endif
