#ifndef _SERVER_EPOLL_
#define _SERVER_EPOLL_

#include <iostream>
#include <cstdio>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

class server_epoll{
public:
	server_epoll();
	server_epoll(unsigned short port, int maxnum,int timeout);
	~server_epoll();
	
	int server_init();
	void server_start();
	int set_noblock(int fd);
	int SetReUseAddr(int fd);
	int server_wait();
	int nio_write(int fd, char* buf, int len);
	
private:
	unsigned short _Port;
	int	_maxnum;
	int serverfd;
	int epollfd;
	int	m_timeout;
	struct epoll_event *m_epoll;
	
};



#endif
