#include <QApplication>
#include "../include/vispart.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    vispart w;
    w.show();
    
    return a.exec();
}
