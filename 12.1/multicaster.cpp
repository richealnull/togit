#include "multicaster.h"
#include <map>
#include <arpa/inet.h>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include "log.h"
#include "md5.h"
#include "conntable.h"
#include "utility.h"
#include <sys/types.h>

char* _sysmiyue = (char*)"aabbcc";
char* _appmiyue = (char*)"bbccdd";
char* _wgmiyue = (char*)"ccddee";


unsigned long long ntohll(unsigned long long val)
{
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	{
		return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}
	else if (__BYTE_ORDER == __BIG_ENDIAN)
	{
		return val;
	}
}

unsigned long long htonll(unsigned long long val)
{
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	{
		return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}
	else if (__BYTE_ORDER == __BIG_ENDIAN)
	{
		return val; 
	}
}
//141封装
int buff(unsigned char* buffer,int size,unsigned char** _buff){
	int _length = 1 + 4 + 1 + size;
	unsigned char* _buffer = (unsigned char *)malloc(_length);
	_buffer[0]  = 0xaa;
	*((unsigned int *)(_buffer + 1))    = htonl(_length);
	_buffer[5]  = 0 - _buffer[0] - _buffer[1] - _buffer[2] - _buffer[3] - _buffer[4];
	memcpy(_buffer + 6, buffer, size);
	*_buff = _buffer;
	return _length;
}


deque<MulticasterRelayFrom *> MulticasterRelayFrom::_froms;
deque<MulticasterRelayTo *> MulticasterRelayTo::_tos;

map<string, MulticasterClient *> MulticasterClient::_clients; 

int MulticasterParser::multicast(unsigned char *buffer, unsigned int len)
{
	//传播数量
	unsigned short total	= ntohs(*(unsigned short *)(buffer + 1));
	printf("total is %d\n",total);
	//传播集合
	set<string> ids;

	unsigned short pos	= 3;
	for(unsigned int i = 0; i < total; i++)
	{
		unsigned short size	= ntohs(*(unsigned short *)(buffer + pos));
		printf("id len is %d\n",size);
		printf("len is %d\n",len);
		if(pos + 2 + size >= len)
		{
			LogError("protocol error.\n");
			return 0;
		}
		if(size == 11)
		{
			string userid = string((char*)buffer+pos+2,3);
			printf("chuanbo user id hera is %s\n",userid.c_str());
			unsigned long long long_buf =ntohll(*(unsigned long long*)(buffer+pos+5));
			char ltos2[16] ={0};
			sprintf(ltos2,"%lld",long_buf);
			userid += string(ltos2);
			printf("chuanbo user id is %s\n",userid.c_str());
			ids.insert(userid);

			pos	+= 2 + size;
		}
		else if(size == 13)
		{

			string userid = string((char*)buffer+pos+2,3);	
			printf("chuanbo user id hera is %s\n",userid.c_str());
			unsigned short short_buf =ntohs(*(unsigned short *)(buffer+pos+5));
			printf("short_buf is %d\n",short_buf);
			char ltos3[16] ={0};
			sprintf(ltos3,"%d",short_buf);
			userid += string(ltos3);

			unsigned long long long_buf =ntohll(*(unsigned long long*)(buffer+pos+7));
			char ltos2[16] ={0};
			sprintf(ltos2,"%lld",long_buf);
			userid += string(ltos2);
			printf("chuanbo user id is %s\n",userid.c_str());
			ids.insert(userid);

			pos	+= 2 + size;
		}
		else
		{

			string userid = string((char*)buffer+pos+2,size);
			printf("chuanbo user id is %s\n",userid.c_str());
			ids.insert(userid);

			pos += 2 + size;
		}
	}

	//传播
	if(ids.size() > 0)
	{
		unsigned int size	= ntohl(*(unsigned int *)(buffer + pos));
		if(pos + 4 + size != len)
		{
			LogError("protocol error.\n");
			return 0;
		}

		TcpPacket141 *packet	= new TcpPacket141(buffer + pos + 4, size);
		printf("size is %d\n",size);
		if(packet)
		{
			packet->addRef();
			for(set<string>::iterator it = ids.begin(); it != ids.end(); it++)
			{
				map<string, MulticasterClient *>::iterator mit	= MulticasterClient::_clients.find(*it);
				if(mit != MulticasterClient::_clients.end() && mit->second)
					mit->second->send(packet);
			}
			packet->release();
		}
	}
	return ids.size();
}

int MulticasterParser::boardcast(unsigned char *buffer, unsigned int len)
{
	//传播
	if(MulticasterClient::_clients.size() > 0)
	{
		unsigned int size	= ntohl(*(unsigned int *)(buffer + 1));
		if(5 + size != len)
		{
			LogError("protocol error.\n");
			return 0;
		}
		TcpPacket141 *packet	= new TcpPacket141(buffer + 5, size);
		if(packet)
		{
			packet->addRef();
			for(map<string, MulticasterClient *>::iterator it = MulticasterClient::_clients.begin(); it != MulticasterClient::_clients.end(); it++)
			{
				it->second->send(packet);
			}
			packet->release();
		}
	}
	return MulticasterClient::_clients.size();
}

MulticasterClient::MulticasterClient(const char *server, unsigned short port, int fp):TcpServerClient(server, port, fp)
{
	_userId	= "unregister_multicaster_client";
}

MulticasterClient::~MulticasterClient()
{
	_avalidated = false;
}

void MulticasterClient::onPacket(unsigned char *buffer, unsigned int size)
{
	//注册命令
	printf("Multicasterclient::onPacket out\n");

	if (buffer[0] == MULTICASTER_OPCODE_REGISTER)
	{
		printf("Onpacket size is %d\n",size);
		printf("Login----------------------------\n");
		if(size <7 || size -16 - 7 <= 0)
		{
			printf("size too small\n");
			return;
		}

		char* BUFFER_OFFSET = (char*)malloc(4); 
		unsigned char* BUFFER_MD5 = (unsigned char*)malloc(16);
		memcpy(BUFFER_OFFSET,(char*)buffer+1,4);
		printf("offer*********\n");
		int suijima = ntohl(*(int*)BUFFER_OFFSET);
		printf("suijima %d\n",suijima);
		unsigned char* CLIENT_MD5 = (unsigned char*)malloc(16);
		if(size == 34)
		{
			memcpy(CLIENT_MD5,buffer + 7 + MULTICASTER_WGID_LEN,16);
			printf("get wgclient md5\n");
			//打印id
			_userId =string((char*)buffer + 7,3);
			unsigned long long ll =ntohll(*(unsigned long long*)(buffer+10));
			char ltos[16] ={0};
			sprintf(ltos,"%lld",ll);
			_userId += string(ltos);
			printf("long long %s\n",ltos);
			printf("userid %s\n",_userId.c_str());
			string _userIds = string((char*)buffer + 7,size - 16 - 7);
			printf("userids %s\n",_userIds.c_str());

			char userid[11] ={0};
			memcpy(userid,(char*)buffer + 7 ,MULTICASTER_WGID_LEN);
			if(userid[0] == 'G' && userid[1] == 'W'){
				printf("*********\n");  //服务器端生成md5G
				create_md5((unsigned char*)_userIds.c_str(),MULTICASTER_WGID_LEN,BUFFER_MD5,_sysmiyue);
			}
			//打印md5
			printf("server md5:");
			for(int i =0;i<16;i++)
			{
				printf("%02x",BUFFER_MD5[i]);
			}
			printf("\n");
			printf("client md5:");
			for(int i =0; i<16;i++){
				printf("%02x",(unsigned)CLIENT_MD5[i]);
			}
			printf("\n");
		}

		if(size == 36){

			memcpy(CLIENT_MD5,buffer + 7 + MULTICASTER_APP_LEN,16);
			printf("get appclient md5\n");

			//打印id
			_userId = string((char*)buffer + 7,3);
			printf("get 3 usr\n");
			if(buffer[7] == 'S'){
				printf("login id is sys\n");
			unsigned long long ll =ntohll(*(unsigned long long*)(buffer+10));
			char ltos2[16] ={0};
			sprintf(ltos2,"%lld",ll);
			_userId += string(ltos2);

			printf("long long is %lld\n",ll);
			}
			else if(buffer[7] == 'U'){
			printf("login id is usr\n");
			unsigned short kk =ntohs(*(unsigned short *)(buffer+10));
			printf("kk is %d\n",kk);
			char ltos3[16] ={0};
			sprintf(ltos3,"%d",kk);
			_userId += string(ltos3);

			unsigned long long ll =ntohll(*(unsigned long long*)(buffer+12));
			char ltos2[16] ={0};
			sprintf(ltos2,"%lld",ll);
			_userId += string(ltos2);
			printf("long long is %lld\n",ll);
			}

			/*unsigned short kk =ntohs(*(unsigned short *)(buffer+10));
			printf("kk is %d\n",kk);
			char ltos3[16] ={0};
			sprintf(ltos3,"%d",kk);
			_userId += string(ltos3);

			unsigned long long ll =ntohll(*(unsigned long long*)(buffer+12));
			char ltos2[16] ={0};
			sprintf(ltos2,"%lld",ll);
			_userId += string(ltos2);

			printf("long long is %lld\n",ll);*/
			printf("userid is %s\n",_userId.c_str());
			string _userIds = string((char*)buffer + 7,size - 16 - 7);
			printf("userids is %s\n",_userIds.c_str());

			char userid[13] ={0};
			memcpy(userid,(char*)buffer + 7 ,MULTICASTER_APP_LEN);
			if(userid[0] == 'U' && userid[1] == 'S'){
				printf("*********app\n");  //服务器端生成md5
				create_md5((unsigned char*)_userIds.c_str(),MULTICASTER_APP_LEN,BUFFER_MD5,_appmiyue);
			}
			else if(userid[0] == 'S' && userid[1] =='Y'){
				printf("********sys\n"); //系统生成md5
				create_md5((unsigned char*)_userIds.c_str(),MULTICASTER_APP_LEN,BUFFER_MD5,_sysmiyue);
			}

			//打印md5
			printf("server md5:");
			for(int i =0;i<16;i++)
			{
				printf("%02x",BUFFER_MD5[i]);
			}
			printf("\n");
			printf("client md5:");
			for(int i =0; i<16;i++){
				printf("%02x",(unsigned)CLIENT_MD5[i]);
			}
			printf("\n");
		}

		printf("_fp----%d\n",_fp);
		conntable* c =TcpServer::_tables[_fp];
		printf("-----------------suijima\n");
		//判断随机码
		int random;
		map<int,conntable*>:: iterator ot = NetBase::_tables.begin();
		map<int,conntable*>::iterator next;
		while(ot !=NetBase::_tables.end())
		{
			next = ot;
			next++;
			//printf("tables size is %ld\n",NetBase::_tables.size());
			ot->second->getRandom(&random);
			if(suijima == random)
			{
				printf("random is same\n");
				NetBase::_tables.erase(_fp);
				NetBase::closes(_fp);
				return;
			}
			ot = next;
		}

		c->setRandom(suijima);
		printf("stat--------------\n");
		c->setStat(1);
		printf("id---------------\n");
		c->setId(_userId);
		printf("time--------------\n");
		c->setTime(time(NULL));
		// }
		printf("time over------------\n");
		printf("_userId is %s\n\n\n\n",_userId.c_str());
		map<string, MulticasterClient *>::iterator it	= _clients.find(_userId);
		if (it != _clients.end() && it->second != this)
			delete it->second;
		_clients[_userId] = this;
}
//多播命令
else if (buffer[0] == MULTICASTER_OPCODE_MULTICAST)
{
	printf("duobo-----------------------------------------\n");
	int stat;
	conntable* c =TcpServer::_tables[_fp];
	c->setTime(time(NULL));
	printf("time change %lu\n",time(NULL));
	c->getStat(&stat);
	if(stat != 1)
	{
		printf("duobo stat is error\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	MulticasterParser::multicast(buffer, size);
	MulticasterRelayTo::relay(buffer, size);
	MulticasterRelayFrom::relay(buffer, size);
}
//广播命令
else if(buffer[0] == MULTICASTER_OPCODE_BOARDCAST)
{
	MulticasterParser::boardcast(buffer, size);

	MulticasterRelayTo::relay(buffer, size);
	MulticasterRelayFrom::relay(buffer, size);
}
//密钥增加包
else if(buffer[0] == MULTICASTER_OPCODE_HEARTBEAT)
{
	int stat;
	char* id = (char*)malloc(20);
	conntable* c = TcpServer::_tables[_fp];
	c->getStat(&stat);
	c->getId(id);
	if(stat != 1)
	{
		printf("add miyue stat error\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	if(id[0] != 'S')
	{
		printf("id is not system\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	//写map
	string _otId;
	string _otKey;
	int idlen = ntohs(*(int *)(buffer + 1));
	printf("idlen %d\n",idlen);
	_otId = string((char*)buffer+5,idlen);
	printf("otid %s\n",_otId.c_str());

	map<string, string>::iterator at = _otlist.find(_otId);
	if (at != _otlist.end())
	{
		printf("the opcompany had already key\n");
		return;
	}

	int keylen = ntohs(*(int *)(buffer+5+idlen));
	printf("keylen %d\n",keylen);
	_otKey = string((char*)buffer+9+idlen,keylen);
	printf("otkey %s\n",_otKey.c_str());
	NetBase::_otlist[_otId]=_otKey;
	//写文件
	ofstream in;
	in.open("otlist.txt",ios_base::app);
	in<<_otId<<"\t"<<_otKey<<"\n";
	printf("write txt\n");
	in.close();
	free(id);
}

//修改密钥包
else if(buffer[0] == MULTICASTER_OPCODE_CHANGE)
{
	int stat;
	char* id = (char*)malloc(20);
	conntable* c = TcpServer::_tables[_fp];
	c->getStat(&stat);
	c->getId(id);
	if(stat != 1)
	{
		printf("change miyue stat error\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	if(id[0] != 'S')
	{
		printf("id is not system\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	//改map
	string _otId;
	string _otKey;
	int idlen = ntohs(*(int *)(buffer + 1));
	printf("idlen %d\n",idlen);
	_otId = string((char*)buffer+5,idlen);
	printf("otid %s\n",_otId.c_str());

	int keylen = ntohs(*(int *)(buffer+5+idlen));
	printf("keylen %d\n",keylen);
	_otKey = string((char*)buffer+9+idlen,keylen);
	printf("otkey %s\n",_otKey.c_str());

	map<string, string>::iterator at = _otlist.find(_otId);
	if (at != _otlist.end())
	{ 
		_otlist.erase(at);
		_otlist[_otId] = _otKey;
	} 
	//改文件
	ofstream out("otlist.txt",ios::out);
	map<string,string>::iterator qt;
	for(qt=_otlist.begin() ; qt != _otlist.end();qt++)
	{
		out<<qt->first<<"\t"<<qt->second<<"\n";
	}
	printf("write txt\n");
	out.close();
	free(id); 
}

//删除密钥
else if(buffer[0] == MULTICASTER_OPCODE_DELETE)
{
	int stat;
	char* id = (char*)malloc(20);
	conntable* c = TcpServer::_tables[_fp];
	c->getStat(&stat);
	c->getId(id);
	if(stat != 1)
	{
		printf("change miyue stat error\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	if(id[0] != 'S')
	{
		printf("id is not system\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	//删map
	string _otId;
	int idlen = ntohs(*(int *)(buffer + 1));
	printf("idlen %d\n",idlen);
	_otId = string((char*)buffer+5,idlen);
	printf("otid %s\n",_otId.c_str());

	map<string, string>::iterator at = _otlist.find(_otId);
	if (at != _otlist.end())
	{ 
		_otlist.erase(at);
		printf("delete succese\n");
	} 
	//删文件

	ofstream out("otlist.txt",ios::out);
	map<string,string>::iterator qt;
	for(qt=_otlist.begin(); qt != _otlist.end();qt++)
	{
		out<<qt->first<<"\t"<<qt->second<<"\n";
	}
	printf("delete txt\n");
	out.close();
	free(id);  
}

//查看密钥 
else if(buffer[0] == MULTICASTER_OPCODE_SELECT)
{
	int stat;
	char* id = (char*)malloc(20);
	conntable* c = TcpServer::_tables[_fp];
	c->getStat(&stat);
	c->getId(id);
	if(stat != 1)
	{
		printf("select miyue stat error\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	if(id[0] != 'S')
	{
		printf("id is not system\n");
		NetBase::_tables.erase(_fp);
		NetBase::closes(_fp);
		return;
	}
	//查map
	int i =1;
	if(_otlist.size() <= 0)
	{
		printf("map is enpty\n");
		return;
	}   
	map<string, string>::iterator at = _otlist.begin();
	while(at != _otlist.end())
	{ 
		printf("%d company %s key is %s\n",i,at->first.c_str(),at->second.c_str());
		at++;
		i++;
	}
	free(id);
}
//返回时间接口，不知道有没有用
else if(buffer[0] == MULTICASTER_OPCODE_TIME)
{
	printf("time packet\n");
	unsigned char* buffer = NULL;
	time_t servertime = time(NULL);
	long servertime1 =(long)servertime;
	servertime1 = htonl(servertime1);
	printf("servertime is %ld\n",servertime1);
	unsigned char b[9]={0};
	b[0] = 0x08;
	memcpy(b+1,(char*)&servertime1,sizeof(servertime1));
	int len = buff(b,9,&buffer);
	::send(_fp,buffer,len,0);
}
//后台获取服务器信息消息
else if(buffer[0] == MULTICASTER_OPCODE_SYS)
{
	conntable* c =TcpServer::_tables[_fp];
	c->setTime(time(NULL));
	printf("time change %lu\n",time(NULL));	
	double cpu = getCpuRate();  //cpu 
	int conn = NetBase::_tables.size();
	int app = 0;
	int wg = 0;
	int len = *(int*)(buffer+1);
	unsigned char* server_name= (unsigned char*)buffer+5;
	printf("len is %d server_name is %s\n",len,server_name);
	/*string ip;
	//公网ip
	getPublicIp(ip); 
	printf("ip: %s\n", ip.c_str());
	char *cip = NULL;
	const char *dip =ip.c_str();  
	cip = strtok((char*)dip,":");
	printf("%s\n",cip);
	printf("%d\n",strlen(cip));
	 */
	//app和wg数量
	printf("clients size is %d\n",_clients.size());
	map<string,MulticasterClient *>::iterator user = _clients.begin();
	while(user != _clients.end())
	{
		string ap = "USR";
		string gw = "GW";
		string ukey = user->first;
		printf("ukey is %s\n",ukey.c_str());
		if(ukey.compare(0,3,ap,0,3) == 0)
		{
			app++;
		}
		if(ukey.compare(0,2,gw,0,2) == 0)
		{
			wg++;
		}
		user++;
	}
	//len = strlen(cip);
	conn =htonl(conn);
	app =htonl(app);
	wg = htonl(wg);
	int len1 =htonl(len);
	unsigned char b[40]={0};
	b[0] = 0x08;
	printf("cpu is %f conn is %d app is %d wg is %d namelen is %d\n",cpu,ntohl(conn),ntohl(app),wg,len);
	printf("buffer -------\n");
	memcpy(b+1,(char*)&len1,sizeof(len));
	printf("namelen -------\n");
	memcpy(b+1+sizeof(len),server_name,len);
	printf("cip -------\n");
	memcpy(b+1+sizeof(len)+len,(char*)&conn,sizeof(conn));
	printf("conn -------\n");
	memcpy(b+1+sizeof(len)+len+sizeof(conn),(char*)&app,sizeof(app));
	printf("app -------\n");
	memcpy(b+1+sizeof(len)+len+sizeof(conn)+sizeof(app),(char*)&wg,sizeof(wg));
	printf("wg -------\n");
	memcpy(b+1+sizeof(len)+len+sizeof(conn)+sizeof(app)+sizeof(wg),(char*)&cpu,sizeof(cpu));
	printf("cpu -------\n");
	unsigned char* sendbuffer = NULL;
	len +=25;
	int lens = buff(b,len,&sendbuffer);
	::send(_fp,sendbuffer,lens,0);        
}		
//返回给后台程序 
else if(buffer[0] == 0x10)
{
	if(buffer[1] == 0x01)
	{
		short num = 0;
		int pos = 0;
		char sender[1024] = {0};
		sender[0] = 0x01;
		pos += 3;
		printf("所有用户\n");
		//获取app id发送回去
		map<string,MulticasterClient *>::iterator user = _clients.begin();
		while(user != _clients.end())
		{
			if(num == 50){
				memcpy(sender+1,(char*)&num,2);
				unsigned char* sendbuffer = NULL;
				int lens = buff((unsigned char*)sender,pos,&sendbuffer);
				::send(_fp,(char*)sendbuffer,lens,0);
				//发送数据
				memset(sender,0,sizeof(sender));
				sender[0] = 0x01;
				num = 0;
				pos = 3;
			}
			string ap = "USR";
			string ukey = user->first;
			printf("ukey is %s\n",ukey.c_str());
			if(ukey.compare(0,3,ap,0,3) == 0)
			{
				short idlen = ukey.size();
				char* id = (char*)ukey.c_str();
				printf("idlen is %d,id is %s\n",idlen,id);
				memcpy(sender+pos,(char*)&idlen,2);
				pos += 2;
				memcpy(sender+pos,id,idlen);
				pos += idlen;	
				num++;
			}
			user++;
		}
		//发送一个完毕的包
		sender[0] = 0x02;
		memcpy(sender+1,(char*)&num,2);
		unsigned char* sendbuffer = NULL;
		int lens = buff((unsigned char*)sender,pos,&sendbuffer);
		::send(_fp,sendbuffer,lens,0);
	}
	else if(buffer[1] == 0x02)
	{
		printf("所有管理员\n");
		//获取网关 id 发送回去
		short num = 0;
		int pos = 0;
		unsigned char sender[1024] = {0};
		sender[0] = 0x03;
		pos += 3;
		printf("所有用户\n");
		map<string,MulticasterClient *>::iterator user = _clients.begin();
		while(user != _clients.end())
		{
			if(num == 50){
				memcpy(sender+1,(char*)&num,2);
				unsigned char* sendbuffer = NULL;
				int lens = buff(sender,pos,&sendbuffer);
				::send(_fp,sendbuffer,lens,0);
				//发送数据
				memset(sender,0,sizeof(sender));
				sender[0] = 0x03;
				num = 0;
				pos = 3;
			}
			string gw = "GW";
			string ukey = user->first;
			printf("ukey is %s\n",ukey.c_str());
			if(ukey.compare(0,3,gw,0,3) == 0)
			{
				short idlen = ukey.size();
				char* id = (char*)ukey.c_str();
				printf("idlen is %d,id is %s\n",idlen,id);
				memcpy(sender+pos,(char*)&idlen,2);
				pos += 2;
				memcpy(sender+pos,id,idlen);
				pos += idlen;	
				num++;
			}
			user++;
		}
		//发送一个完毕的包
		sender[0] = 0x04;
		memcpy(sender+1,(char*)&num,2);
		unsigned char* sendbuffer = NULL;
		int lens = buff(sender,pos,&sendbuffer);
		::send(_fp,sendbuffer,lens,0);
	}	
	}
}
void MulticasterClient::onClose()
{
	map<string, MulticasterClient *>::iterator it	= _clients.find(_userId);
	if(it != _clients.end())
		_clients.erase(it);   
	TcpServerClient::onClose();
}

MulticasterRelayFrom::MulticasterRelayFrom(const char *server, unsigned short port, int fp):TcpServerClient(server, port, fp)
{
	_froms.push_back(this);
}

MulticasterRelayFrom::~MulticasterRelayFrom()
{
	deque<MulticasterRelayFrom *>::iterator it	= find(_froms.begin(), _froms.end(), this);
	if(it != _froms.end())
		_froms.erase(it);
}

void MulticasterRelayFrom::onPacket(unsigned char *buffer, unsigned int size)
{
	//多播
	if(buffer[0] == MULTICASTER_OPCODE_MULTICAST)
	{
		MulticasterRelayTo::relay(buffer, size);
		MulticasterParser::multicast(buffer, size);
	}
	//广播
	else if(buffer[0] == MULTICASTER_OPCODE_BOARDCAST)
	{
		MulticasterRelayTo::relay(buffer, size);
		MulticasterParser::boardcast(buffer, size);
	}
}

void MulticasterRelayFrom::onClose()
{
	//TcpServerClient::onClose();

	delete this;
}

void MulticasterRelayFrom::relay(unsigned char *buffer, unsigned int size)
{
	if(_froms.size())
	{
		TcpPacket141 *packet	= new TcpPacket141(buffer, size);
		if(packet)
		{
			packet->addRef();
			for(deque<MulticasterRelayFrom *>::iterator it = _froms.begin(); it != _froms.end(); it++)
			{
				(*it)->send(packet);
			}
			packet->release();
		}
	}
}

MulticasterRelayTo::MulticasterRelayTo(const char *server, unsigned short port):TcpClient(server, port)
{
	_tos.push_back(this);
}

MulticasterRelayTo::~MulticasterRelayTo()
{
	deque<MulticasterRelayTo *>::iterator it	= find(_tos.begin(), _tos.end(), this);
	if(it != _tos.end())
		_tos.erase(it);
}

void MulticasterRelayTo::onPacket(unsigned char *buffer, unsigned int size)
{
	//多播
	if(buffer[0] == MULTICASTER_OPCODE_MULTICAST)
	{
		MulticasterParser::multicast(buffer, size);
		MulticasterRelayFrom::relay(buffer, size);
	}
	//广播
	else if(buffer[0] == MULTICASTER_OPCODE_BOARDCAST)
	{
		MulticasterParser::boardcast(buffer, size);
		MulticasterRelayFrom::relay(buffer, size);
	}
}

void MulticasterRelayTo::onClose()
{
	TcpClient::onClose();

	delete this;
}

void MulticasterRelayTo::relay(unsigned char *buffer, unsigned int size)
{
	if(_tos.size())
	{
		TcpPacket141 *packet = new TcpPacket141(buffer, size);
		if(packet)
		{
			packet->addRef();
			for(deque<MulticasterRelayTo *>::iterator it = _tos.begin(); it != _tos.end(); it++)
			{
				(*it)->send(packet);
			}
			packet->release();
		}
	}
}
string MulticasterRelayTo::getClassName(){

	return string("MulticasterRelayTo");
}

string MulticasterRelayFrom::getClassName(){

	return string("MulticasterRelayFrom");
}
