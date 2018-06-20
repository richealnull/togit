#include "net_base.h"
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "log.h"
#include "utility.h"

int NetBase::_fpEpoll = -1;

map<int, NetBase *> NetBase::_bases;
map<int, conntable*> NetBase::_tables;
map<string, string> NetBase::_otlist;

pthread_mutex_t NetBase::tables_mutex;
pthread_mutexattr_t NetBase::tables_mutexattr;

NetBase::NetBase()
{
    if(_fpEpoll < 0)
        _fpEpoll	= epoll_create(65535);

    _packet	= 0;
    _pos	= 0;
}

NetBase::~NetBase()
{
    printf("~~~~~~~~~~netbase\n");

    close();
}

void NetBase::start()
{
    struct epoll_event ev = {0};
    ev.data.fd	= _fp;
    ev.events	= EPOLLIN | EPOLLET;
    epoll_ctl(_fpEpoll, EPOLL_CTL_ADD, _fp, &ev);
    
    _bases[_fp]	= this;
}

void NetBase::send(Packet *packet)
{
    packet->addRef();

    _packets.push_back(packet);

    struct epoll_event ev = {0};
    ev.data.fd	= _fp;
    ev.events	= EPOLLIN | EPOLLET | EPOLLOUT;
    epoll_ctl(_fpEpoll, EPOLL_CTL_MOD, _fp, &ev);
}

void NetBase::onRecv()
{
}

void NetBase::onSend()
{
    if(!_fp)
        return;

    while(_packets.size() || _packet)
    {
        if(!_packet)
        {
            _packet	= _packets.front();
            _packets.pop_front();
            _pos	= 0;
        }
        int ret	= onSend(_packet->data() + _pos, _packet->length() - _pos, _packet);
        if(ret <= 0)
        {
            if(ret < 0 && EAGAIN != errno)
            {
                LogError("NetBase onSend %d error:%s\n", _fp, strerror(errno));
                close();
                return;
            }
            break;
        }
        _pos	+= ret;
        if(_pos >= _packet->length())
        {
            _packet->release();
            _packet	= 0;
            _pos	= 0;
        }
    }

    struct epoll_event nev = {0};
    nev.data.fd	= _fp;
    nev.events	= EPOLLIN | EPOLLET;
    if(_packets.size() || _packet)
        nev.events	|= EPOLLOUT;
    epoll_ctl(_fpEpoll, EPOLL_CTL_MOD, _fp, &nev);
}

void NetBase::closes(int fd)
{
    if(fd)
    {
        ::close(fd);
        printf("close fd %d\n",fd);
        _bases.erase(fd);
        fd = 0;
    }
}

void NetBase::onClose()
{
    LogInfo("onClose %d.\n", _fp);

    if(_fp)
    {
        struct epoll_event ev = {0};
        ev.data.fd	= _fp;
        ev.events	= EPOLLIN | EPOLLET | EPOLLOUT;
        epoll_ctl(_fpEpoll, EPOLL_CTL_DEL, _fp, &ev);

        ::close(_fp);
        printf("~netbase erase bases\n");
        _bases.erase(_fp);
	NetBase::_tables.erase(_fp);
        _fp		= 0;
    }

    for(deque<Packet *>::iterator it = _packets.begin(); it != _packets.end(); it++)
    {
        (*it)->release();
    }
    _packets.clear();

    if(_packet)
    _packet->release();
    _packet	= 0;
    _pos	= 0;
}

void NetBase::close()
{
    onClose();
}

void NetBase::slice()
{
    static struct epoll_event events[1024 * 100];
    int size	= _bases.size();
    if (size > 1024 * 100)
        size	= 1024 * 100;
    int eventcount	= epoll_wait(_fpEpoll, events, size, 100);
    for (int i = 0; i < eventcount; i++)
    {
        struct epoll_event *ev	= events + i;

        if(ev->data.fd < 0)
            continue;

        NetBase *base = _bases[ev->data.fd];
        
        if(!base)
            continue;

        if(ev->events & EPOLLIN)
        {
            base->onRecv();
        }
        if(ev->events & EPOLLOUT)
        {
            base->onSend();
        }
        
    }
}
string NetBase::getClassName(){

    return string("NetBase");
}
void tableMutexInit(){
    pthread_mutexattr_init(&NetBase::tables_mutexattr);
    pthread_mutexattr_settype(&NetBase::tables_mutexattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&NetBase::tables_mutex,&NetBase::tables_mutexattr);

}


