#include "main.h"

/* Ждет один из сигналов
	По SIGUSR1: обновляем конфигурацию
	Иначе: выход
*/
int WorkProc(const char* CFG_FILE)
{
    struct sigaction sigact;
    sigset_t sigset;
    int signo;					// Полученый сигнал
	int cmd_status		=	0;
    command cmd;				// Считанный файл конфигурации = 16 команд, по одной на порт.
    hid_device * dev;

    dev = dev_connect();

    if ( LoadConfig(CFG_FILE, &cmd) == 0 )
		WriteLog("[DAEMON] Successfully load config\n");

    // сигналы об ошибках в программе будут обрататывать более тщательно
    // указываем что хотим получать расширенную информацию об ошибках
    sigact.sa_flags = SA_SIGINFO;
    // задаем функцию обработчик сигналов
    sigact.sa_sigaction = signal_error;

    sigemptyset(&sigact.sa_mask);

    // установим наш обработчик на сигналы
    sigaction(SIGFPE, &sigact, 0); 	// ошибка FPU
    sigaction(SIGILL, &sigact, 0); 	// ошибочная инструкция
    sigaction(SIGSEGV, &sigact, 0);	// ошибка доступа к памяти
    sigaction(SIGBUS, &sigact, 0); 	// ошибка шины, при обращении к физической памяти

    sigemptyset(&sigset);

    // блокируем сигналы которые будем ожидать
    sigaddset(&sigset, SIGQUIT);	// сигнал остановки процесса пользователем
    sigaddset(&sigset, SIGINT);		// сигнал для остановки процесса пользователем с терминала
    sigaddset(&sigset, SIGTERM);	// сигнал запроса завершения процесса
    sigaddset(&sigset, SIGHUP);		// Cигнал обновления конфигурации
    sigaddset(&sigset, SIGUSR2);	// пользовательский сигнал, который мы будем использовать для записи в устройство

    // Собственно, блокировка
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    WriteLog("[DAEMON] Started\n");

	while(1)						// цикл ожидания сообщений
	{
		if( dev == NULL ){
			WriteLog("[DAEMON] Device communication problem (maybe it is unpluged?)\n");
			dev = dev_connect();
			if( dev == NULL ){
				WriteLog("[DAEMON] Reconnect doesn't solve the problem\n");
			} else {
				WriteLog("[DAEMON] Reconnect solve the problem\n");
				cmd_status = send(dev, cmd);		// запись после устраненного сбоя
			}
		} else {
			cmd_status = send(dev, cmd);			// запись в нормальных условиях
		}

		if( cmd_status != 0 ) {
			dev = NULL;
		}

		sigwait(&sigset, &signo);	// ждем указанных сообщений

		if (signo == SIGHUP){		// Команда на обновление конфигурации
			if ( LoadConfig(CFG_FILE, &cmd) == 0 ){			// Негативные результаты объяснит сам LoadConfig
				WriteLog("[DAEMON] Reload config OK\n");
			}
		} else {					// если какой-либо другой сигнал, то выйдем из цикла
			hid_close(dev);
			hid_exit();
			break;
		}
	}

    WriteLog("[DAEMON] Stopped\n");

    // вернем код не требующим перезапуска
    return CHILD_NEED_TERMINATE;
}
