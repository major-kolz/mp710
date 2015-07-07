#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

///	Библиотеки
#include <stdio.h>		// printf, работа с файлами
#include <string.h>		// strerror
#include <errno.h>		// errno
#include <unistd.h>		// pid & fork
#include <sys/types.h>	// umask
#include <sys/stat.h>
#include <signal.h>		// siginfo_t и проч.
#include <stdlib.h>		// exit
#include <sys/wait.h>	// wait
#include <execinfo.h>	// backtrace
#include <time.h> 		// time_t, struct tm
#include "hidapi.h"		// Работа с usb (там libusb подключается)

/// Константы
#define FD_LIMIT	1024*10				// лимит для установки максимально кол-во открытых дискрипторов
#define CHILD_NEED_WORK         1		// константы для кодов завершения процесса
#define CHILD_NEED_TERMINATE    2
#define PID_FILE "./.local/share/mp710/daemon.pid"			// имя файла, в котором хранится PID демона (нужно для коректной остановки)
#define LOG_FILE "./.local/share/mp710/daemon.log"			// Начинается с точки, ибо перейдем в домашнюю директорию

/// Работа с устройством
typedef struct{
	int ratio[16];
	int ratio_mod[16];
	int time1[16];
	int time2[16];
	int repeat1[16];
	int repeat2[16];
} command;

hid_device * dev_connect();
int send(hid_device*, command);

/// Прототипы используемых функций
int LoadConfig(const char*, command*);	// загружает конфиг

int MonitorProc(const char* CFG_FILE);	// следит за демоном, перезапускает в случае чего
int WorkProc();

void WriteLog(const char*, ...);
void SetPidFile(const char*, pid_t);	// создание PID-файла
void signal_error(int, siginfo_t *, void *);

#endif // MAIN_H_INCLUDED
