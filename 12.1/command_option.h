#ifndef __COMMAND_OPTIONS_H__
#define __COMMAND_OPTIONS_H__

#include <string>
#include <string.h>
#include <map>
#include <getopt.h>
#include "utility.h"

using namespace std;

typedef int (*HandleOption)(char *opt);

//命令行参数处理类
class CommandOption
{
    struct Option
    {
        HandleOption handler;
        string name;
        string tip;
        bool arg;
    };
private:
    map<char, struct Option> _options;
public:
    //注册命令
    CommandOption *registerOption(const char *optionName, bool arg, HandleOption handler, const char *tip = 0)
    {
        _options[optionName[0]].handler = handler;
        _options[optionName[0]].name    = optionName;
        _options[optionName[0]].tip     = tip ? tip : "";
        _options[optionName[0]].arg     = arg;

        return this;
    }
    //解析命令并且分配
    int parse(int argc, char *argv[])
    {
        struct option *options  = (struct option *)(malloc(sizeof(struct option) * (_options.size() + 1)));
        if(!options)
            return 0;

        struct option *opt  = options;

        char optString[255] = {0};

        map<char, struct Option>::iterator it, next;
        STLFOR(it, next, _options)
        {
            char ch = it->first;
            strncat(optString, &ch, 1);
            if(it->second.arg)
                strcat(optString, ":");

            opt->name       = it->second.name.c_str();
            opt->has_arg    = it->second.arg ? required_argument : no_argument;
            opt->flag       = 0;
            opt->val        = ch;

            opt++;
        }

        opt->name       = 0;
        opt->has_arg    = 0;
        opt->flag       = 0;
        opt->val        = 0;

        int optVal;

        while((optVal = getopt_long(argc, argv, optString, options, 0)) != -1)
        {
            map<char, struct Option>::iterator it   = _options.find(optVal);
            if(_options.end() == it)
            {
                printf("\n");
                map<char, struct Option>::iterator i, n;
                STLFOR(i, n, _options)
                {
                    printf("-%c --%s : %s\n", i->first, i->second.name.c_str(), i->second.tip.c_str());
                }
                printf("\n");
                
                free(options);
                return -1;
            }

            if(it->second.handler)
            {
                int ret = it->second.handler(optarg);
                if(ret < 0)
                {
                    free(options);
                    return ret;
                }
            }
        }

        free(options);
        return 0;
    }
};

#endif
