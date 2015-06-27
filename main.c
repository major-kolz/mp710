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

int main()
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
    }

    govern(handle);

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
