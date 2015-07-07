#include "main.h"
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

/// Следующий код появился благодаря: http://www.cplusplus.com/reference/cstdio/vfprintf/
void WriteLog(const char* txt, ...)		// Запись в лог-файл. Каждый вызов заново открывает файл и дописывает сообщение в конец
{
	FILE* log = fopen(LOG_FILE, "a");
	va_list arg;
    va_start(arg, txt);
    vfprintf(log, txt, arg);
    va_end(arg);
	fclose(log);
}

void SetPidFile(const char* Filename, pid_t pid)
{
    FILE* f;

    f = fopen(Filename, "w+");
    if (f == NULL)
    {
    	WriteLog("Problem with creating PID-file (%s)\n", strerror(errno));
    }
    else
    {
        fprintf(f, "%u", pid);
        fclose(f);
    }
}
