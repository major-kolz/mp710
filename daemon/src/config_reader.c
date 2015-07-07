/**	Структура конфигурационного файла
 16 строк вида:
	[скважность порта] [изменение скважности < 0x10] [время выполнения 1] [время выполнения 2] [число повторений 1] [число выполнений 2]

 Номер порта = номер строки
**/
// Для разделения:  printf("\n%x %x  %x\n", hex, hex>>8, hex>>8<<8 ^ hex ); 65536 - макс число

#include "main.h"

int LoadConfig(const char* config_descriptor, command* form)
{
	FILE* config = fopen(config_descriptor, "r");	// открываем конфигурационный файл для чтения
	if( config == NULL ){
		WriteLog("Can't open configuration file %s\n", config_descriptor);
		return -1;
	}

	int i = 0;
    int res = -1;
	while(i < 16){
		res = fscanf(config, "%x %x %x %x %x %x", &form->ratio[i], &form->ratio_mod[i], &form->time1[i], &form->time2[i], &form->repeat1[i], &form->repeat2[i]);
        if( res != 6 )
            WriteLog("[DAEMON] Problems with reading the configuration file:\n\t\t"
                    "read only %d values (should be 6) from line %d", res, i);
		// Для отладки:
		//WriteLog("%#x %#x %#x %#x %#x %#x\n", form->ratio[i], form->ratio_mod[i], form->time1[i], form->time2[i], form->repeat1[i], form->repeat2[i]);
		++i;
	}

	fclose(config);
	return 0;
}
