#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include "md5.h"
#include <sys/types.h>
#include <time.h>
#define SHORT_LEN 2
#define NAME_HEAD_LEN 3
#define LONG_LEN 8
#define NAME_LEN 13

static const unsigned char TCPPROCOTOL_STATE_HEAD = 0;
static const unsigned char TCPPROCOTOL_STATE_LENGTH = 1;
static const unsigned char TCPPROCOTOL_STATE_CSLENGTH = 2;
static const unsigned char TCPPROCOTOL_STATE_BODY = 3;

char* _miyue = "bbccdd";
// n to h
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

//h to n
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

//封包函数，141+内容
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


int main(){
    int nRecvBuf=38*1024;//设置为32K
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(clientfd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

    if(clientfd == -1){
        printf("socket error\n");
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int cret = connect(clientfd,(struct sockaddr*)&addr,sizeof(addr));
    unsigned char b[40]={0};
    b[0] = 0x01;
    time_t  time1 = time(NULL);
    srand(time1);
    int suijima= rand();
    printf("rand %d\n",suijima);
    suijima = htonl(suijima);
    printf("rand2 %d\n",suijima);
    printf("2222222222222\n");
    memcpy(b+1,(char*)&suijima,sizeof(suijima));
    printf("3333333333333\n");


    char* heat ="USR";
    char name[13] ={0} ;
    short nlen=13;
    unsigned long long name3 = 84094263265406351;
    short s = 95;
    printf("long long 1 is %lld\n",name3);
    printf("short is %d\n",s);
    nlen = htonl(nlen);
    s = htonl(s);
    name3 = htonll(name3);
    memcpy(name,heat,3);
    memcpy(name + NAME_HEAD_LEN,(char*)&s,sizeof(s));
    memcpy(name + NAME_HEAD_LEN+2,(char*)&name3,LONG_LEN);
    memcpy(b+1+sizeof(suijima),(char* )&nlen,SHORT_LEN);
    //printf("name is %s\n",name);
    unsigned char* buffer=NULL;
    unsigned char* buffer2 =(unsigned char*)malloc(16);
    int len2 = create_md5((unsigned char*)name,NAME_LEN,buffer2,_miyue);

    printf("md5:");
    for(int i =0;i<16;i++)
    {
        printf("%02x",buffer2[i]);
    }
    printf("\n");

    memcpy((char*)b + 1 + sizeof(suijima) + SHORT_LEN,name,NAME_LEN);
    memcpy((char*)b + 1 + sizeof(suijima) + SHORT_LEN + NAME_LEN,buffer2,len2);
    int length = 1+sizeof(suijima) + SHORT_LEN + NAME_LEN + len2;
    int len = buff(b,length,&buffer);
    printf("%d\n",len);
    printf("%d\n",len2);
    if(cret == -1){

        perror("connect:");
        printf("%d\n",clientfd);
        printf("cret error\n");

    }

    int wret = write(clientfd,buffer,len);
    printf("buffer %s\n",buffer);
    printf("wret %d\n",wret);
    char recvbuff[4006];
    bzero(recvbuff,sizeof(recvbuff));
    FILE* fp = fopen("stl.7z","wb+");
    if(fp == NULL)
    {
        printf("file a.txt can not open to write\n");
    }
    int ll =0;
    while(ll = recv(clientfd,recvbuff,4006,0))
    {
        printf("ll is %d\n",ll);
        if(ll<0)
        {
            printf("recv data from server  failed!\n");
            break;    
        }
        unsigned int len = 4;
        int datalen = 0;
        memcpy(((unsigned char *)&datalen), recvbuff+1, len);
        datalen = ntohl(datalen);
        printf("datalen is %d\n",datalen);
        while(ll<datalen)
        {
            char recvbuff2[datalen-ll];
            bzero(recvbuff2,sizeof(recvbuff2));
            int re = recv(clientfd,recvbuff2,datalen-ll,0);
            memcpy(recvbuff+ll,recvbuff2,re);
            ll += re;
        }
        
        int write_length = fwrite(recvbuff+6,sizeof(char),ll-6,fp);
        printf("write is %d\n",write_length);
        if(write_length < ll-6)
          {
          printf("file: a.txt write failed\n");
          break;
          }
          
        bzero(recvbuff,4006);
        fflush(fp);
    }
    fclose(fp);
    close(clientfd);
    return 0;
}
