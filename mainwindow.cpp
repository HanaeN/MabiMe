#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

#include "PackReader/mabipackreader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setQuitOnLastWindowClosed(true);
    ui->setupUi(this);
    QTimer::singleShot(1, this, SLOT(startTimer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show() {
    // suppress show
    QMainWindow::show();
}

void MainWindow::startTimer() {
    MabiPackReader *p = new MabiPackReader();
    if (p->OpenPackage("C:/Nexon/Library/mabinogi/appdata/package/183_full.pack")) {
        QByteArray file = p->ExtractFile("gfx\\char\\human\\female\\face\\female_default_f00.pmg");
        QFile f("D:\\Coding\\tmp.pmg");
        f.open(QFile::ReadWrite);
        f.write(file);
        f.close();
        // opened so do  things
        p->ClosePackage();
    }
    this->close();
}
