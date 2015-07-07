///	Программа для работы с устройством mp710 от МастерКит
/*	Выполненно по материалам: http://habrahabr.ru/post/129207/
*	Описание работы:
*		запускается из домашней директории
*		хранит свой PID в файле	~/.local/share/mp710/daemon.pid
*		файл конфигурации:		~/.local/share/mp710/config.ini либо переданный параметром
*		лог пишет в:			~/.local/share/mp710/daemon.log
*	Алгоритм работы:
*		- Отделение от управляющего терминала и переход в фоновый режим.
*		- Разделение на две части: родитель(мониторинг) и потомок(функционал демона).
*		- Мониторинг состояния процесса демона.
*		- Обработка команды обновления конфига.
*		- Обработка ошибок.
*/

#if !defined(_GNU_SOURCE)
	#define _GNU_SOURCE
#endif

#include "main.h"

int main(int argc, char** argv)
{
	int status;
    int pid;
    const char* DEF_CFG_FILE = "./.local/share/mp710/config.txt";		// путь к файлу конфигурации по-умолчанию

    if( chdir( getenv("HOME") ) == -1 ){								// переходим в домашнюю директорию
        printf("Can't change directory (%s)\n", strerror(errno));
        return -2;
    }
	FILE* f;															// Проверим, не запускаем ли мы демона повторно
    f = fopen(PID_FILE, "r");
    if (f != NULL){
    	printf("Daemon already working! Cancel this initiation.\n");
		return -3;
	}

    pid = fork();
    if (pid == -1){ 													// Если не удалось запустить потомка
        printf("Error: Start daemon failed (%s)\n", strerror(errno));
        return -1;
    } else if (!pid){ 													// Если это потомок
        umask(0);															// разрешаем выставлять все биты прав на создаваемые файлы, иначе у нас могут быть проблемы с правами доступа

        setsid();															// создаём новый сеанс, чтобы не зависеть от родителя

    	time_t rawtime;														// Следующие 4 строки нужны для определения времени
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		WriteLog("\n-> %s", asctime (timeinfo));

        if (argc == 2){														// Если получили параметр, то трактуем его как путь к файлу конфигурации
			status = MonitorProc(argv[1]);										// данная функция будет осуществлять слежение за "демоном"
		} else {
			status = MonitorProc(DEF_CFG_FILE);
		}

        return status;
    } else {																// Если это родитель
        SetPidFile(PID_FILE, pid);			                                    // данная функция создаст файл с нашим PID'ом
        printf("Initial phase is done.\n");

        return 0;																// завершим процес, т.к. основную свою задачу (запуск демона) мы выполнили
	}
}

int MonitorProc(const char* CFG_FILE)
{
	int      pid = -1;
    int      status;
    int      need_start = 1;
    sigset_t sigset;
    siginfo_t siginfo;

    sigemptyset(&sigset);			// настраиваем сигналы которые будем обрабатывать
    sigaddset(&sigset, SIGQUIT);	// сигнал остановки процесса пользователем
    sigaddset(&sigset, SIGINT);		// сигнал для остановки процесса пользователем с терминала
    sigaddset(&sigset, SIGTERM);	// сигнал запроса завершения процесса
    sigaddset(&sigset, SIGCHLD);	// сигнал посылаемый при изменении статуса дочернего процесса
    sigaddset(&sigset, SIGHUP);		// пользовательский сигнал который мы будем использовать для обновления конфига
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    while(1){
        if (need_start){			// если необходимо создать потомка
            pid = fork();
        }

        need_start = 1;

        if ( pid == -1 ){			// если произошла ошибка с потомком
            WriteLog("[MONITOR] Fork failed (%s)\n", strerror(errno));
        } else if ( !pid ){ 		// если мы потомок
            status = WorkProc(CFG_FILE);	// запустим функцию отвечающую за работу демона
            exit(status);					// завершим процесс
        } else {					// если мы родитель
            sigwaitinfo(&sigset, &siginfo);			// ожидаем поступление сигнала
            if (siginfo.si_signo == SIGCHLD){		// если пришел сигнал от потомка
				wait(&status);						// получаем статус завершение
                status = WEXITSTATUS(status);		// преобразуем статус в нормальный вид
                if( status == CHILD_NEED_TERMINATE ){	// если потомок завершил работу с кодом говорящем о том, что нет нужды дальше работать                    WriteLog("[MONITOR] Child stopped\n");
                    break;
                } else if( status == CHILD_NEED_WORK ){	// если требуется перезапустить потомка
                    WriteLog("[MONITOR] Child restart\n");
                }
            } else if(siginfo.si_signo == SIGHUP){ // если пришел сигнал что необходимо перезагрузить конфиг
                kill(pid, SIGHUP); 					// перешлем его потомку
                need_start = 0; 						// установим флаг что нам не надо запускать потомка заново
            } else {									// если пришел какой-либо другой ожидаемый сигнал
                WriteLog("[MONITOR] Signal %s\n", strsignal(siginfo.si_signo));
                kill(pid, SIGTERM);
                status = 0;
                break;
            }
        }
    }

    WriteLog("[MONITOR] Stop\n");
    unlink(PID_FILE);		// удалим файл с PID'ом
    return status;
}

void signal_error(int sig, siginfo_t *si, void *ptr)
 {
     void* ErrorAddr;
     void* Trace[16];
     int    x;
     int    TraceSize;
     char** Messages;

     // запишем в лог что за сигнал пришел
     WriteLog("[DAEMON] Signal: %s, Addr: 0x%0.16X\n", strsignal(sig), si->si_addr);


     #if __WORDSIZE == 64 // если дело имеем с 64 битной ОС
         // получим адрес инструкции которая вызвала ошибку
         ErrorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_RIP];
     #else
         // получим адрес инструкции которая вызвала ошибку
         ErrorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_EIP];
     #endif

     // произведем backtrace чтобы получить весь стек вызовов
     TraceSize = backtrace(Trace, 16);
     Trace[1] = ErrorAddr;

     // получим расшифровку трасировки
     Messages = backtrace_symbols(Trace, TraceSize);
     if (Messages)
     {
         WriteLog("== Backtrace ==\n");

         // запишем в лог
         for (x = 1; x < TraceSize; x++)
         {
             WriteLog("%s\n", Messages[x]);
         }

         WriteLog("== End Backtrace ==\n");
         free(Messages);
     }

     WriteLog("[DAEMON] Stopped\n");

     // завершим процесс с кодом требующим перезапуска
     exit(CHILD_NEED_WORK);
 }
