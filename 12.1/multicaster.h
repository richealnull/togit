#ifndef __MULTICASTER_H__
#define __MULTICASTER_H__

#include "tcp_client.h"
#include "tcp_server.h"
#include "udp_base.h"
#include <map>

using namespace std;

#define MULTICASTER_OPCODE_REGISTER 0x01
#define MULTICASTER_OPCODE_MULTICAST 0x02
#define MULTICASTER_OPCODE_BOARDCAST 0x03
#define MULTICASTER_OPCODE_HEARTBEAT 0x04
#define MULTICASTER_OPCODE_DELETE 0x05
#define MULTICASTER_OPCODE_CHANGE 0x06
#define MULTICASTER_OPCODE_SELECT 0x07
#define MULTICASTER_OPCODE_TIME 0x08
#define MULTICASTER_OPCODE_SYS 0x09
#define MULTICASTER_WGID_LEN 11
#define MULTICASTER_APP_LEN 13
//解析数据包的类
class MulticasterParser
{
public:
    //解析多播
	static int multicast(unsigned char *buffer, unsigned int len);
    //解析广播
	static int boardcast(unsigned char *buffer, unsigned int len);
};

//客户端连接处理类
class MulticasterClient : public TcpServerClient
{
private:
	string _userId;
    bool _avalidated;
    
    //包解析器
	TcpParser141 _parser;
	
    //包处理函数
	virtual void onPacket(unsigned char *buffer, unsigned int size);
    //连接关闭处理函数
	virtual void onClose();
public:
    static map<string, MulticasterClient *> _clients;
	MulticasterClient(const char *server, unsigned short port, int fp);
	virtual ~MulticasterClient();

    //解析器
	virtual ITcpParser *parser() {return &_parser;};
};

class MulticasterRelayFrom : public TcpServerClient
{
private:
    //其它接线员集合
	static deque<MulticasterRelayFrom *> _froms;

    //包解析器
	TcpParser141 _parser;

    //包处理函数
	virtual void onPacket(unsigned char *buffer, unsigned int size);
    //连接关闭函数
	virtual void onClose();
public:
	MulticasterRelayFrom(const char *server, unsigned short port, int fp);
	virtual ~MulticasterRelayFrom();

    //解析器
	virtual ITcpParser *parser() {return &_parser;}

    //virtual void onRecv();

    virtual string getClassName();

    //中继到其它接线员
	static void relay(unsigned char *buffer, unsigned int size);
};

class MulticasterRelayTo : public TcpClient
{
protected:
    //其它接线员集合
	static deque<MulticasterRelayTo *> _tos;

    //包解析器
	TcpParser141 _parser;

    //包处理函数
	virtual void onPacket(unsigned char *buffer, unsigned int size);
    //连接关闭函数
	virtual void onClose();
public:
	MulticasterRelayTo(const char *server, unsigned short port);
	virtual ~MulticasterRelayTo();
	
    //解析器
	virtual ITcpParser *parser() {return &_parser;}

    virtual string getClassName();

    //中继到其它接线员
	static void relay(unsigned char *buffer, unsigned int size);
};

#endif
