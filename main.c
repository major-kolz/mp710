/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

const unsigned short vendor_id = 0x16c0;
const unsigned short product_id = 0x05df;
unsigned char buf[8];

void check_devices();
void show_package();
short send(hid_device* whom);
void govern(hid_device* handle);
int inputNum();
void all_off(hid_device* handle);

int main(int argc, char * argv[])
{
    /*------------------*/
    const wchar_t *serial_number = NULL;
    hid_device* handle;
    /*------------------*/

    handle = hid_open(vendor_id, product_id, serial_number);
    if (!handle) {
		printf("Не получилось подключить устройство...\n");
		check_devices();
		hid_close(handle);
        hid_exit();
 		return 1;
	}
   

    buf[0]=0x1d;
    hid_send_feature_report(handle, buf, 8);
    hid_get_feature_report(handle, buf, 8);
    if ((buf[0]!=0x1d)&&(buf[1]!=0x2d))
    {
        printf("Устройство реагирует некорректно! Попробуйте переподключить его");
        hid_close(handle);
        hid_exit();
 		return 1;
    }

    if (argc == 1)
        govern(handle); // ручной режим управления
    else if (argc == 3) // вызов с параметрами
    {   
        int state = argv[1][1] == 'n' ? 0x80 : 0x00;
        int port = atoi(argv[2]) - 1;
        if ( port < 0 ||  port > 16) {
            printf("Порт должен быть целым числом в диапазоне от 1 до 16 включительно\n");
            hid_close(handle);
            hid_exit();
            return 1;   
        }
        buf[ port ] = state;
        send(handle); 
    } else // --help, -h и ошибки
        printf("Приложение для работы с цифровым usb-коммутатором MP71 производства 'Masterkit'\n"
                "  Для того, чтобы включить порт №n [1-16] выполните:  'mp710_t on n'\n"
                "  Для того, чтобы выключить порт №n [1-16] выполните:  'mp710_t off n'\n"
                "  Запуск приложения без параметров предоставит последовательный доступ к портам\n\n"); 

    hid_close(handle);
	hid_exit();

    return 0;
}


void check_devices()
{
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		if( vendor_id == cur_dev->vendor_id && product_id == cur_dev->product_id)
            printf("\tНо система распознает его как подключенное...\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
}

void show_package()
{
    unsigned itr = 0;
    unsigned end = sizeof(buf);
    printf("Содержание пакета:\n");
    while( itr < end )
	{
	    printf("%#x\n", buf[itr]);
	    itr++;
	}
}

short send(hid_device* whom)
{
    if( hid_send_feature_report(whom, buf, sizeof(buf)) == sizeof(buf) )
	{
	    if( hid_get_feature_report(whom, buf, sizeof(buf)) != sizeof(buf) )
	    {
            printf("Не удалось получить ответ...\n");
            show_package();
            return -1;
	    }
	}
    else
    {
        printf("Не удалось отправить пакет...\n");
        show_package();
        return -2;
    }
    return 0;
}

int inputNum()
{
    int tmp;
    printf("> ");
    scanf("%d", &tmp);
    while(tmp < 0 || tmp > 16)
    {
        printf("\tВключить\\выключить порт - [1; 16], либо 0 - для выхода\n");
        printf("> ");
        scanf("%d", &tmp);
    }
    return tmp;
}

void all_off(hid_device* handle)
{
    buf[0] = 0x63;  // магическое число для записи в устройство
	buf[1] = 0x00;  // номер порта [числа зациклены в кольцо из 16 элементов, от 0x00 до 0х0f]
	buf[2] = 0x00;  // скважность выходного сигнала на этом порте [0x00 - 0x80]
	buf[3] = 0x00;  // изменение скважности по времени.
	buf[4] = 0xFF;  // на время выполнения смахивает
	buf[5] = 0xff;
	buf[6] = 0xFF;  // кол-во повторений
	buf[7] = 0xff;
    int i = 0;
    while(i<16)
    {
        buf[1] = i;
        send(handle);
        ++i;
    }
}

void govern(hid_device* handle)
{
    buf[0] = 0x63;  // магическое число для записи в устройство
	buf[1] = 0x00;  // номер порта [числа зациклены в кольцо из 16 элементов, от 0x00 до 0х0f]
	buf[2] = 0x80;  // скважность выходного сигнала на этом порте [0x00 - 0x80]
	buf[3] = 0x00;  // изменение скважности по времени.
	buf[4] = 0xFF;  // на время выполнения смахивает
	buf[5] = 0xff;
	buf[6] = 0xFF;  // кол-во повторений
	buf[7] = 0xff;

    all_off(handle);
    char dex[16] = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
    int read;

    printf("\tВведите число в диапазоне [1-16] чтобы включить\\выключить это порт,\nЛибо 0 - для завершения программы\n");
    do
    {
        read = inputNum();
        if( read == 0 )
            break;
        buf[1] = read - 1;
        if( dex[ buf[1] ])
        {
            buf[2] = 0x00;
            dex[ buf[1] ] = 0;
        }
        else
        {
            buf[2] = 0x80;
            dex[ buf[1] ] = 1;
        }
        send(handle);
    } while(1);
}
