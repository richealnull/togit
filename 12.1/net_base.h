#ifndef __NET_BASE_H__
#define __NET_BASE_H__
#include <map>
#include <deque>
#include "packet.h"
#include "conntable.h"
#include <pthread.h>
#include <unistd.h>
using namespace std;
class NetBase
{
    private:
        static int _fpEpoll;

        static map<int, NetBase *> _bases;

        deque<Packet *> _packets;

        Packet *_packet;
        unsigned int _pos;
    protected:
        int _fp;
	int _conn;
        virtual void onRecv();
        void onSend();
        virtual int onSend(unsigned char *buffer, unsigned int size, Packet *packet) {return 0;}
        virtual void onClose();
    public:
        static pthread_mutex_t tables_mutex;
        static pthread_mutexattr_t tables_mutexattr;
        void tableMutexInit();
        static map<int,conntable *> _tables;
        static map<string,string> _otlist;
        NetBase();
        virtual ~NetBase();
        void send(Packet *packet);
        void start();
        void close();
        virtual string getClassName();
        static void closes(int fd);

        static void slice();
};

#endif
