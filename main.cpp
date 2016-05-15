#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Yai");
    a.setApplicationName("MabiMe");
    a.setOrganizationDomain("http://mabi.me");
    MainWindow w;
    w.show();

    return a.exec();
}
