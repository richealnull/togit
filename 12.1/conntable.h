#ifndef __CONNTABLE_H__
#define __CONNTABLE_H__
#include <time.h>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

class conntable{

public:
    conntable();
    conntable(int fd);
    void setRandom(int random);
    void setFd(int fd);
    void setStat(int stat);
    void setTime(time_t Time);
    void setId(string id);
    void getRandom(int* random);
    void getFd(int* fd);
    void getStat(int* stat);
    void getTime(time_t* Time);
    void getId(string id);
private:
    int random;
    int fd;
    int stat;
    time_t Time;
    string id;
};
#endif
