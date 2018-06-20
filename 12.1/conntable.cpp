#include "conntable.h"
#include <stdlib.h>


     conntable::conntable(){
         fd = -1;
         random = -1;
         stat = -1;
         Time = -1;
         id = "";

     }
     conntable::conntable(int fd){
         this->fd = fd;
         random = -1;
         stat = -1;
         Time = -1;
         id = "";
     }
     void conntable::setRandom(int random){
         this->random = random;
     }
     void conntable::setFd(int fd){
         this->fd = fd;
     }
     void conntable::setStat(int stat){
         this->stat = stat;
     }
     void conntable::setTime(time_t Time){
         this->Time=Time;
     }
     void conntable::setId(string id){
         if(id != ""){
             this->id = id;
         }
     }
     void conntable::getRandom(int* random){
         *random = this->random;
     }
     void conntable::getFd(int* fd){
         *fd = this->fd;
     }
     void conntable::getStat(int* stat){
         *stat = this->stat;
     }
     void conntable::getTime(time_t* Time){
         *Time = this->Time;
     }
     void conntable::getId(string id){
          if(id != ""){
              id = this->id;
          }
     }
