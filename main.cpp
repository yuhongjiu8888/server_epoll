#include "server_epoll.h"

int main(int argc, char* argv[])
{
	server_epoll app;
	int iRet = app.server_init();
	if (iRet < 0)
	{
		std::cout << "server init error!" << std::endl;
		return -1;
	}

	app.server_start();

	return 0;
}