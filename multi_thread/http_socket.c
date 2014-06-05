#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <map>

#include "http_socket.h"
#include "sys_define.h"

#define DEFAULT_READ_BUF_SIZE 2048
#define DEFAULT_WRITE_BUF_SIZE 1024*1024

#define MAX_READ_BUF_SIZE 4096
#define MAX_WRITE_BUF_SIZE 1024*1024*10

std::map<int, socket_buf> sb_array;

socket_buf find_socket_buf(int fd)
{
	std::map<int, socket_buf>::iterator it = sb_array.find(fd);
	if(it == sb_array.end())
		return NULL;

	return it->second;
}

int set_nonblocking(int fd)
{
	int val;

	if((val = fcntl(fd, F_GETFL, 0)) < 0) {
		perror("fcntl(fd, F_GETFL) failed:)");
		return L_HTTP_FAIL;
	}
	if(fcntl(fd, F_SETFL, O_NONBLOCK | val) < 0) {
		perror("fcntl(fd, F_SETFL, O_NONBLOCK) failed: ");
		return L_HTTP_FAIL;
	}

	return L_HTTP_SUCCESS;
}

socket_buf generate_socket_buf(int fd)
{
	socket_buf sb = (socket_buf)malloc(sizeof(struct socket_buffer));

	if(sb == NULL) {
		perror("malloc failed");
		return NULL;
	}

	sb->fd = fd;
	sb->state = STATE_HTTP_UNSET;
	sb->rb_size = 0;
	sb->wb_size = 0;
	sb->read_buf = (char*)malloc(sizeof(char) * DEFAULT_READ_BUF_SIZE);
	sb->write_buf = (char*)malloc(sizeof(char) * DEFAULT_WRITE_BUF_SIZE);
	if(sb->read_buf == NULL || sb->write_buf == NULL)
		return NULL;

	return sb;
}

int socket_read_buf_alloc(socket_buf sb, int size)
{
	if(size <= 0 || size >= MAX_READ_BUF_SIZE) {
		printf("invalid read buf size\n");
		return L_HTTP_FAIL;
	}
	
	if(size <= DEFAULT_READ_BUF_SIZE)
		return L_HTTP_SUCCESS;

	sb->read_buf = (char*)calloc(size, sizeof(char));
	if(sb->read_buf == NULL) {
		printf("calloc fail\n");
		return L_HTTP_FAIL;
	}
	
	return L_HTTP_SUCCESS;
}

int socket_write_buf_alloc(socket_buf sb, int size)
{
	if(size <= 0 || size >= MAX_WRITE_BUF_SIZE) {
		printf("invalid write buf size\n");
		return L_HTTP_FAIL;
	}
	
	if(size <= DEFAULT_WRITE_BUF_SIZE)
		return L_HTTP_SUCCESS;

	sb->write_buf = (char*)calloc(size, sizeof(char));
	if(sb->write_buf == NULL) {
		printf("calloc fail\n");
		return L_HTTP_FAIL;
	}
	
	return L_HTTP_SUCCESS;
}

void free_socket_buf(int fd)
{
	socket_buf sb = find_socket_buf(fd);
	free_socket_buf(sb);
}

void free_socket_buf(socket_buf sb)
{
	if(sb == NULL)
		return;
	
	free(sb->read_buf);
	sb->rb_size = 0;

	free(sb->write_buf);
	sb->wb_size = 0;
	
	free(sb);
}
