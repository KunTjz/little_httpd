#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>

#include "sys_define.h"
#include "http_channel.h"

int send_fd(int fd, channel* ch)
{
	struct msghdr msg;  
	struct iovec iov[1]; 
	int nsend;

	// 有些系统使用的是旧的msg_accrights域来传递描述符，Linux下是新的msg_control字段  
//#ifdef HAVE_MSGHDR_MSG_CONTROL  
	union{ // 保证cmsghdr和msg_control的对齐  
		struct cmsghdr cm;  
		char control[CMSG_SPACE(sizeof(int))];  
	}control_un;  
	struct cmsghdr *cmptr;   

	// 设置辅助缓冲区和长度  
	msg.msg_control = control_un.control;   
	msg.msg_controllen = sizeof(control_un.control);  

	if(ch->fd != -1) {
		// 只需要一组附属数据就够了，直接通过CMSG_FIRSTHDR取得  
		cmptr = CMSG_FIRSTHDR(&msg);  

		// 设置必要的字段，数据和长度  
		cmptr->cmsg_len = CMSG_LEN(sizeof(int)); // fd类型是int，设置长度  
		cmptr->cmsg_level = SOL_SOCKET;   
		cmptr->cmsg_type = SCM_RIGHTS;  // 指明发送的是描述符  
		*((int*)CMSG_DATA(cmptr)) = ch->fd; // 把fd写入辅助数据中  
		//#else  
		//	msg.msg_accrights = (caddr_t)&send_fd; // 旧的方法
		//	msg.msg_accrightslen = sizeof(int);  
		//#endif  
	}
	else {
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
	}

	msg.msg_flags = 0;

	// UDP才需要，无视  
	msg.msg_name = NULL;  
	msg.msg_namelen = 0;  

	// 别忘了设置数据缓冲区，实际上1个字节就够了  
	iov[0].iov_base = (char*)ch;  
	iov[0].iov_len = sizeof(channel);  
	msg.msg_iov = iov;  
	msg.msg_iovlen = 1;  
	
	if((nsend = sendmsg(fd, &msg, 0)) < 0) {
		perror("sendmsg");
		return L_HTTP_FAIL;
	}

	syslog(LOG_INFO, "--------send proc:%d fd:%d", fd, ch->fd);

	return L_HTTP_SUCCESS;
}

int recv_fd(int fd, channel* ch)
{
	struct msghdr msg;  
	struct iovec iov[1];  
	int n;  
//#ifdef HAVE_MSGHDR_MSG_CONTROL  
	union{ // 对齐  
		struct cmsghdr cm;  
		char control[CMSG_SPACE(sizeof(int))];  
	}control_un;  
	struct cmsghdr *cmptr;  

	// 设置辅助数据缓冲区和长度  
	msg.msg_control = control_un.control;  
	msg.msg_controllen = sizeof(control_un.control);  
//#else  
//	msg.msg_accrights = (caddr_t) &newfd; // 这个简单  
//	msg.msg_accrightslen = sizeof(int);  
//#endif   

	// TCP无视  
	msg.msg_name = NULL;  
	msg.msg_namelen = 0;  

	// 设置数据缓冲区  
	iov[0].iov_base = (char*)ch;  
	iov[0].iov_len = sizeof(channel);  
	msg.msg_iov = iov;  
	msg.msg_iovlen = 1;  

	// 设置结束，准备接收  
	if((n = recvmsg(fd, &msg, 0)) <= 0)  
	{  
		return n;  
	}  
//#ifdef HAVE_MSGHDR_MSG_CONTROL  

	// 检查是否收到了辅助数据，以及长度 
	cmptr = CMSG_FIRSTHDR(&msg);  
	if((cmptr != NULL) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int))))  
	{  
		// 还是必要的检查  
		if(cmptr->cmsg_level != SOL_SOCKET)  
		{  
			printf("control level != SOL_SOCKET/n");  
			return L_HTTP_FAIL; 
		}  
		if(cmptr->cmsg_type != SCM_RIGHTS)  
		{  
			printf("control type != SCM_RIGHTS/n");  
			return L_HTTP_FAIL;
		}  
		// 好了，描述符在这  
		ch->fd = *((int*)CMSG_DATA(cmptr));  
	}  
	else  
	{  
		if(cmptr == NULL) 
			printf("null cmptr, fd not passed./n");  
		else 
			printf("message len[%d] if incorrect./n", cmptr->cmsg_len);  
		
		ch->fd = -1; // descriptor was not passed
	}  
//#else  
//	if(msg.msg_accrightslen == sizeof(int)) *recvfd = newfd;   
//	else *recvfd = -1;  
//#endif  
	syslog(LOG_INFO, "++++++++proc:%d receive fd:%d", getpid(), ch->fd);	

	return L_HTTP_SUCCESS;  
}

