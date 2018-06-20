#include <unistd.h>
#include <pthread.h>
#include "tcp_server.h"
#include "multicaster.h"
#include "command_option.h"
#include "log.h"
#include "utility.h"

void* pthread_func(void* ptr)
{
    while(1){
        printf("pthread time-------------- \n");
        {
        Lock(NetBase::tables_mutex);
        map<int,conntable*>::iterator it =NetBase::_tables.begin();
        map<int,conntable*>::iterator next;
        while(it != NetBase::_tables.end())   //遍历连接表
        {
            next = it;
            next++;
            
            time_t time2;
            int fd;
            int stat;
            printf("getTime before \n");
            it->second->getTime(&time2);  
            printf("getTime after \n");
            it->second->getFd(&fd);
            printf("time is %lu\n",time2);
            printf("fd is %d  ",fd);
            it->second->getStat(&stat);
            printf("stat is %d\n",stat);
            if(stat == -1)
            {
                NetBase::_tables.erase(fd);
                printf("close table[fd]\n");
                NetBase::closes(fd);
                printf("close fd %d\n",fd);                              
            }

            if((time(NULL)-time2) > 200 && stat == 1)
            {
                char* id =(char*)malloc(20);
                it->second->getId(id);
                //删除链接表_tables中的实例
                NetBase::_tables.erase(it);
                printf("close table[fd]\n");
                //删除_clients中的实例
                printf("close clients[id]\n");
                /*map<string, MulticasterClient *>::iterator ot=MulticasterClient::_clients.find(id);
                if(ot != MulticasterClient::_clients.end())
                {
                    MulticasterClient::_clients.erase(ot);   
                }*/
               //删除_bases实例
                NetBase::closes(fd);
                printf("close fd %d\n",fd);
                free(id);
                
            }
          it = next;  
        }
        }
          sleep(30);
    }

}

//启动参数：监听客户端端口
int handleOptionDaemon(char *argStr)
{
    const char *ip      = "127.0.0.1";
    unsigned short port = 9002;

    char *args[2]   = {0};
    int size    = strsplit(argStr, ":", args, 2);

    if(size == 1)
    {
        port    = atoi(args[0]);
    }
    else if(size == 2)
    {
        ip      = args[0];
        port    = atoi(args[1]);
    }
    else
    {
        return -1;
    }
    //printf("asdasdas\n");
    (new TcpServer(ip, port, new TcpServerHandler<MulticasterClient>))->start();
    return 0;
}

//启动参数：监听其它接线员
int handleOptionFrom(char *argStr)
{
    const char *ip      = "127.0.0.1";
    unsigned short port = 9002;

    char *args[2]   = {0};
    int size    = strsplit(argStr, ":", args, 2);

    if(size == 1)
    {
        port    = atoi(args[0]);
    }
    else if(size == 2)
    {
        ip      = args[0];
        port    = atoi(args[1]);
    }
    else
    {
        return -1;
    }
    (new TcpServer(ip, port, new TcpServerHandler<MulticasterRelayFrom>))->start();
    return 0;
}

//启动参数：连接其它接线员
int handleOptionTo(char *argStr)
{
    const char *ip      = "127.0.0.1";
    unsigned short port = 9002;

    char *args[2]   = {0};
    int size    = strsplit(argStr, ":", args, 2);

    if(size == 1)
    {
        port    = atoi(args[0]);
    }
    else if(size == 2)
    {
        ip      = args[0];
        port    = atoi(args[1]);
    }
    else
    {
        return -1;
    }

    (new MulticasterRelayTo(ip, port))->start();
    return 0;
}

int main(int argc, char *argv[])
{
    //
   // NetBase::tableMutexInit();
    //注册启动参数处理器
    printf("0000000\n");
    int ret	= (new CommandOption)
        ->registerOption("daemon", true, handleOptionDaemon, "listen daemon(syntax ip:port)")
        ->registerOption("from", true, handleOptionFrom, "relay from(synctax ip:port)")
        ->registerOption("to", true, handleOptionTo, "relay to(synctax ip:port)")
        ->parse(argc, argv);

    pthread_t tid;
    printf("pthread---------star\n");
    int i =  pthread_create(&tid,NULL,pthread_func,NULL);
    printf("pthread--------create%d\n",i);

    if(ret < 0)
    {
        LogError("error arg:%d\n", ret);
        return ret;
    }
    /* char ip_port[21]={0};
       printf("请输入ip:port:");
       scanf("%s",ip_port);
       printf("%s\n",ip_port);
       if(strcmp(ip_port,"000000")!=0){
       handleOptionTo(ip_port);
       }
       */
    while(1)
    {
        //LogError("aaaaaaaaaaaaaaaaaaa\n");
        //printf("111111111\n");
        NetBase::slice();
    }

    pthread_exit( NULL);
	//return 0;
}
