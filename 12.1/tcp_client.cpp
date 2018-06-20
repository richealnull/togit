#include "tcp_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "log.h"

TcpClient::TcpClient(const char *server, unsigned short port):NetBase(),TcpParserHandler()
{
	_server	= server;
	_port	= port;
}

void TcpClient::onConnect()
{
	LogInfo("onConnect %d.\n", _fp);
}

void TcpClient::onRecv()
{
	NetBase::onRecv();

	while(1)
	{
		static unsigned char buffer[1024];
		int size = recv(_fp, buffer, 1024, 0);
		if(!size)
		{
			LogError("TcpClient %s:%d recv 0 info:%s\n", _server.c_str(), _port, strerror(errno));
			close();
			break;
		}
		if(size < 0)
		{
			if(EAGAIN != errno)
			{
				LogError("TcpClient %s:%d recv error:%s\n", _server.c_str(), _port, strerror(errno));
				close();
			}
			break;
		}

		parser()->parse(buffer, size, this);
	}
}

int TcpClient::onSend(unsigned char *buffer, unsigned int size, Packet *packet)
{
	return ::send(_fp, buffer, size, 0);
}

void TcpClient::start()
{
	_fp	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in sa;
	sa.sin_family		= AF_INET;
	sa.sin_port			= htons(_port);
	
	struct hostent *ht = gethostbyname(_server.c_str());
	memcpy(&sa.sin_addr, ht->h_addr, ht->h_length);
	
	if(connect(_fp, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	{
        LogError("TcpClient connect %s:%d error:%s", _server.c_str(), _port, strerror(errno));
		close();
		_fp	= 0;
	}
	int sock_opts	= fcntl(_fp, F_GETFL);
	sock_opts	|= O_NONBLOCK;
	fcntl(_fp, F_SETFL, sock_opts);

	NetBase::start();	
	onConnect();
}
