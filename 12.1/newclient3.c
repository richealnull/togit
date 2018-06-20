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
//多播包封装函数
int fengzhuang(unsigned char** buff,int length,unsigned char* sendbuff){
    unsigned char* buffer=(unsigned char*)malloc(22+length);
    memset(buffer,0,22+length);
    int len = 0;
    if(buffer == NULL){
        printf("malloc error\n");
    }
    buffer[0] = 0x02;
    len += 1;
    // 设置广播标识码

    short ids = 1;//id的数量

    printf("ids大小1\n");
    short IDS = htons(ids);
    memcpy((char*)buffer+len,(char* )&IDS,sizeof(ids));
    len +=sizeof(ids);
    while(ids !=0 )
    {
        char* heat ="USR";
        unsigned long long name3 = 84094263265406351;
        short s = 95;
        s = htonl(s);
        name3 = htonll(name3);

        printf("用户名字USR");
        unsigned char name[13] = {0};
        short nlen = htons((short)13);

        memcpy(name,heat,3);
        memcpy(name + NAME_HEAD_LEN,(char*)&s,2);
        memcpy(name + NAME_HEAD_LEN+2,(char*)&name3,LONG_LEN);

        memcpy(buffer+len,(char*)&nlen,sizeof(nlen));
        len += sizeof(nlen);
        memcpy(buffer+len,name,13);
        len += 13;
        --ids;
    }
    int rlen=htonl(length);
    memcpy(buffer+len,(char*)&rlen,sizeof(rlen));
    len += sizeof(rlen);
    memcpy(buffer+len,sendbuff,length);
    len += length;
    printf("%d\n",len);
    *buff = buffer;
    return len;
}


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
    unsigned long long name3 = 20179998888;
    short s = 33;
    printf("long long 1 is %lld\n",name3);
    printf("short is %d\n",s);
    nlen = htonl(nlen);
    s = htonl(s);
    name3 = htonll(name3);
    memcpy(name,heat,3);
    memcpy(name + NAME_HEAD_LEN,(char*)&s,sizeof(s));
    memcpy(name + NAME_HEAD_LEN+2,(char*)&name3,LONG_LEN);
    memcpy(b+1+sizeof(suijima),(char* )&nlen,SHORT_LEN);
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
