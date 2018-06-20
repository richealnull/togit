#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdlib.h>
#include "md5class.h"
#include <sys/types.h>
#include <time.h>
char* _miyue = "aabbcc";
CMD5 md5;

//生成MD5函数，_buff2为传出
int setmd5(unsigned char* _buff,int size,unsigned char** _buff2)
{
    int miyuelen = strlen(_miyue);
    int _length = miyuelen+size;
    unsigned char* _buffer =(unsigned char*)malloc(_length);
    memcpy(_buffer,_miyue,miyuelen);
    memcpy(_buffer+miyuelen,_buff,size);
    md5.setPlainText((char*)_buffer);
    *_buff2 =(unsigned char*)md5.getMD5Digest();
    return 32;
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

void fengzhuang(unsigned char** buff,int* length){
    unsigned char* buffer=(unsigned char*)malloc(100);
    int len = 0;
    if(buffer == NULL){
        printf("malloc error\n");
    }
    buffer[0] = 0x05;
    len += 1;
   // 设置广播标识码 
        printf("\n");
   // printf("%d\n",ids);

    printf("请输入公司id:");
    char name[100]={0};
    scanf("%s",name);
    int nlen = htons((int)strlen(name));
    memcpy(buffer+len,(char*)&nlen,sizeof(nlen));

    len += sizeof(nlen);

    strcpy((char*)buffer+len,name);

    len += strlen(name);

    printf("name = %s len %d\n",name,strlen(name));

    *length = len;
    printf("%d\n",len);
    *buff = buffer;

}



int main(){
    int clientfd = socket(AF_INET,SOCK_STREAM,0);

    if(clientfd == -1){
        printf("socket error\n");
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int cret = connect(clientfd,(struct sockaddr*)&addr,sizeof(addr));
    unsigned char b[42]={0};
    b[0] = 0x01;
    time_t  time1 = time(NULL);
    srand((time1));
    int suijima=rand();
    printf("rand %d\n",suijima);
    suijima = htonl(suijima);
    printf("rand2 %d\n",suijima);

    printf("2222222222222\n");
    memcpy(b+1,(char*)&suijima,sizeof(suijima));
    printf("3333333333333\n");


    char* name = "sssfan4";
    short nlen=(short)strlen(name);
    memcpy(b+1+sizeof(suijima),(char* )&nlen,sizeof(nlen));
    //printf("%d %d\n",nlen,sizeof(nlen));
    strcpy((char*)b+1+sizeof(suijima)+sizeof(nlen),(char*)name);
    unsigned char* buffer=NULL;
    unsigned char* buffer2 =NULL;
    int len2 = setmd5((unsigned char*)name,nlen,&buffer2);
    strcpy((char*)b+1+sizeof(suijima)+sizeof(nlen)+strlen(name),(char*)buffer2);
    int length = 1+sizeof(suijima)+sizeof(nlen)+strlen(name)+len2;
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
        unsigned char* bu =NULL;
        unsigned char* c =NULL;
        int setlen = 0;
        fengzhuang(&c,&setlen);
        printf("---------fengzhung\n");
        int len4 =buff(c,setlen,&bu);
        int wre2 = write(clientfd,bu,len4);
        printf("wret2 %d\n",wre2);
        

    while(1)
    {
        char buff[100]={0};
        int rret=read(clientfd,buff,100);
        printf("rret %d  buff  %s\n",rret,buff+6);
        getchar();
        sleep(3);

    }

    return 0;
}
