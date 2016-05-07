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
    connect(ui->glSurface, SIGNAL(cameraChange(CameraInfo)), SLOT(cameraChange(CameraInfo)));
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
//        QString PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i.pmg";
        QString PMGpath = "gfx\\char\\monster\\mesh\\tabhartas\\tabhartas_mesh.pmg";
        this->setWindowTitle("MM:" + PMGpath);
        r.LoadPMG(p->ExtractFile(PMGpath));
        for (int i = 0; i < r.meshes.count(); i++) {
            ui->glSurface->meshes.append(r.meshes[i]);
        }
        ui->lMeshes->setText("Meshes: " + QString::number(r.meshes.count()));
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

void MainWindow::cameraChange(CameraInfo camera) {
    ui->lPitch->setText("Pitch: " + QString::number(camera.rotation.pitch));
    ui->lYaw->setText("Yaw: " + QString::number(camera.rotation.yaw));

    ui->lX->setText("X: " + QString::number(camera.x));
    ui->lY->setText("Y: " + QString::number(camera.y));
    ui->lZ->setText("Zoom: " + QString::number(camera.zoom));

}
