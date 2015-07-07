#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QButtonGroup>
#include <QFileDialog>
#include "QDir"
#include "QMessageBox"

#include <sys/types.h>
#include <signal.h>

namespace Ui {
class controller;
}

class controller : public QWidget
{
    Q_OBJECT
    
public:
    explicit controller(QWidget *parent = 0);
    ~controller();
    void cmd_offall();      // выключаем
    void cmd_onall();       // включаем
private:
    Ui::controller *ui;
    int page;
    QProcess *vec;
    QString CFG_FILE;
    QString PID_FILE;
    QString toDAEMON;
    QString WORK_DIR;
    unsigned char cmd[16][6];
    int is_daemon_started();

    QButtonGroup* row1;
    QButtonGroup* row2;
    QButtonGroup* row3;
    QButtonGroup* row4;
    QButtonGroup* row5;
    QButtonGroup* row6;
    QButtonGroup* row7;
    QButtonGroup* row8;
    QButtonGroup* row9;
    QButtonGroup* row10;
    QButtonGroup* row11;
    QButtonGroup* row12;
    QButtonGroup* row13;
    QButtonGroup* row14;
    QButtonGroup* row15;
    QButtonGroup* row16;

    void link_rows();
    void uncheck_ratio_checkbox();
public slots:
    void page_sliding_left();
    void page_sliding_right();
    void start_daemon();
    void stop_daemon();
    void reconfig_daemon();

    int update_config();            // Перезаписать файл конфигурации
    void perceive_config();         // Считать конфигурацию с выставленных кнопок
    void set_time_max(bool);
    void set_repeat_max(bool);
    void set_mod2zero();
    void all_ratio_on();
    void all_ratio_off();
    void all_on();                  // Время и повторения на максиммум, скважность тоже
    void all_off();                 // тоже, но скважность в 0
    void show_another_rows(bool);
    void row_ratio_change(QAbstractButton*);
    void open_file_dialog();
};

#endif // CONTROLLER_H
