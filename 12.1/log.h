#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#ifndef NOLOG
#define LogError(arg...) Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, 0, arg);
#define LogDebug(arg...) Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, 1, arg);
#define LogInfo(arg...) Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, 2, arg);
#define LogTrace(arg...) Log::output(arg);
#else
#define LogError(arg...) Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, 0, arg);
#define LogDebug(arg...)
#define LogInfo(arg...)
#define LogTrace(arg...)
#endif

class Log
{
public:
	static void trace(const char *file, int line, const char *func, int level, const char *fmt, ...);
	
    static void output(const char *fmt, ...);
};

#endif
