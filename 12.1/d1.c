#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    buffer[0] = 0x02;
    len += 1;
    //设置广播标识码

    short ids;//id的数量

    printf("请输入ids大小：");

    scanf("%d",(int *)&ids);

    printf("\n");
    short IDS = htons(ids);
    memcpy(buffer+len,(char* )&IDS,sizeof(ids));

    len +=sizeof(ids);
    //printf("%d\n",ids);

    while(ids !=0 ){
        printf("请输入用户名字:");
        char name[100]={0};
        scanf("%s",name);
        short nlen = htons((short)strlen(name));
        memcpy(buffer+len,(char*)&nlen,sizeof(nlen));

        len += sizeof(nlen);

        strcpy(buffer+len,name);

        len += strlen(name);

        //printf("name = %s len %d\n",name,strlen(name));
        --ids;
    }
    printf("请输入内容:");
    char nr[100]={0};
    scanf("%s",nr);
    int rlen=htonl(strlen(nr));
    memcpy(buffer+len,(char*)&rlen,sizeof(rlen));
    len += sizeof(rlen);
    strcpy(buffer+len,nr);
    len += strlen(nr);
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
    addr.sin_addr.s_addr=inet_addr("114.67.153.62");

    int cret = connect(clientfd,(struct sockaddr*)&addr,sizeof(addr));

    if(cret == -1){

        perror("connect:");
        //printf("%d\n",clientfd);
        printf("cret error\n");

    }
    while(1)
    {
        unsigned char* c=NULL;
        unsigned char* buffer=NULL;
        int length2 = 0;
        fengzhuang(&c,&length2);
        printf("112213123");
        int len = buff( c,length2,&buffer);
        int writ = write(clientfd,buffer,len);
        printf("buffer %s\n",buffer);

        printf("writ %d\n",writ);

        sleep(3);
        getchar();

    }

    return 0;

}
