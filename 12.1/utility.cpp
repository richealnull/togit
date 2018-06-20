#include "utility.h"
#include <sys/time.h>
#include <string.h>
#include <sstream>
#include <regex.h>

unsigned long long int getTickCount()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

time_t getLocalTime(time_t time, int targetTimezone)
{
    return time + targetTimezone * 60 * 60;
}

time_t getMorningTime(time_t time)
{
    struct tm now	= *(gmtime(&time));
    now.tm_hour	= 0;
    now.tm_min	= 0;
    now.tm_sec	= 0;
    return mktime(&now);
}

string fillZero(int time)
{
    return time < 10 ? "0" + num2str(time) : num2str(time);
}

string hex2str(unsigned char *buffer, int length)
{
    string str;
    for(int i = 0; i < length; i++)
    {
        char h  = (*(buffer + i)) >> 4;

        if(h <= 9)
            h   += '0';
        else
            h   += 'A' - 10;

        str += h;

        char l  = *(buffer + i) & 0x0f;

        if(l <= 9)
            l   += '0';
        else
            l   += 'A' - 10;

        str += l;
    }
    return str;
}

string hex2rev(unsigned char *buffer, int length)
{
    string str;
    for(int i = length - 1; i >= 0; i--)
    {
        char h  = (*(buffer + i)) >> 4;

        if(h <= 9)
            h   += '0';
        else
            h   += 'A' - 10;

        str += h;

        char l  = *(buffer + i) & 0x0f;

        if(l <= 9)
            l   += '0';
        else
            l   += 'A' - 10;

        str += l;
    }
    return str;
}

int strsplit(char *src, const char *delimiters, char **dest, int size)
{
    int i = 0;
    char *buf   = src;
    while((dest[i]=strtok(buf, delimiters)) != 0 && i < size)
    {
        buf = 0;
        i++;
    }
    return i;
}

deque<string> strsplit(string src, const string &delimiters)
{
    string::size_type pos;
    deque<string> result;

    src	+= delimiters;

    string::size_type size	= src.size();

    for(string::size_type i = 0; i < size; i++)
    {
        pos	= src.find(delimiters, i);
        if(pos < size)
        {
            string s	= src.substr(i, pos - i);
            result.push_back(s);
            i	= pos + delimiters.size() - 1;
        }
    }
    return result;
}

void strreplace(string &src, const char *from, const char *to)
{
    int index = src.find(from);
    while(index >= 0)
    {
        src.replace(index, strlen(from), to);
        index = src.find(from, index + strlen(to));
    }
}

void strreplace(string &src, const char *pattern, function<string (string &)> onMatch)
{
    regex_t	reg;
    if(regcomp(&reg, pattern, REG_EXTENDED))
        return;

    char strSrc[1024 * 1024 + 1] = {0};
    char rest[1024 * 1024 + 1] = {0};
    strncpy(strSrc, src.c_str(), 1024 * 1024);
    regmatch_t match[1];
    do
    {
        if(regexec(&reg, strSrc, sizeof(match) / sizeof(match[0]), match, 0))
            break;

        string key(strSrc + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
        string replace	= onMatch(key);
        strcpy(rest, strSrc + match[0].rm_eo);
        strncpy(strSrc + match[0].rm_so, replace.c_str(), 1024 * 1024 - match[0].rm_so);
        strncpy(strSrc + strlen(strSrc), rest, 1024 * 1024 - strlen(strSrc));
    }
    while(1);

    regfree(&reg);

    src	= strSrc;
}

string num2str(int num)
{
    string str;
    stringstream ss;
    ss << num;
    ss >> str;
    return str;
}

double cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n)  
{  
    double od, nd;  
    double id, sd;  
    double cpu_use ;  

    od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);
    //第一次(用户+优先级+系统+空闲)的时间再赋给od  
    nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);
    //第二次(用户+优先级+系统+空闲)的时间再赋给od  
    id = (double) (n->idle);    //用户第一次和第二次的时间之差再赋给id  
    sd = (double) (o->idle) ;    //系统第一次和第二次的时间之差再赋给sd  
    if((nd-od) != 0)  
        cpu_use =100.0- ((id-sd))/(nd-od)*100.00; 
    //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used  
    else cpu_use = 0;  
    return cpu_use;  
}  

void get_cpuoccupy (CPU_OCCUPY *cpust)  
{  
    FILE *fd;   
    char buff[256];  
    CPU_OCCUPY *cpu_occupy;  
    cpu_occupy=cpust;  

    fd = fopen ("/proc/stat", "r");  
    fgets (buff, sizeof(buff), fd);  

    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);  

    fclose(fd);  
} 

double getCpuRate()  
{  
    CPU_OCCUPY cpu_stat1;  
    CPU_OCCUPY cpu_stat2;  
    double cpu;  
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);  
    sleep(1);  

    //第二次获取cpu使用情况  
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);  
    //计算cpu使用率  
    cpu = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);  

    return cpu;  
}

string GetLocalIp()
{
    int sock_get_ip;  
    char ipaddr[50];  

    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;     

    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
        printf("socket create failse...GetLocalIp!/n");  
        return "";  
    }  

    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     

    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
        return "";     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));         

    printf("local ip:%s /n",ipaddr);      
    close( sock_get_ip );  
    return string( ipaddr );
}
bool getPublicIp(string& ip)  
{  
    int    sock;  
    char **pptr = NULL;  
    struct sockaddr_in    destAddr;  
    struct hostent    *ptr = NULL;  
    char destIP[128];  
  
    sock = socket(AF_INET,SOCK_STREAM,0);  
    if( -1 == sock ){  
        perror("creat socket failed");  
        return false;  
    }  
    bzero((void *)&destAddr,sizeof(destAddr));  
    destAddr.sin_family = AF_INET;  
    destAddr.sin_port = htons(80);  
    ptr = gethostbyname("www.ip138.com");  
    if(NULL == ptr){  
        perror("gethostbyname error");  
        return false;  
    }  
    for(pptr=ptr->h_addr_list ; NULL != *pptr ; ++pptr){  
        inet_ntop(ptr->h_addrtype,*pptr,destIP,sizeof(destIP));  
        printf("addr:%s\n",destIP);  
        ip = destIP;  
        return true;  
    }  
    return true;  
}  
