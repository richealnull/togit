#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//日志清除       

int main(){


    int fd = open("/root/12.1/a.log",O_RDWR,0777);
    ftruncate(fd,0);
    return 0;
}

