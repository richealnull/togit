#include "network.h"
#include "md5.h"
#define SHORT_LEN 2
#define NAME_HEAD_LEN 3
#define LONG_LEN 8
#define NAME_LEN 13



char* _miyue = (char*)"bbccdd";

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

void test(int i)
{
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


    char* heat =(char*)"USR";
    char name[13] ={0} ;
    short nlen=13;
    unsigned long long name3 = 84094263265406351+i;
    short s = 95;
    printf("long long 1 is %lld\n",name3);
    nlen = htons(nlen);
    s = htons(s);
    name3 = htonll(name3);
    memcpy(name,heat,3);
    memcpy(name + NAME_HEAD_LEN,(char*)&s,2);
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
	
	
    int fd = myConnect2(10000, "0.0.0.0");
    if(-1 == fd)
    {
        printf("connect error!\n");
        return;
    }
    
    myWrite(fd,(char*)buffer,len);

    close(fd);

    exit(0);
}

int main()
{   
    int i;
    int n = 20000;
    for(i=0;i<n;++i)
    {
        pid_t pid = fork();
        if(pid > 0)
            continue;
        else
            test(i);
    }

    for(i=0;i<n;++i)
        wait(NULL);

    return 0;
}
