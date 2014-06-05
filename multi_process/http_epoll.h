#ifndef __HTTP_EPOLL_H__
#define __HTTP_EPOLL_H__

int epoll_init();
int epoll_add_event(int epoll_fd, uint32_t mod, int fd);
int epoll_del_event(int epoll_fd, int fd);
int epoll_modify_mod(int epoll_fd, uint32_t mod, int fd);

int master_epoll_start(int epoll_fd, unsigned int max_events, int listen_sock);

int worker_epoll_start(int epoll_fd, unsigned int max_events, int channel_fd);

#endif
