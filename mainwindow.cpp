#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>


#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"

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
        PMGReader r;
//        r.LoadPMG(p->ExtractFile("gfx\\char\\human\\female\\hair\\female_hair01_t01.pmg"));
        r.LoadPMG(p->ExtractFile("gfx\\char\\human\\female\\hair\\female_hair07_t07.pmg"));
//        r.LoadPMG(p->ExtractFile("gfx\\char\\item\\mesh\\item_woodplate01_i.pmg"));
        qDebug() << r.meshes.count();
        ui->glSurface->meshes.append(r.meshes[0]);
        ui->glSurface->repaint();

        /*
        QFile f("D:\\Coding\\tmp.pmg");
        f.open(QFile::ReadWrite);
        f.write(file);
        f.close();
*/
        // opened so do  things
        p->ClosePackage();
    }
    //this->close();
}
