#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <string>
#include "net_base.h"
#include "tcp_packet.h"

using namespace std;

class TcpClient : public NetBase, public TcpParserHandler
{
protected:
	string _server;
	unsigned short _port;

	virtual void onConnect();
	virtual void onRecv();
	virtual int onSend(unsigned char *buffer, unsigned int size, Packet *packet);
public:
	TcpClient(const char *server, unsigned short port);
	virtual ~TcpClient() {}

	virtual ITcpParser *parser()	= 0;

	void start();
};

#endif
