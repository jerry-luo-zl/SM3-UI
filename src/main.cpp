#include "sm3_mainwindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SM3_MainWindow w;
    w.show();
    return a.exec();
}
