#include "../include/vispart.h"

vispart::vispart(QWidget *parent)
    : QWidget(parent)
{
    status = new QLabel("Checking device...", this);
    status->setGeometry(100, 10, 135, 20);

    conTdev = new QPushButton("Connect", this);
    conTdev->move(3, 3);

    char buttons[] = {
        '1', 'Q', 'A', 'Z',
        '2', 'W', 'S', 'X',
        '3', 'E', 'D', 'C',
        '4', 'R', 'F', 'V',
    };
    QString txt;
    for(unsigned i = 0; i < 4; ++i)
    {
        for(unsigned j = 0; j < 4; ++j)
        {
            txt = buttons[4*i + j];
            chanels[ 4*i + j ] = new QCheckBox(txt, this);
            if(i > 1)
                chanels[ 4*i + j ] -> move( i*50 + 50, j*25 + 40 );
            else
                chanels[ 4*i + j ] -> move( i*50 + 10, j*25 + 40);
            pulse_ratio[4*i + j] = false;
        }
    }
    resize(270, 150);

    QObject::connect(this, SIGNAL(goodbye()), this, SLOT(close()));
    QObject::connect(conTdev, SIGNAL(clicked()), this, SLOT(reconnect_device()));

    is_ready = false;
    switch( dev_connect() )
    {
    case 0:
        status->setText("Device is ready!");
        conTdev->setFlat(true);
        is_ready = true;
        offAll();
        break;
    case 1:
        status->setText("Can't connect...");
        break;
    case 2:
        status->setText("Not response...");
    };
}

vispart::~vispart()
{
    hid_close(handle);
    hid_exit();
}

void vispart::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_1:
        change_state(0);
        break;
    case Qt::Key_Q:
        change_state(1);
        break;
    case Qt::Key_A:
        change_state(2);
        break;
    case Qt::Key_Z:
        change_state(3);
        break;
    case Qt::Key_2:
        change_state(4);
        break;
    case Qt::Key_W:
        change_state(5);
        break;
    case Qt::Key_S:
        change_state(6);
        break;
    case Qt::Key_X:
        change_state(7);
        break;
    case Qt::Key_3:
        change_state(8);
        break;
    case Qt::Key_E:
        change_state(9);
        break;
    case Qt::Key_D:
        change_state(10);
        break;
    case Qt::Key_C:
        change_state(11);
        break;
    case Qt::Key_4:
        change_state(12);
        break;
    case Qt::Key_R:
        change_state(13);
        break;
    case Qt::Key_F:
        change_state(14);
        break;
    case Qt::Key_V:
        change_state(15);
        break;
    case Qt::Key_Escape:
        emit goodbye();
    }
}

void vispart::change_state(unsigned chanel)
{
    if(is_ready)
    {
        unsigned char buf[8];
        buf[0] = 0x63;  // магическое число для записи в устройство
        buf[1] = 0x00;  // номер порта [числа зациклены в кольцо из 16 элементов, от 0x00 до 0х0f]
        buf[2] = 0x80;  // скважность выходного сигнала на этом порте [0x00 - 0x80]
        buf[3] = 0x00;  // изменение скважности по времени.
        buf[4] = 0xFF;  // на время выполнения смахивает
        buf[5] = 0xff;
        buf[6] = 0xFF;  // кол-во повторений
        buf[7] = 0xff;
        if(pulse_ratio[chanel])
        {
            chanels[chanel]->setChecked(false);
            pulse_ratio[chanel] = false;
            buf[1] = chanel;
            buf[2] = 0;
            send_command(buf);
        }
        else
        {
            chanels[chanel]->setChecked(true);
            pulse_ratio[chanel] = true;
            buf[1] = chanel;
            send_command(buf);
        }
    }
    else
        status->setText("Device disconected!");
}
int vispart::dev_connect()
{
    handle = hid_open(0x16c0, 0x05df, NULL);
    if(!handle)
        return 1;

    unsigned char buf[8];
    buf[0]=0x1d;
    hid_send_feature_report(handle, buf, 8);
    hid_get_feature_report(handle, buf, 8);
    if ((buf[0]!=0x1d)&&(buf[1]!=0x2d))
        return 2;
    return 0;
}

void vispart::reconnect_device()
{
    if(!is_ready)
        switch( dev_connect() )
        {
        case 0:
            status->setText("Device is ready!");
            is_ready = true;
            offAll();
            conTdev->setFlat(true);
            break;
        case 1:
            status->setText("Can't connect...");
            break;
        case 2:
            status->setText("Not response...");
        };
}

int vispart::send_command(unsigned char* buf)
{
    if( hid_send_feature_report(handle, buf, sizeof(buf)) == sizeof(buf) )
    {
        if( hid_get_feature_report(handle, buf, sizeof(buf)) != sizeof(buf) )
        {
            status->setText("Getn't proof!");
            return 1;
        }
    }
    else
    {
        status->setText("Sending failed!");
        return 2;
    }
    return 0;
}

void vispart::offAll()
{
    unsigned char buf[8];
    buf[0] = 0x63;  // магическое число для записи в устройство
    buf[1] = 0x00;  // номер порта [числа зациклены в кольцо из 16 элементов, от 0x00 до 0х0f]
    buf[2] = 0x00;  // скважность выходного сигнала на этом порте [0x00 - 0x80]
    buf[3] = 0x00;  // изменение скважности по времени.
    buf[4] = 0xFF;  // на время выполнения смахивает
    buf[5] = 0xff;
    buf[6] = 0xFF;  // кол-во повторений
    buf[7] = 0xff;
    for(unsigned i = 0; i < 16; ++i)
    {
        buf[1] = i;
        send_command(buf);
    }
}
