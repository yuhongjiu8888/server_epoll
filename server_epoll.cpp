#include "server_epoll.h"

server_epoll::server_epoll(){
	_Port = 8080;	/*defual port*/
	_maxnum = 10;
	m_timeout = 3000;
}

server_epoll::server_epoll(unsigned short port,int maxnum,int timeout){
	_Port = port;
	_maxnum = maxnum;
	m_timeout = timeout;
}

server_epoll::~server_epoll(){
	if(NULL!=m_event)
		delete m_event;
	if(serverfd)
		close(serverfd);
}

int server_epoll::server_init(){
	struct sockaddr_in serveraddr;
	
	epollfd = epoll_create(_maxnum);
	if(epollfd < 0){
		perror("epoll_create error!\n");
		std::cout<<errno<<:<<strerror(errno)<<std::endl;
		return -1;
	}
	
	memset(&serveraddr,0x00,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(_Port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	serverfd = socket(AF_INET,SOCK_STREAM,0);
	if(serverfd == -1){
		perror("socket create error \n");
		std::cout<<errno<<:<<strerror(errno)<<std::endl;
		return -1;
	}
	std::cout<<"socket create success!["<<serverfd<<"]"<<std::endl;

	if(bind(serverfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in)) == -1){
		perror("bind socket error \n");
		std::cout<<errno<<:<<strerror(errno)<<std::endl;
		return -1;
	}

	if(listen(serverfd,5) == -1){
		perror("listen socket error \n");
		std::cout<<errno<<:<<strerror(errno)<<std::endl;
		return -1;
	}

	set_noblock(serverfd);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = serverfd;

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,serverfd,&ev) == -1){
		perror("epoll_ctl error \n");
		std::cout<<errno<<:<<strerror(errno)<<std::endl;
		return -1;
	}

	return serverfd;
}

void server_epoll::server_start(){
	int nReady = 0;
	m_epoll = new struct epoll_event[_maxnum];

	while(1){
		nReady = server_wait();

		if(nReady <= 0){
			perror("no client connected!\n");
			continue;
		}
		
		for(int i = 0; i < nReady; i++){
			if(m_epoll[i].data.fd == serverfd){
				struct sockaddr_in client_addr;
				memset(&client_addr, 0, sizeof(struct sockaddr_in));
				socklen_t client_len = sizeof(client_addr);
				
				int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
				if (clientfd < 0) {
					perror("accept");
					return 4;
				}
				std::cout<<"client come from ip<"<<inet_addr(client_addr.sin_addr)<<"> port:"<<ntohs(client_addr.sin_port);
				std::endl;

				set_noblock(clientfd);

				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
				ev.data.fd = clientfd;

				epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
			}
			else{
						/*recv接受报文*/
						/*send发送报文*/
			}
		}
	}
}


int server_epoll::server_wait(){
	return epoll_wait(epollfd,m_epoll,_maxnum,m_timeout);
}

int server_epoll::set_noblock(int fd){
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) 
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

int server_epoll::SetReUseAddr(int fd) {
	int reuse = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
}


int server_epoll::nio_write(int fd, char* buf, int len){
	int write_pos = 0;
	int left_len = len;
	int writed_len = 0;
	
	while(left_len > 0){
		writed_len = read(fd,buf+write_pos,left_len);
		if(writed_len < 0){
			if(error == EAGAIN){
				continue;
			}
			else
				return -1;
				
			
		}
		
		left_len -= writed_len;
		write_pos += writed_len; 
		
	}
	return len;
}
