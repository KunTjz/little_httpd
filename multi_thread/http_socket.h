#ifndef __HTTP_SOCKET_H__
#define __HTTP_SOCKET_H__

#include "http_timer.h"

enum sock_state
{
	STATE_HTTP_UNSET = 1,
	STATE_HTTP_READ,
	STATE_HTTP_WRITE
};

typedef struct socket_buffer
{
	int fd;
	unsigned int state;

	char* read_buf;
	char* write_buf;
	unsigned int rb_size;
	unsigned int wb_size;
	
	time_stamp  timer;
}* socket_buf;

int set_nonblocking(int fd);

socket_buf generate_socket_buf(int fd);
socket_buf find_socket_buf(int fd);

int socket_read_buf_alloc(socket_buf sb, int size);
int socket_write_buf_alloc(socket_buf sb, int size);

void free_socket_buf(int fd);
void free_socket_buf(socket_buf sb);

#endif
