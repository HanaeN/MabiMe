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
//        QString PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
//        QString PMGpath = "gfx\\char\\monster\\mesh\\tabhartas\\tabhartas";
        QString PMGpath = "gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01";
        this->setWindowTitle("MM:" + PMGpath);

        // load a PMG
        r.loadPMG(p->ExtractFile(PMGpath + "_boss_mesh.pmg"));
        f.loadFRM(p->ExtractFile(PMGpath + "_framework.frm"));

        PMGObject *o = new PMGObject();
        o->bones = f.bones;
        for (int i = 0; i < r.textures.count(); i++) {
            PMGTexture *t = new PMGTexture;
            t->name = r.textures[i];
            QPixmap pix;
            pix.loadFromData(p->ExtractFile(p->FindTexture(t->name)));
            t->img = pix.toImage();
            o->textures.append(t);
        }
        o->meshes = r.meshes;
        ui->glSurface->addPMG(o);
        ui->lMeshes->setText("Meshes: " + QString::number(r.meshes.count()));
        ui->glSurface->repaint();
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
