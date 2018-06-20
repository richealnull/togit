#include "log.h"
#include <time.h>

void Log::trace(const char *file, int line, const char *func, int level, const char *fmt, ...)
{
    time_t now	= time(0);
    struct tm *t = localtime(&now);

    static char sLogBuffer[1024 * 1024 * 2];
	static unsigned int seq	= 0;
    sprintf(sLogBuffer, "%04d-%02d-%02d %02d:%02d:%02d|%s:%d|lv:%d|%d|%s|", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, file, line, level, seq++, func);

    va_list args;
    va_start(args, fmt);
    vsprintf(sLogBuffer + strlen(sLogBuffer), fmt, args);
    va_end(args);
	
	printf("%s", sLogBuffer);

	fflush(stdout);
}

void Log::output(const char *fmt, ...)
{
    static char sLogBuffer[1024 * 1024 * 2];
    va_list args;
    va_start(args, fmt);
    vsprintf(sLogBuffer, fmt, args);
    va_end(args);
	printf("%s", sLogBuffer);
	
	fflush(stdout);
}
