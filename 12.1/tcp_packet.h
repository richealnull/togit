#ifndef __TCP_PACKET_H__
#define __TCP_PACKET_H__

#include <stdlib.h>
#include <string>
#include "packet.h"
using namespace std;

class TcpParserHandler
{
public:
	virtual ~TcpParserHandler() {}

	virtual void onPacket(unsigned char *buffer, unsigned int size) = 0;
};

class ITcpParser
{
public:
	virtual void parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler) = 0;
};

class TcpParser141 : public ITcpParser
{
private:
	static const unsigned char TCPPROCOTOL_STATE_HEAD = 0;
	static const unsigned char TCPPROCOTOL_STATE_LENGTH = 1;
	static const unsigned char TCPPROCOTOL_STATE_CSLENGTH = 2;
	static const unsigned char TCPPROCOTOL_STATE_BODY = 3;
	
	unsigned char _state;
	unsigned int _length;
	unsigned int _pos;
	unsigned char *_buffer;
public:
	TcpParser141()
	{
		_buffer	= 0;
		_state	= TCPPROCOTOL_STATE_HEAD;
		_length	= 0;
		_pos	= 0;
	}
	virtual ~TcpParser141()
	{
		if(_buffer)
		{
			free(_buffer);
			_buffer	= 0;
		}
	}
	virtual void parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler);
};

class TcpPacket141 : public Packet
{
private:
	unsigned char *_buffer;
	unsigned int _length;
public:
	TcpPacket141(unsigned char *buffer, unsigned int size);
	virtual ~TcpPacket141();
	
	virtual unsigned char *data() {return _buffer;}
	virtual unsigned int length() {return _length;}
};

class TcpParser121n1 : public ITcpParser
{
private:
	static const unsigned char TCPPROCOTOL_STATE_HEAD = 0;
	static const unsigned char TCPPROCOTOL_STATE_LENGTH = 1;
	static const unsigned char TCPPROCOTOL_STATE_CSLENGTH = 2;
	static const unsigned char TCPPROCOTOL_STATE_BODY = 3;
	
	unsigned char _state;
	unsigned int _length;
	unsigned int _pos;
	unsigned char _cs;
	unsigned char *_buffer;
public:
	TcpParser121n1()
	{
		_buffer	= 0;
		_state	= TCPPROCOTOL_STATE_HEAD;
		_length	= 0;
		_pos	= 0;
		_cs		= 0;
	}
	virtual ~TcpParser121n1()
	{
		if(_buffer)
			free(_buffer);
	}
	virtual void parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler);
};

class TcpPacket121n1 : public Packet
{
private:
	unsigned char *_buffer;
	unsigned int _length;
public:
	TcpPacket121n1(unsigned char *buffer, unsigned int size);
	virtual ~TcpPacket121n1();
	
	virtual unsigned char *data() {return _buffer;}
	virtual unsigned int length() {return _length;}
};

class TcpParser12n1 : public ITcpParser
{
private:
	static const unsigned char TCPPROCOTOL_STATE_HEAD = 0;
	static const unsigned char TCPPROCOTOL_STATE_LENGTH = 1;
	static const unsigned char TCPPROCOTOL_STATE_BODY = 2;
	
	unsigned char _state;
	unsigned int _length;
	unsigned int _pos;
	unsigned char _cs;
	unsigned char *_buffer;
public:
	TcpParser12n1()
	{
		_buffer	= 0;
		_state	= TCPPROCOTOL_STATE_HEAD;
		_length	= 0;
		_pos	= 0;
		_cs		= 0;
	}
	virtual ~TcpParser12n1()
	{
		if(_buffer)
			free(_buffer);
	}
	virtual void parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler);
};

class TcpPacket12n1 : public Packet
{
private:
	unsigned char *_buffer;
	unsigned int _length;
public:
	TcpPacket12n1(unsigned char *buffer, unsigned int size);
	virtual ~TcpPacket12n1();
	
	virtual unsigned char *data() {return _buffer;}
	virtual unsigned int length() {return _length;}
};

class TcpParserLine : public ITcpParser
{
private:
	static const unsigned int MAX_LINE_BUFFER = 255;

	char _buffer[MAX_LINE_BUFFER];
	unsigned int _length;
public:
	TcpParserLine()
	{
		_length	= 0;
	}
	virtual void parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler);
};

class TcpPacketLine : public Packet
{
private:
	string _packet;
public:
	TcpPacketLine(const string &packet)
	{
		_packet	= packet;
		_packet	+= "\n";
	}

	virtual ~TcpPacketLine() {}
	
	virtual unsigned char *data() {return (unsigned char *)_packet.c_str();}
	virtual unsigned int length() {return _packet.size() + 1;}
};

#endif
