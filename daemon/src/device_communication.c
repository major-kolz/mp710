#include "main.h"

int check_devices()					// Поиск устройства в системе
{
	struct hid_device_info *devs, *cur_dev;
	const unsigned short vendor_id = 0x16c0;
	const unsigned short product_id = 0x05df;
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {				//	 перебираем все устройства, до совпадения с Product ID и Vendor ID
		if( vendor_id == cur_dev->vendor_id && product_id == cur_dev->product_id)
			return 0;
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	return 1;
}

hid_device * dev_connect()
{
	const unsigned short vendor_id = 0x16c0;
	const unsigned short product_id = 0x05df;
	const wchar_t *serial_number = NULL;
	unsigned char buf[8];
	hid_device * dev;

	if( check_devices() ){
		WriteLog("[DAEMON] CONNECTING FAULT! Device not found!\n");
		return NULL;
	}

	dev = hid_open(vendor_id, product_id, serial_number);
    if(!dev){
    	WriteLog("[DAEMON] CONNECTING FAULT! Can't open device\n");
        return NULL;
    }

    buf[0]=0x1d;
    hid_send_feature_report(dev, buf, 8);
    hid_get_feature_report(dev, buf, 8);

    if ((buf[0]!=0x1d)&&(buf[1]!=0x2d)){
		WriteLog("[DAEMON] Error with device connecting: returned false answer at checking\n");
		return NULL;
    }
    return dev;
}

int send(hid_device* dev, command cmd)
{
	int i = 0;				// итерационная переменная по портам
	unsigned char buf[8];	// собственно, команда
	buf[0] = 0x63;			// "магическое" число для записи

	while(i < 16)
	{
		buf[1] = i;
		buf[2] = cmd.ratio[i];
		buf[3] = cmd.ratio_mod[i];
		buf[4] = cmd.time1[i];
		buf[5] = cmd.time2[i];
		buf[6] = cmd.repeat1[i];
		buf[7] = cmd.repeat2[i];

		//WriteLog("%d %d %d %d %d %d\n", buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);	// отладка
		if( hid_send_feature_report(dev, buf, sizeof(buf)) == sizeof(buf) ){
			if( hid_get_feature_report(dev, buf, sizeof(buf)) != sizeof(buf) ){
				WriteLog("[DAEMON] Device not responce\n");
				return -1;
			}
		} else {
			WriteLog("[DAEMON] Command N %d sending failed\n", i);
			return -2;
		}
		++i;
	}
	WriteLog("[DAEMON] New command was successfully sended\n");

	return 0;
}
