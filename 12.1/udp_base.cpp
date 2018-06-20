#include "udp_base.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

UdpBase::UdpBase(const char *server, unsigned short port)
{
	_fp	= socket(AF_INET, SOCK_DGRAM, 0);

	if(server || port)
	{
		struct sockaddr_in sa;
		sa.sin_family		= AF_INET;
		sa.sin_port			= htons(port);
		sa.sin_addr.s_addr	= inet_addr(server);

		int sock_opts	= 1;

		setsockopt(_fp, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opts, sizeof(sock_opts));
		sock_opts	= fcntl(_fp, F_GETFL);
		sock_opts	|= O_NONBLOCK;
		fcntl(_fp, F_SETFL, sock_opts);

		if(bind(_fp, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0)
		{
			fprintf(stderr, "UdpBase %s:%d bind failed: %s\n", server, port, strerror(errno));
			exit(-1);
		}
	}
}

void UdpBase::onRecv()
{
	socklen_t len;
	unsigned char buffer[1024];
	struct sockaddr_in sa;
	int ret	= recvfrom(_fp, buffer, 1024, 0, (struct sockaddr *)&sa, &len);
	if(ret < 0)
	{
		fprintf(stderr, "UdpBase %s:%d recvfrom error:%s\n", _server.c_str(), _port, strerror(errno));
		close();
		return;
	}
	onPacket(buffer, ret, (struct sockaddr *)&sa);
}

int UdpBase::onSend(unsigned char *buffer, unsigned int size, Packet *packet)
{
	UdpPacket *p	= dynamic_cast<UdpPacket *>(packet);
	if(!p)
		return size;

	int ret	= sendto(_fp, buffer, size, 0, p->saAddr(), sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		fprintf(stderr, "UdpBase %s:%d recvfrom error:%s\n", _server.c_str(), _port, strerror(errno));
		close();
		return ret;
	}
	return size;
}
