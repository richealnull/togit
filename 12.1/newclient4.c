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

#define SHORT_LEN 2
#define NAME_HEAD_LEN 3
#define LONG_LEN 8
#define NAME_LEN 13



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
    int _length = 6 + size;
    unsigned char* _buffer = (unsigned char *)malloc(_length);
    memset(_buffer,0,_length);
    _buffer[0]  = 0xaa;
    *(unsigned int *)(_buffer + 1)  = htonl(_length);
    _buffer[5]  = 0 - _buffer[0] - _buffer[1] - _buffer[2] - _buffer[3] - _buffer[4];
    memcpy(_buffer + 6, buffer, size);
    *_buff = _buffer;
    return _length;
}
//发送时间包

int main(){
    int nSendBuf=38*1024;//设置为32K
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(clientfd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

    if(clientfd == -1){
        printf("socket error\n");
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr=inet_addr("0.0.0.0");

    int cret = connect(clientfd,(struct sockaddr*)&addr,sizeof(addr));
    unsigned char b[5]={0};
    b[0] =0x08
    if(cret == -1){

        perror("connect:");
        printf("%d\n",clientfd);
        printf("cret error\n");

    }

    int wret = write(clientfd,buffer,len);
    printf("buffer %s\n",buffer);
    printf("wret %d\n",wret);
    unsigned char* sendbuff=(unsigned char*)calloc(4000,sizeof(unsigned char));
    FILE* fp = fopen("stl.7z","rb+");
    if(fp == NULL)
    {
        printf("file a.txt can not open to write\n");
    }
    printf("send-----------\n");
    int ll = 0;
    while((ll = fread(sendbuff,sizeof(unsigned char),4000,fp))>0)
    {
        unsigned char* c=NULL;
        unsigned char* buf=NULL;
        int length2 = 0;
        printf("ll =%d\n",ll);
        printf("fengzhuang----\n");
        length2 = fengzhuang(&c,ll,sendbuff);
        printf("len2 = %d\n",length2);
        printf("buff----------\n");
        int lens = buff(c,length2,&buf);
        printf("buff len %d\n",lens);
        if(send(clientfd,buf,lens,0)<0)
        {
            printf("send file fail\n");
        }
        printf("----------\n");
        memset(sendbuff,0,4000);
    }
    fclose(fp);
    close(clientfd);
    return 0;
}
