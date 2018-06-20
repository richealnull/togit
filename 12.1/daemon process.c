#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

#define MAXFILE 65535
//守护进程
int main(){

    pid_t pc,pid;

    int i, fd, len;

    char* buf="this is a Dameon\n";

    len = strlen(buf);

    pc = fork();
    if(pc < 0){
        printf("error fork\n");
        exit(1);
    }
    else if(pc > 0){
      exit(0);
    }

    setsid();

    pid = fork();

    if(pid < 0){
       perror("fork error\n");
    }


    if(pid > 0){
       exit(0);
    }

    chdir("/");

    umask(0);
    
    for(i = 2 ; i < MAXFILE ; i++){
       close(i);
    }
    

    int logfd = open("/root/12.1/a.log",O_WRONLY | O_APPEND);

    printf("logfd :: %d\n",logfd);
    int ret = dup2(logfd,STDOUT_FILENO);
    if(ret == -1)
	{
		printf("dup2 error");
	}
    
    execlp("/root/12.1/multicaster","multicaster","-d0.0.0.0:10000","-f0.0.0.0:10001",NULL);
    
    //while(1);
}
