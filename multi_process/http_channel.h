#ifndef __HTTP_CHANNEL_H__
#define __HTTP_CHANNEL_H__

enum {
	HTTP_COMMAND_TRANS_FD = 0,
	HTTP_COMMAND_RESTART,
	HTTP_COMMAND_EXIT
};

typedef struct
{
	unsigned int cmd;
	int	fd; 	
}channel;

int send_fd(int fd, channel* ch);
int recv_fd(int fd, channel* ch);

#endif
