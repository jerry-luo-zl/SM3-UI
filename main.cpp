#include "sm3_mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SM3_MainWindow w;
    w.show();
    return a.exec();
}
