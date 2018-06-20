#include "tcp_packet.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "log.h"

void TcpParser141::parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler)
{
    unsigned int pos	= 0;
    while(pos < size)
    {
        if(TCPPROCOTOL_STATE_HEAD == _state)
        {
            if(0xaa == buffer[pos])
            
            {
                int i =0;for(i=0;i<12;i++) {printf(" %x ",buffer[i]);}printf("\n");
                     pos++;
                
                _state	= TCPPROCOTOL_STATE_LENGTH;
                _pos	= 0;
            printf("5555555555555\n");
            }
            else
            {
                LogError("head error:%02x\n", buffer[pos]);
                pos++;
            }
           // printf("head*****%d\n",buffer1[pos]);
        }
        else if(TCPPROCOTOL_STATE_LENGTH == _state)
        {
            unsigned int len	= 4 - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(((unsigned char *)&_length) + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= 4)
            {
                _length	= ntohl(_length);
                _pos	= 0;
                _state	= TCPPROCOTOL_STATE_CSLENGTH;
            printf("666666666666666\n");
            printf("%d\n",buffer[pos]);
            }
           // printf("lenth*****%d\n",buffer1[pos]);
        }
        else if(TCPPROCOTOL_STATE_CSLENGTH == _state)
        {
            unsigned char *buf	= (unsigned char *)&_length;
            if((unsigned char)(buffer[pos] + 0xaa + buf[0] + buf[1] + buf[2] + buf[3]))
            {
                LogError("check head checksum error.\n");
                if(pos < 4)
                    pos	= 0;
                else
                    pos	-= 4;
                _state	= TCPPROCOTOL_STATE_HEAD;
            printf("7777777777777777777\n");
            }
            else
            {
                pos++;
                _pos	= 0;
                _length	-= 6;
                _buffer	= (unsigned char *)malloc(_length);
                if(!_buffer)
                {
                    LogError("out of memory.\n");
                    exit(-2);
                }
                printf("11111111111111\n");
                _state	= TCPPROCOTOL_STATE_BODY;
            }
            //printf("check*****%d\n",buffer1[pos]);
        }
        else if(TCPPROCOTOL_STATE_BODY == _state)
        {
            unsigned int len	= _length - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(_buffer + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= _length)
            {
                printf("222222222\n");
                handler->onPacket(_buffer, _length);
                free(_buffer);
               // free(buffer1);
             //   free(servermd5);
               // free(clienmd5);
                _buffer	= 0;
                _state	= TCPPROCOTOL_STATE_HEAD;
            }
            printf("body*****%d\n",buffer[pos]);
        }
    }
}

TcpPacket141::TcpPacket141(unsigned char *buffer, unsigned int size):Packet()
{
    _length	= 1 + 4 + 1 + size;
    _buffer	= (unsigned char *)malloc(_length);
    _buffer[0]	= 0xaa;
    *((unsigned int *)(_buffer + 1))	= htonl(_length);
    _buffer[5]	= 0 - _buffer[0] - _buffer[1] - _buffer[2] - _buffer[3] - _buffer[4];
    memcpy(_buffer + 6, buffer, size);
	printf("141fengzhuang ----\n");
	for(int i =0;i<size+6;i++)
         {
              printf("%02x ",_buffer[i]);
         }  
}

TcpPacket141::~TcpPacket141()
{
    free(_buffer);
}

void TcpParser121n1::parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler)
{
    unsigned int pos	= 0;
    while(pos < size)
    {
        if(TCPPROCOTOL_STATE_HEAD == _state)
        {
            if(0xaa == buffer[pos])
            {
                pos++;
                _state	= TCPPROCOTOL_STATE_LENGTH;
                _pos	= 0;
                _cs		= 0xaa;
            }
            else
            {
                LogError("head error:%02x\n", buffer[pos]);
                pos++;
            }
        }
        else if(TCPPROCOTOL_STATE_LENGTH == _state)
        {
            unsigned int len	= 2 - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(((unsigned char *)&_length) + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= 2)
            {
                _cs		+= _length >> 8;
                _cs		+= _length;
                _length	= ntohs(_length);
                _pos	= 0;
                _state	= TCPPROCOTOL_STATE_CSLENGTH;
            }
        }
        else if(TCPPROCOTOL_STATE_CSLENGTH == _state)
        {
            unsigned char *buf	= (unsigned char *)&_length;
            if((unsigned char)(buffer[pos] + 0xaa + buf[0] + buf[1]))
            {
                LogError("check head checksum error.\n");
                if(pos < 2)
                    pos	= 0;
                else
                    pos	-= 2;
                _state	= TCPPROCOTOL_STATE_HEAD;
            }
            else
            {
                _cs	+= buffer[pos];
                pos++;
                _pos	= 0;
                _length	-= 4;
                _buffer	= (unsigned char *)malloc(_length);
                if(!_buffer)
                {
                    LogError("out of memory.\n");
                    exit(-2);
                }
                _state	= TCPPROCOTOL_STATE_BODY;
            }
        }
        else if(TCPPROCOTOL_STATE_BODY == _state)
        {
            unsigned int len	= _length - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(_buffer + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= _length)
            {
                for(unsigned int i = 0; i < _length; i++)
                    _cs	+= _buffer[i];
                if(_cs)
                {
                    LogError("checksum error.\n");
                    if(pos < (_length + 3))
                        pos	= 0;
                    else
                        pos	-= _length + 3;
                }
                else
                {
                    handler->onPacket(_buffer, _length - 1);
                }
                free(_buffer);
                _buffer	= 0;
                _state	= TCPPROCOTOL_STATE_HEAD;
            }
        }
    }
}

TcpPacket121n1::TcpPacket121n1(unsigned char *buffer, unsigned int size):Packet()
{
    _length	= 1 + 2 + 1 + size + 1;
    _buffer	= (unsigned char *)malloc(_length);
    _buffer[0]	= 0xaa;
    *((unsigned short *)(_buffer + 1))	= htons(_length);
    _buffer[3]	= 0 - _buffer[0] - _buffer[1] - _buffer[2];
    memcpy(_buffer + 4, buffer, size);
    _buffer[4 + size]	= 0;
    for(unsigned int i = 0; i < _length - 1; i++)
    {
        _buffer[4 + size]	+= _buffer[i];
    }
    _buffer[4 + size]	= 0 - _buffer[4 + size];
}

TcpPacket121n1::~TcpPacket121n1()
{
    free(_buffer);
}

void TcpParser12n1::parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler)
{
    unsigned int pos	= 0;
    while(pos < size)
    {
        if(TCPPROCOTOL_STATE_HEAD == _state)
        {
            if(0xaa == buffer[pos])
            {
                pos++;
                _state	= TCPPROCOTOL_STATE_LENGTH;
                _pos	= 0;
                _cs		= 0xaa;
            }
            else
            {
                pos++;
            }
        }
        else if(TCPPROCOTOL_STATE_LENGTH == _state)
        {
            unsigned int len	= 2 - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(((unsigned char *)&_length) + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= 2)
            {
                _cs		+= _length >> 8;
                _cs		+= _length;
                _length	= ntohs(_length);
                _pos	= 0;
                _length	-= 3;
                _buffer	= (unsigned char *)malloc(_length);
                if(!_buffer)
                {
                    LogError("out of memory.\n");
                    exit(-2);
                }
                _state	= TCPPROCOTOL_STATE_BODY;
            }
        }
        else if(TCPPROCOTOL_STATE_BODY == _state)
        {
            unsigned int len	= _length - _pos;
            if(size - pos < len)
                len	= size - pos;
            memcpy(_buffer + _pos, buffer + pos, len);
            _pos	+= len;
            pos		+= len;
            if(_pos >= _length)
            {
                for(unsigned int i = 0; i < _length; i++)
                    _cs	+= _buffer[i];
                if(_cs)
                {
                    LogError("checksum error.\n");
                    if(pos < (_length + 2))
                        pos	= 0;
                    else
                        pos	-= _length + 2;
                }
                else
                {
                    handler->onPacket(_buffer, _length - 1);
                }
                free(_buffer);
                _buffer	= 0;
                _state	= TCPPROCOTOL_STATE_HEAD;
            }
        }
    }
}

TcpPacket12n1::TcpPacket12n1(unsigned char *buffer, unsigned int size):Packet()
{
    _length	= 1 + 2 + size + 1;
    _buffer	= (unsigned char *)malloc(_length);
    _buffer[0]	= 0xaa;
    *((unsigned short *)(_buffer + 1))	= htons(_length);
    memcpy(_buffer + 3, buffer, size);
    _buffer[3 + size]	= 0;
    for(unsigned int i = 0; i < _length - 1; i++)
    {
        _buffer[3 + size]	+= _buffer[i];
    }
    _buffer[3 + size]	= 0 - _buffer[3 + size];
}

TcpPacket12n1::~TcpPacket12n1()
{
    free(_buffer);
}

void TcpParserLine::parse(unsigned char *buffer, unsigned int size, TcpParserHandler *handler)
{
    unsigned int pos	= 0;
    while(pos < size)
    {
        _buffer[_length++]	= buffer[pos++];
        if(_buffer[_length - 1] == 0 || _buffer[_length - 1] == '\n')
        {
            _buffer[_length - 1]	= 0;

            if(handler)
                handler->onPacket((unsigned char *)_buffer, _length);

            _length	= 0;
        }

        _length	%= MAX_LINE_BUFFER;
    }
}
