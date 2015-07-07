#include "controller.h"
#include "ui_controller.h"

controller::controller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::controller)
{
    ui->setupUi(this);
    WORK_DIR = QDir::homePath() + "/.local/share/mp710/";
    CFG_FILE = WORK_DIR + "config.txt";
    PID_FILE = WORK_DIR + "daemon.pid";
    toDAEMON = "./mp710_daemon";

    cmd_offall();
    page = 0;

    is_daemon_started();
    link_rows();
    ///
    connect(ui->left_page, SIGNAL(clicked()), this, SLOT(page_sliding_left()) );
    connect(ui->right_page, SIGNAL(clicked()), this, SLOT(page_sliding_right()) );
    connect(ui->save_config, SIGNAL(clicked()), this, SLOT(update_config()) );
    connect(ui->daemon_start, SIGNAL(clicked()), this, SLOT(start_daemon()) );
    connect(ui->daemon_stop, SIGNAL(clicked()), this, SLOT(stop_daemon()) );
    connect(ui->config_change__reload, SIGNAL(clicked()), this, SLOT(reconfig_daemon()) );
    connect(ui->open_file_dialog, SIGNAL(clicked()), this, SLOT(open_file_dialog()) );

    connect(ui->mod_to_zero, SIGNAL(clicked()), this, SLOT(set_mod2zero()));
    connect(ui->time_maximaze, SIGNAL(clicked(bool)), this, SLOT(set_time_max(bool)));
    connect(ui->repeat_maximaze, SIGNAL(clicked(bool)), this, SLOT(set_repeat_max(bool)));
    connect(ui->ratio_on, SIGNAL(clicked()), this, SLOT(all_ratio_on()));
    connect(ui->ratio_off, SIGNAL(clicked()), this, SLOT(all_ratio_off()));
    connect(ui->on_all, SIGNAL(clicked()), this, SLOT(all_on()));
    connect(ui->off_all, SIGNAL(clicked()), this, SLOT(all_off()));
    connect(ui->Leader, SIGNAL(clicked(bool)) , this, SLOT(show_another_rows(bool)) );
}

void controller::link_rows()
{
    row1 = new QButtonGroup;
    row2 = new QButtonGroup;
    row3 = new QButtonGroup;
    row4 = new QButtonGroup;
    row5 = new QButtonGroup;
    row6 = new QButtonGroup;
    row7 = new QButtonGroup;
    row8 = new QButtonGroup;
    row9 = new QButtonGroup;
    row10 = new QButtonGroup;
    row11 = new QButtonGroup;
    row12 = new QButtonGroup;
    row13 = new QButtonGroup;
    row14 = new QButtonGroup;
    row15 = new QButtonGroup;
    row16 = new QButtonGroup;

    row1->addButton(ui->setmaxRatio_1);
    row1->addButton(ui->setminRatio_1);

    row2->addButton(ui->setmaxRatio_2);
    row2->addButton(ui->setminRatio_2);

    row3->addButton(ui->setmaxRatio_3);
    row3->addButton(ui->setminRatio_3);

    row4->addButton(ui->setmaxRatio_4);
    row4->addButton(ui->setminRatio_4);

    row5->addButton(ui->setmaxRatio_5);
    row5->addButton(ui->setminRatio_5);

    row6->addButton(ui->setmaxRatio_6);
    row6->addButton(ui->setminRatio_6);

    row7->addButton(ui->setmaxRatio_7);
    row7->addButton(ui->setminRatio_7);

    row8->addButton(ui->setmaxRatio_8);
    row8->addButton(ui->setminRatio_8);

    row9->addButton(ui->setmaxRatio_9);
    row9->addButton(ui->setminRatio_9);

    row10->addButton(ui->setmaxRatio_10);
    row10->addButton(ui->setminRatio_10);

    row11->addButton(ui->setmaxRatio_11);
    row11->addButton(ui->setminRatio_11);

    row12->addButton(ui->setmaxRatio_12);
    row12->addButton(ui->setminRatio_12);

    row13->addButton(ui->setmaxRatio_13);
    row13->addButton(ui->setminRatio_13);

    row14->addButton(ui->setmaxRatio_14);
    row14->addButton(ui->setminRatio_14);

    row15->addButton(ui->setmaxRatio_15);
    row15->addButton(ui->setminRatio_15);

    row16->addButton(ui->setmaxRatio_16);
    row16->addButton(ui->setminRatio_16);

    connect(row1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row2, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row3, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row4, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row5, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row6, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row7, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row8, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row9, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row10, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row11, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row12, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row13, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row14, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row15, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
    connect(row16, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(row_ratio_change(QAbstractButton*)) );
}

controller::~controller()
{
    delete ui;
}

void controller::cmd_offall()
{
    for(int i = 0; i < 16; ++i)
    {
        cmd[i][0] = 0x00;   // ratio
        cmd[i][1] = 0x00;   // mod
        cmd[i][2] = 0xFF;   // time
        cmd[i][3] = 0xFF;
        cmd[i][4] = 0xFF;   // repeat
        cmd[i][5] = 0xFF;
    }
}

void controller::cmd_onall()
{
    for(int i = 0; i < 16; ++i)
    {
        cmd[i][0] = 0x80;
        cmd[i][1] = 0x00;
        cmd[i][2] = 0xFF;
        cmd[i][3] = 0xFF;
        cmd[i][4] = 0xFF;
        cmd[i][5] = 0xFF;
    }
}

int controller::is_daemon_started()
{
    QFile pid(PID_FILE);
    if (pid.exists())
    {
        ui->daemon_stop->setEnabled(true);
        ui->daemon_start->setEnabled(false);
        pid.close();
        return 0;
    }
    return 1;
}

/// ======================================= СЛОТЫ ===============================================
int controller::update_config()
{
    perceive_config();              // обновимся
    QFile file(CFG_FILE);
    QTextStream out(&file);
    if(file.open(QIODevice::WriteOnly))
    {
          for(int i = 0; i < 16; ++i)
              out << "0x" + QString::number(cmd[i][0], 16) << " " 
                  << "0x" + QString::number(cmd[i][1], 16) << " "  
                  << "0x" + QString::number(cmd[i][2], 16) << " "
                  << "0x" + QString::number(cmd[i][3], 16) << " "
                  << "0x" + QString::number(cmd[i][4], 16) << " "
                  << "0x" + QString::number(cmd[i][5], 16) << "\n";
          ui->config_edit_status->setText(trUtf8("Статус: Сохраненно"));
    }
    file.close();

    return 0;
}

void controller::open_file_dialog()
{
    QString customFile = QFileDialog::getOpenFileName(this, tr("Open Configuration"), WORK_DIR, tr("Text Files (*cfg)"));
    ui->cfg_filepath->setText(customFile);
}

void controller::page_sliding_left()
{
    page --;
    if(page < 0)
        page = 3;
    ui->config_panel->setCurrentIndex(page);
}

void controller::page_sliding_right()
{
    page ++;
    if(page > 3)
        page = 0;
    ui->config_panel->setCurrentIndex(page);
}

void controller::start_daemon()
{   
    QMessageBox msgBox;
    vec = new QProcess(this);

    if(ui->cfg_filepath->text() != "") { // пользователь задает свой файл конфигурации
        QStringList customConfigFile;
        customConfigFile << ui->cfg_filepath->text();
        QString ccf = customConfigFile.at(0).toLocal8Bit().constData();
        QFile configuration(ccf);
        if (!configuration.exists()) {
            msgBox.setText( trUtf8("Указанный вами файл конфигурации не существует!") );
            msgBox.exec();
            return;
        }
        CFG_FILE = ccf;
        vec->start(toDAEMON, customConfigFile);
    } else {                            // использует стандартный
        QFile default_config(CFG_FILE); 
        if (!default_config.exists())
            update_config();
        vec->start(toDAEMON);
    }

    if( !vec->waitForFinished() ) {
        msgBox.setText( trUtf8("Проблемы с запуском демона: не получается выделить под него процесс. Проверьте Проверьте 'mp710_daemon' в списке запущенных процессов. Если процесса с таким именем нет - проверьте, есть ли бинарный файл с таким именем рядом с бинарником этой программы.") );
        msgBox.exec();
        return;
    }
        
    QFile pid(PID_FILE);
    if (pid.exists())
    {
        ui->daemon_stop->setEnabled(true);
        ui->daemon_start->setEnabled(false);
        pid.close();
    } else {
        msgBox.setText( trUtf8("Проблемы с запуском демона: ") + PID_FILE  + trUtf8(" не обнаружен. Проверьте 'mp710_daemon' в списке запущенных процессов. Если процесса с таким именем нет - проверьте, есть ли бинарный файл с таким именем рядом с бинарником этой программы."));
        msgBox.exec();
    }
}

void controller::stop_daemon()
{
    QFile pid(PID_FILE);
    if (pid.open(QIODevice::ReadOnly))
    {
        QTextStream in(&pid);
        QString PID;
        in >> PID;
        if(!kill(PID.toInt(), SIGTERM))
        {
            ui->daemon_start->setEnabled(true);
            ui->daemon_stop->setEnabled(false);
        }
    }
    pid.close();
}

void controller::reconfig_daemon()
{
    QFile pid(PID_FILE);
    if (pid.open(QIODevice::ReadOnly))
    {
        QTextStream in(&pid);
        QString PID;
        in >> PID;
        kill(PID.toInt(), SIGHUP);
    }
    pid.close();
}

void controller::show_another_rows(bool select)
{
    if(!select)
    {
        ui->labnam_2->show();
        ui->labnam_3->show();
        ui->labnam_4->show();
        ui->Mm1->show();
        ui->Mm2->show();
        ui->Mm3->show();
        ui->Mm4->show();

        ui->ratio_2->show();
        ui->ratio_3->show();
        ui->ratio_4->show();
        ui->ratio_5->show();
        ui->ratio_6->show();
        ui->ratio_7->show();
        ui->ratio_8->show();
        ui->ratio_9->show();
        ui->ratio_10->show();
        ui->ratio_11->show();
        ui->ratio_12->show();
        ui->ratio_13->show();
        ui->ratio_14->show();
        ui->ratio_15->show();
        ui->ratio_16->show();

        ui->mod2->show();
        ui->mod3->show();
        ui->mod4->show();
        ui->mod5->show();
        ui->mod6->show();
        ui->mod7->show();
        ui->mod8->show();
        ui->mod9->show();
        ui->mod10->show();
        ui->mod11->show();
        ui->mod12->show();
        ui->mod13->show();
        ui->mod14->show();
        ui->mod15->show();
        ui->mod16->show();

        ui->time_2->show();
        ui->time_3->show();
        ui->time_4->show();
        ui->time_5->show();
        ui->time_6->show();
        ui->time_7->show();
        ui->time_8->show();
        ui->time_9->show();
        ui->time_10->show();
        ui->time_11->show();
        ui->time_12->show();
        ui->time_13->show();
        ui->time_14->show();
        ui->time_15->show();
        ui->time_16->show();

        ui->repeat2->show();
        ui->repeat3->show();
        ui->repeat4->show();
        ui->repeat5->show();
        ui->repeat6->show();
        ui->repeat7->show();
        ui->repeat8->show();
        ui->repeat9->show();
        ui->repeat10->show();
        ui->repeat11->show();
        ui->repeat12->show();
        ui->repeat13->show();
        ui->repeat14->show();
        ui->repeat15->show();
        ui->repeat16->show();
    }
    else
    {
        ui->labnam_2->hide();
        ui->labnam_3->hide();
        ui->labnam_4->hide();
        ui->Mm1->hide();
        ui->Mm2->hide();
        ui->Mm3->hide();
        ui->Mm4->hide();

        ui->ratio_2->hide();
        ui->ratio_3->hide();
        ui->ratio_4->hide();
        ui->ratio_5->hide();
        ui->ratio_6->hide();
        ui->ratio_7->hide();
        ui->ratio_8->hide();
        ui->ratio_9->hide();
        ui->ratio_10->hide();
        ui->ratio_11->hide();
        ui->ratio_12->hide();
        ui->ratio_13->hide();
        ui->ratio_14->hide();
        ui->ratio_15->hide();
        ui->ratio_16->hide();

        ui->mod2->hide();
        ui->mod3->hide();
        ui->mod4->hide();
        ui->mod5->hide();
        ui->mod6->hide();
        ui->mod7->hide();
        ui->mod8->hide();
        ui->mod9->hide();
        ui->mod10->hide();
        ui->mod11->hide();
        ui->mod12->hide();
        ui->mod13->hide();
        ui->mod14->hide();
        ui->mod15->hide();
        ui->mod16->hide();

        ui->time_2->hide();
        ui->time_3->hide();
        ui->time_4->hide();
        ui->time_5->hide();
        ui->time_6->hide();
        ui->time_7->hide();
        ui->time_8->hide();
        ui->time_9->hide();
        ui->time_10->hide();
        ui->time_11->hide();
        ui->time_12->hide();
        ui->time_13->hide();
        ui->time_14->hide();
        ui->time_15->hide();
        ui->time_16->hide();

        ui->repeat2->hide();
        ui->repeat3->hide();
        ui->repeat4->hide();
        ui->repeat5->hide();
        ui->repeat6->hide();
        ui->repeat7->hide();
        ui->repeat8->hide();
        ui->repeat9->hide();
        ui->repeat10->hide();
        ui->repeat11->hide();
        ui->repeat12->hide();
        ui->repeat13->hide();
        ui->repeat14->hide();
        ui->repeat15->hide();
        ui->repeat16->hide();
    }
}

void controller::perceive_config()
{///Мне стыдно за эту функцию( Она опрашивает все кнопки и составляет конфигурацию пользователя для устройства
    if( ui->Leader->isChecked() )    // все по первой строчке
    {
        for(unsigned i = 0; i < 16; ++i)
        {
            cmd[i][0] = ui->ratio_1->value();
            cmd[i][1] = ui->mod1->value();
            cmd[i][2] = ui->time_1->value()>>8;
            cmd[i][3] = ui->time_1->value()>>8<<8;
            cmd[i][4] = ui->repeat1->value()>>8;
            cmd[i][5] = ui->repeat1->value()>>8<<8;
        }
    }
    else
    {
        cmd[0][0] = ui->ratio_1->value();
        cmd[1][0] = ui->ratio_2->value();
        cmd[2][0] = ui->ratio_3->value();
        cmd[3][0] = ui->ratio_4->value();
        cmd[4][0] = ui->ratio_5->value();
        cmd[5][0] = ui->ratio_6->value();
        cmd[6][0] = ui->ratio_7->value();
        cmd[7][0] = ui->ratio_8->value();
        cmd[8][0] = ui->ratio_9->value();
        cmd[9][0] = ui->ratio_10->value();
        cmd[10][0] = ui->ratio_11->value();
        cmd[11][0] = ui->ratio_12->value();
        cmd[12][0] = ui->ratio_13->value();
        cmd[13][0] = ui->ratio_14->value();
        cmd[14][0] = ui->ratio_15->value();
        cmd[15][0] = ui->ratio_16->value();

        cmd[0][1] = ui->mod1->value();
        cmd[1][1] = ui->mod2->value();
        cmd[2][1] = ui->mod3->value();
        cmd[3][1] = ui->mod4->value();
        cmd[4][1] = ui->mod5->value();
        cmd[5][1] = ui->mod6->value();
        cmd[6][1] = ui->mod7->value();
        cmd[7][1] = ui->mod8->value();
        cmd[8][1] = ui->mod9->value();
        cmd[9][1] = ui->mod10->value();
        cmd[10][1] = ui->mod11->value();
        cmd[11][1] = ui->mod12->value();
        cmd[12][1] = ui->mod13->value();
        cmd[13][1] = ui->mod14->value();
        cmd[14][1] = ui->mod15->value();
        cmd[15][1] = ui->mod16->value();

        cmd[0][2] = ui->time_1->value()>>8;
        cmd[1][2] = ui->time_2->value()>>8;
        cmd[2][2] = ui->time_3->value()>>8;
        cmd[3][2] = ui->time_4->value()>>8;
        cmd[4][2] = ui->time_5->value()>>8;
        cmd[5][2] = ui->time_6->value()>>8;
        cmd[6][2] = ui->time_7->value()>>8;
        cmd[7][2] = ui->time_8->value()>>8;
        cmd[8][2] = ui->time_9->value()>>8;
        cmd[9][2] = ui->time_10->value()>>8;
        cmd[10][3] = ui->time_11->value()>>8;
        cmd[11][2] = ui->time_12->value()>>8;
        cmd[12][2] = ui->time_13->value()>>8;
        cmd[13][2] = ui->time_14->value()>>8;
        cmd[14][2] = ui->time_15->value()>>8;
        cmd[15][2] = ui->time_16->value()>>8;

        cmd[0][3] = ui->time_1->value()>>8<<8;
        cmd[1][3] = ui->time_2->value()>>8<<8;
        cmd[2][3] = ui->time_3->value()>>8<<8;
        cmd[3][3] = ui->time_4->value()>>8<<8;
        cmd[4][3] = ui->time_5->value()>>8<<8;
        cmd[5][3] = ui->time_6->value()>>8<<8;
        cmd[6][3] = ui->time_7->value()>>8<<8;
        cmd[7][3] = ui->time_8->value()>>8<<8;
        cmd[8][3] = ui->time_9->value()>>8<<8;
        cmd[9][3] = ui->time_10->value()>>8<<8;
        cmd[10][3] = ui->time_11->value()>>8<<8;
        cmd[11][3] = ui->time_12->value()>>8<<8;
        cmd[12][3] = ui->time_13->value()>>8<<8;
        cmd[13][3] = ui->time_14->value()>>8<<8;
        cmd[14][3] = ui->time_15->value()>>8<<8;
        cmd[15][3] = ui->time_16->value()>>8<<8;

        cmd[0][4] = ui->repeat1->value()>>8;
        cmd[1][4] = ui->repeat2->value()>>8;
        cmd[2][4] = ui->repeat3->value()>>8;
        cmd[3][4] = ui->repeat4->value()>>8;
        cmd[4][4] = ui->repeat5->value()>>8;
        cmd[5][4] = ui->repeat6->value()>>8;
        cmd[6][4] = ui->repeat7->value()>>8;
        cmd[7][4] = ui->repeat8->value()>>8;
        cmd[8][4] = ui->repeat9->value()>>8;
        cmd[9][4] = ui->repeat10->value()>>8;
        cmd[10][4] = ui->repeat11->value()>>8;
        cmd[11][4] = ui->repeat12->value()>>8;
        cmd[12][4] = ui->repeat13->value()>>8;
        cmd[13][4] = ui->repeat14->value()>>8;
        cmd[14][4] = ui->repeat15->value()>>8;
        cmd[15][4] = ui->repeat16->value()>>8;

        cmd[0][5] = ui->repeat1->value()>>8<<8;
        cmd[1][5] = ui->repeat2->value()>>8<<8;
        cmd[2][5] = ui->repeat3->value()>>8<<8;
        cmd[3][5] = ui->repeat4->value()>>8<<8;
        cmd[4][5] = ui->repeat5->value()>>8<<8;
        cmd[5][5] = ui->repeat6->value()>>8<<8;
        cmd[6][5] = ui->repeat7->value()>>8<<8;
        cmd[7][5] = ui->repeat8->value()>>8<<8;
        cmd[8][5] = ui->repeat9->value()>>8<<8;
        cmd[9][5] = ui->repeat10->value()>>8<<8;
        cmd[10][5] = ui->repeat11->value()>>8<<8;
        cmd[11][5] = ui->repeat12->value()>>8<<8;
        cmd[12][5] = ui->repeat13->value()>>8<<8;
        cmd[13][5] = ui->repeat14->value()>>8<<8;
        cmd[14][5] = ui->repeat15->value()>>8<<8;
        cmd[15][5] = ui->repeat16->value()>>8<<8;

    }
}

void controller::set_time_max(bool JohnBool)
{
    unsigned char value = 0x00;
    if(JohnBool)
        value = 0xFF;

    for(unsigned i = 0; i < 16; ++i)
    {
        cmd[i][2] = value;
        cmd[i][3] = value;
    }

    ui->time_1->setValue(0xffff);
    ui->time_2->setValue(0xffff);
    ui->time_3->setValue(0xffff);
    ui->time_4->setValue(0xffff);
    ui->time_5->setValue(0xffff);
    ui->time_6->setValue(0xffff);
    ui->time_7->setValue(0xffff);
    ui->time_8->setValue(0xffff);
    ui->time_9->setValue(0xffff);
    ui->time_10->setValue(0xffff);
    ui->time_11->setValue(0xffff);
    ui->time_12->setValue(0xffff);
    ui->time_13->setValue(0xffff);
    ui->time_14->setValue(0xffff);
    ui->time_15->setValue(0xffff);
    ui->time_16->setValue(0xffff);
}

void controller::set_repeat_max(bool JohnBool)
{
    unsigned char value = 0x00;
    if(JohnBool)
        value = 0xFF;

    for(unsigned i = 0; i < 16; ++i)
    {
        cmd[i][4] = value;
        cmd[i][5] = value;
    }

    ui->repeat1->setValue(0xffff);
    ui->repeat2->setValue(0xffff);
    ui->repeat3->setValue(0xffff);
    ui->repeat4->setValue(0xffff);
    ui->repeat5->setValue(0xffff);
    ui->repeat6->setValue(0xffff);
    ui->repeat7->setValue(0xffff);
    ui->repeat8->setValue(0xffff);
    ui->repeat9->setValue(0xffff);
    ui->repeat10->setValue(0xffff);
    ui->repeat11->setValue(0xffff);
    ui->repeat12->setValue(0xffff);
    ui->repeat13->setValue(0xffff);
    ui->repeat14->setValue(0xffff);
    ui->repeat15->setValue(0xffff);
    ui->repeat16->setValue(0xffff);
}

void controller::set_mod2zero()
{
    unsigned char value = 0x00;

    for(unsigned i = 0; i < 16; ++i)
        cmd[i][1] = value;

    ui->mod1->setValue(value);
    ui->mod2->setValue(0x00);
    ui->mod3->setValue(0x00);
    ui->mod4->setValue(0x00);
    ui->mod5->setValue(0x00);
    ui->mod6->setValue(0x00);
    ui->mod7->setValue(0x00);
    ui->mod8->setValue(0x00);
    ui->mod9->setValue(0x00);
    ui->mod10->setValue(0x00);
    ui->mod11->setValue(0x00);
    ui->mod12->setValue(0x00);
    ui->mod13->setValue(0x00);
    ui->mod14->setValue(0x00);
    ui->mod15->setValue(0x00);
    ui->mod16->setValue(0x00);
}

void controller::uncheck_ratio_checkbox()
{
    ui->setmaxRatio_1->setChecked(false);
    ui->setmaxRatio_2->setChecked(false);
    ui->setmaxRatio_3->setChecked(false);
    ui->setmaxRatio_4->setChecked(false);
    ui->setmaxRatio_5->setChecked(false);
    ui->setmaxRatio_6->setChecked(false);
    ui->setmaxRatio_7->setChecked(false);
    ui->setmaxRatio_8->setChecked(false);
    ui->setmaxRatio_9->setChecked(false);
    ui->setmaxRatio_10->setChecked(false);
    ui->setmaxRatio_11->setChecked(false);
    ui->setmaxRatio_12->setChecked(false);
    ui->setmaxRatio_13->setChecked(false);
    ui->setmaxRatio_14->setChecked(false);
    ui->setmaxRatio_15->setChecked(false);
    ui->setmaxRatio_16->setChecked(false);

    ui->setminRatio_1->setChecked(false);
    ui->setminRatio_2->setChecked(false);
    ui->setminRatio_3->setChecked(false);
    ui->setminRatio_4->setChecked(false);
    ui->setminRatio_5->setChecked(false);
    ui->setminRatio_6->setChecked(false);
    ui->setminRatio_7->setChecked(false);
    ui->setminRatio_8->setChecked(false);
    ui->setminRatio_9->setChecked(false);
    ui->setminRatio_10->setChecked(false);
    ui->setminRatio_11->setChecked(false);
    ui->setminRatio_12->setChecked(false);
    ui->setminRatio_13->setChecked(false);
    ui->setminRatio_14->setChecked(false);
    ui->setminRatio_15->setChecked(false);
    ui->setminRatio_16->setChecked(false);
}

void controller::all_ratio_on()
{
    uncheck_ratio_checkbox();

    for(unsigned i = 0; i < 16; ++i)
        cmd[i][0] = 0x80;

    ui->ratio_1->setValue(0x80);
    ui->ratio_2->setValue(0x80);
    ui->ratio_3->setValue(0x80);
    ui->ratio_4->setValue(0x80);
    ui->ratio_5->setValue(0x80);
    ui->ratio_6->setValue(0x80);
    ui->ratio_7->setValue(0x80);
    ui->ratio_8->setValue(0x80);
    ui->ratio_9->setValue(0x80);
    ui->ratio_10->setValue(0x80);
    ui->ratio_11->setValue(0x80);
    ui->ratio_12->setValue(0x80);
    ui->ratio_13->setValue(0x80);
    ui->ratio_14->setValue(0x80);
    ui->ratio_15->setValue(0x80);
    ui->ratio_16->setValue(0x80);
}

void controller::all_ratio_off()
{
    uncheck_ratio_checkbox();

    for(unsigned i = 0; i < 16; ++i)
        cmd[i][0] = 0x00;

    ui->ratio_1->setValue(0x00);
    ui->ratio_2->setValue(0x00);
    ui->ratio_3->setValue(0x00);
    ui->ratio_4->setValue(0x00);
    ui->ratio_5->setValue(0x00);
    ui->ratio_6->setValue(0x00);
    ui->ratio_7->setValue(0x00);
    ui->ratio_8->setValue(0x00);
    ui->ratio_9->setValue(0x00);
    ui->ratio_10->setValue(0x00);
    ui->ratio_11->setValue(0x00);
    ui->ratio_12->setValue(0x00);
    ui->ratio_13->setValue(0x00);
    ui->ratio_14->setValue(0x00);
    ui->ratio_15->setValue(0x00);
    ui->ratio_16->setValue(0x00);
}

void controller::all_on()
{
    all_ratio_on();
    set_mod2zero();
    set_time_max(true);
    set_repeat_max(true);
}

void controller::all_off()
{
    all_ratio_off();
    set_mod2zero();
    set_time_max(true);
    set_repeat_max(true);
}

void controller::row_ratio_change(QAbstractButton* but)
{
    QString name = but->objectName();
    unsigned ratio = 0x00;
    unsigned index = name.mid(12, -1).toInt();

    if(name.mid(3, 3) == "max")
        ratio = 0x80;

    cmd[index][0] = ratio;

    switch(index){
    case 1:
        ui->ratio_1->setValue(ratio);
        break;
    case 2:
        ui->ratio_2->setValue(ratio);
        break;
    case 3:
        ui->ratio_3->setValue(ratio);
        break;
    case 4:
        ui->ratio_4->setValue(ratio);
        break;
    case 5:
        ui->ratio_5->setValue(ratio);
        break;
    case 6:
        ui->ratio_6->setValue(ratio);
        break;
    case 7:
        ui->ratio_7->setValue(ratio);
        break;
    case 8:
        ui->ratio_8->setValue(ratio);
        break;
    case 9:
        ui->ratio_9->setValue(ratio);
        break;
    case 10:
        ui->ratio_10->setValue(ratio);
        break;
    case 11:
        ui->ratio_11->setValue(ratio);
        break;
    case 12:
        ui->ratio_12->setValue(ratio);
        break;
    case 13:
        ui->ratio_13->setValue(ratio);
        break;
    case 14:
        ui->ratio_14->setValue(ratio);
        break;
    case 15:
        ui->ratio_15->setValue(ratio);
        break;
    case 16:
        ui->ratio_16->setValue(ratio);
        break;
    };
}
