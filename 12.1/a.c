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
int main(){
    int clientfd = socket(AF_INET,SOCK_STREAM,0);

    if(clientfd == -1){
        printf("socket error\n");
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10004);
    addr.sin_addr.s_addr=inet_addr("120.79.24.61");

    int cret = connect(clientfd,(struct sockaddr*)&addr,sizeof(addr));
    unsigned char b[1]={0};
    b[0] = 0x08;
    unsigned char* buffer=NULL;
    int len = buff( b,1,&buffer);
    if(cret == -1){

        perror("connect:");
        printf("%d\n",clientfd);
        printf("cret error\n");

    }
   
    while(1)
    {
        int wret = write(clientfd,buffer,len);
        printf("buffer %s\n",buffer);
        printf("wret %d\n",wret);
        char buff[100]={0};
        int rret=read(clientfd,buff,100);
        long servertime = *(long*)(buff);
        printf("rret %d  buff  %ld\n",rret,servertime);
        if(cret == -1)
        {
            perror("error:");
            printf("%d\n",clientfd);
        }
        getchar();
        sleep(3);

    }
  
    return 0;
}
