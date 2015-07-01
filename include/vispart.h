#ifndef VISPART_H
#define VISPART_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QKeyEvent>
#include <QPushButton>

#include "hidapi.h"

class vispart : public QWidget
{
    Q_OBJECT
    
public:
    vispart(QWidget *parent = 0);
    ~vispart();

    void change_state(unsigned);
    int dev_connect();
    int send_command(unsigned char* buf);
    void offAll();
private:
        QLabel * status;
        QCheckBox * chanels[16];
        QPushButton * conTdev;
        bool pulse_ratio[16];

        hid_device* handle;
        bool is_ready;
protected:
        void keyPressEvent(QKeyEvent *event);
signals:
        void goodbye();
public slots:
        void reconnect_device();
};

#endif // VISPART_H
