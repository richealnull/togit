#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXFILE 65535

int main(){

    
       execlp("./multicaster","multicaster","-d0.0.0.0:10000",NULL);
    
    //while(1);
}
