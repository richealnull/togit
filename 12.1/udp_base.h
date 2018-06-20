#ifndef __UDP_BASE_H__
#define __UDP_BASE_H__

#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <string>
#include "packet.h"
#include "net_base.h"

using namespace std;

class UdpPacket : public Packet
{
private:
	unsigned char _buffer[1024];
	unsigned int _length;
	struct sockaddr_in _sa;
public:
	UdpPacket(unsigned char *buffer, unsigned int len, const char *server, unsigned short port):Packet()
	{
		if(len > 1024)
			len	= 1024;
		memcpy(_buffer, buffer, len);
		_length	= len;
	
		_sa.sin_family		= AF_INET;
		_sa.sin_port		= htons(port);
	
		struct hostent *ht = gethostbyname(server);
		memcpy(&_sa.sin_addr, ht->h_addr, ht->h_length);
	}
	UdpPacket(unsigned char *buffer, unsigned int len, struct sockaddr *sa):Packet()
	{
		if(len > 1024)
			len	= 1024;
		memcpy(_buffer, buffer, len);
		_length	= len;
		memcpy(&_sa, sa, sizeof(struct sockaddr_in));
	}
	struct sockaddr *saAddr() {return (struct sockaddr *)&_sa;}
	virtual unsigned char *data() {return _buffer;}
	virtual unsigned int length() {return _length;}
	virtual void release()
	{
		delete this;
	}
};

class UdpBase : public NetBase
{
protected:
	string _server;
	unsigned short _port;

	virtual void onRecv();
	virtual int onSend(unsigned char *buffer, unsigned int size, Packet *packet);
	virtual void onPacket(unsigned char *buffer, unsigned int size, struct sockaddr *sa) {}
public:
	UdpBase(const char *server = 0, unsigned short port = 0);
};

#endif
