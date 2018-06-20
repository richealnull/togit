#include "tcp_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include "log.h"
#include "conntable.h"
#include "utility.h"

void TcpServerClient::onRecv()
{
	NetBase::onRecv();

	while(1)
	{
		static unsigned char buffer[1024];
        {       
        Lock(NetBase::tables_mutex);
		int size = recv(_fp, buffer, 1024, 0);
		if(!size)
		{
			close();
			break;
		}
		if(size < 0)
		{
			if(EAGAIN != errno)
				close();
			break;
		}
		for(int i =0;i<size;i++)
       		{
                	printf("%02x ",buffer[i]);
        	}

		parser()->parse(buffer, size, this);
        }
	}
}

int TcpServerClient::onSend(unsigned char *buffer, unsigned int size, Packet *packet)
{
	return ::send(_fp, buffer, size, 0);
}

void TcpServerClient::onClose()
{
	NetBase::onClose();

	delete this;
}

TcpServer::TcpServer(const char *server, unsigned short port, ITcpServerHandler *handler):NetBase()
{
	_port	= port;
	_server	= server;

	_handler	= handler;

	_fp		= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	struct sockaddr_in	saLocal;
	saLocal.sin_family	= AF_INET;
	saLocal.sin_port	= htons(port);
	saLocal.sin_addr.s_addr	= inet_addr(server);

    //设置接收缓冲区
    int nRecvBuf=38*1024;//设置为32K
    setsockopt(_fp,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    //设置发送缓冲区
    int nSendBuf=38*1024;//设置为32K
    setsockopt(_fp,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	int sock_opts = 1;
	setsockopt(_fp, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opts, sizeof(sock_opts));
	sock_opts	= fcntl(_fp, F_GETFL);
	sock_opts	|= O_NONBLOCK;
	fcntl(_fp, F_SETFL, sock_opts);

	if(bind(_fp, (struct sockaddr *)&saLocal, sizeof(struct sockaddr_in)) < 0)
	{
		LogError("bind %s:%d failed: %s\n", server, port, strerror(errno));
		exit(-1);
	}

	listen(_fp, 5);

	LogInfo("listen %s:%d fp:%d\n", server, port, _fp);
    printf("listen server %s , fp %d\n",server, _fp);
}

void TcpServer::onRecv()
{
	while(1)
	{
		struct sockaddr_in sa;
		int salen	= sizeof(sa);
		int conn	= accept(_fp, (struct sockaddr *)&sa, (socklen_t *)&salen);
		if(conn < 0)
		{
			if(EAGAIN != errno && ECONNABORTED != errno && EPROTO != errno && EINTR != errno)
			{
				LogError("accept %s:%d error:%s\n", _server.c_str(), _port, strerror(errno));
				close();
			}
			break;
		}
        //+++++++++++++++++++++++++++++++++++++++
        //printf("11111111111111hh\n");
		LogInfo("accept %s:%d client %d\n", _server.c_str(), _port, conn);
	 	int sock_opts	= fcntl(conn, F_GETFL);
		sock_opts	|= O_NONBLOCK;
		fcntl(conn, F_SETFL, sock_opts);
		_handler->create(_server.c_str(), _port, conn);
       		string className = _handler->getHandlerName();
        	printf("classname %s\n",className.c_str()); 
        if(className != "MulticasterRelayFrom" && className !="MulticasterRelayTo"){
              // {
              // Lock(NetBase::tables_mutex);
               TcpServer::_tables[conn] =new conntable(conn);
		       printf("tcp_server tables\n");
               conntable* table =TcpServer::_tables[conn];
               int Fd;
               table->setTime(time(NULL));
               table->getFd(&Fd);
               printf("table ---------------%d\n",Fd); 
               //}
               //+++++++++++++++++++++++++++++++++++++++ 
         }  
	}
}
