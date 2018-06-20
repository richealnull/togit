#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <string>
#include "net_base.h"
#include "tcp_packet.h"

using namespace std;

class TcpServerClient : public NetBase, public TcpParserHandler
{
protected:
	string _server;
	unsigned short _port;

	virtual void onRecv();
	virtual int onSend(unsigned char *buffer, unsigned int size, Packet *packet);
	virtual void onClose();
public:
	TcpServerClient(const char *server, unsigned short port, int fp):NetBase(),TcpParserHandler()
	{
		_server	= server;
		_port	= port;

		_fp		= fp;
	}
	virtual ~TcpServerClient() {}

	virtual ITcpParser *parser()	= 0;
};

class ITcpServerHandler
{
public:
	virtual ~ITcpServerHandler() {}

	virtual TcpServerClient *create(const char *server, unsigned short port, int conn)	= 0;
    virtual string getHandlerName(){ return 0;};
};

template<class TCPSERVERCLIENT>
class TcpServerHandler : public ITcpServerHandler
{
public:
	virtual ~TcpServerHandler() {}

	virtual TcpServerClient *create(const char *server, unsigned short port, int conn)
	{
		client	= new TCPSERVERCLIENT(server, port, conn);
		if(client)
			client->start();
		return client;
	}
    virtual string getHandlerName(){ return client->getClassName(); };
public:
    TCPSERVERCLIENT* client;
};

class TcpServer : public NetBase
{
public:
	ITcpServerHandler *_handler;
	
	string _server;
	unsigned short _port;
protected:
	virtual void onRecv();
public:
	virtual ~TcpServer() {}

	TcpServer(const char *server, unsigned short port, ITcpServerHandler *handler);
};

#endif
