#ifndef UTILITY_H
#define UTILITY_H

#include <pthread.h>
#include <functional>
#include <string>
#include <deque>
#include <unistd.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <unistd.h>  

#include <netdb.h>  
#include <net/if.h>  
#include <arpa/inet.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  

using namespace std;
#define STLFOR(cur, next, list) for(cur = next = (list).begin(); (list).size() && ((cur = next++) != (list).end());)

#define Lock(mutex) Auto_Lock lock(mutex)

struct Auto_Lock
{
    Auto_Lock(pthread_mutex_t &m):mutex(m)
    {
        pthread_mutex_lock(&mutex);
        printf("lock---------------------\n");
    }

    ~Auto_Lock()
    {
        pthread_mutex_unlock(&mutex);
        printf("unlock--------------------\n");
    }

    pthread_mutex_t &mutex;
};

//cpu结构体
typedef struct CPU_PACKED         //定义一个cpu occupy的结构体  
{  
    char name[20];             //定义一个char类型的数组名name有20个元素  
    unsigned int user;        //定义一个无符号的int类型的user  
    unsigned int nice;        //定义一个无符号的int类型的nice  
    unsigned int system;    //定义一个无符号的int类型的system  
    unsigned int idle;         //定义一个无符号的int类型的idle  
    unsigned int iowait;  
    unsigned int irq;  
    unsigned int softirq;  
}CPU_OCCUPY; 

//ip结构体
/*
struct ifaddrs   
{   
    struct ifaddrs  *ifa_next;    // Next item in list    
    char            *ifa_name;    // Name of interface    
    unsigned int     ifa_flags;   // Flags from SIOCGIFFLAGS    
    struct sockaddr *ifa_addr;    // Address of interface   
    struct sockaddr *ifa_netmask; // Netmask of interface    
    union   
    {   
        struct sockaddr *ifu_broadaddr; // Broadcast address of interface    
        struct sockaddr *ifu_dstaddr; // Point-to-point destination address    
    } ifa_ifu;   
#define              ifa_broadaddr ifa_ifu.ifu_ifu_broadaddr   
#define              ifa_dstaddr   ifa_ifu.ifu_ifu_dstaddr   
    void            *ifa_data;    // Address-specific data    
}; */

double cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n);
void get_cpioccupy(CPU_OCCUPY *cpust);
double getCpuRate();  //获取cpu
string GetLocalIp();
bool getPublicIp(string& ip);

unsigned long long int getTickCount();
time_t getLocalTime(time_t time, int targetTimezone);
time_t getMorningTime(time_t time);
string fillZero(int time);
string hex2str(unsigned char *buffer, int length);
string hex2rev(unsigned char *buffer, int length);
int strsplit(char *src, const char *delimiters, char **dest, int size);
deque<string> strsplit(string src, const string &delimiters);
void strreplace(string &src, const char *from, const char *to);
void strreplace(string &src, const char *pattern, function<string (string &)> onMatch);
string num2str(int num);

#endif
