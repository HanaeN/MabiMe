#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"
#include "PackReader/packmanager.h"
#include "model.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setQuitOnLastWindowClosed(true);
    ui->setupUi(this);
    connect(ui->glSurface, SIGNAL(cameraChange(CameraInfo)), SLOT(cameraChange(CameraInfo)));
    p = new PackManager();
    if (p->loadPackages()) {
        QTimer::singleShot(1, this, SLOT(startTimer()));
    } else {
        qDebug() << "failed to load";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::close() {
    QMainWindow::close();
    delete p;
}

void MainWindow::show() {
    // suppress show
    QMainWindow::show();
}

void MainWindow::startTimer() {
//        QString PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
//        QString PMGpath = "gfx\\char\\monster\\mesh\\tabhartas\\tabhartas";
        QString PMGpath = "gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01";

        // load a PMG
//        insertPMG("gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01_boss_mesh", "gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01_framework");

//        PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
//        insertPMG("box", PMGpath, PMGpath);

        PMGpath = "gfx\\char\\pet\\mesh\\";
        insertPMG("fox", PMGpath + "fox\\pet_star_fox_01_mesh", PMGpath + "fox\\pet_star_fox_01_mesh");

//        PMGpath = "gfx\\char\\chapter4\\pet\\mesh\\";
//        insertPMG("fox", PMGpath + "bike\\pet_c4_scooter_imp_mesh", PMGpath + "bike\\pet_c4_scooter_imp_framework");

        //PMGpath = "gfx\\char\\chapter4\\human\\female\\";
        //insertPMG("human", PMGpath + "wear\\female_c4_captainyoo_bsl", PMGpath + "female_framework");
//        PMGpath = "gfx\\char\\human\\female\\";
//        insertPMG("human", PMGpath + "female_default_bss", PMGpath + "female_framework");
//        insertPMG("human", PMGpath + "face\\female_adult01_f01", PMGpath + "female_framework");
//        insertPMG("human", PMGpath + "hair\\female_hair46_t46", PMGpath + "female_framework");
//        insertPMG("human", PMGpath + "wear\\female_summercloth01_bss", PMGpath + "female_framework");
//        insertPMG("human", PMGpath + "shoes\\female_summercloth02_s04", PMGpath + "female_framework");
//        insertPMG("human", "gfx\\char\\chapter4\\human\\female\\mantle\\uni_c4_wing01", PMGpath + "female_framework");

 //        ui->lMeshes->setText("Meshes: " + QString::number(r.meshes.count()));
}

void MainWindow::cameraChange(CameraInfo camera) {
    ui->lPitch->setText("Pitch: " + QString::number(camera.rotation.pitch));
    ui->lYaw->setText("Yaw: " + QString::number(camera.rotation.yaw));

    ui->lX->setText("X: " + QString::number(camera.x));
    ui->lY->setText("Y: " + QString::number(camera.y));
    ui->lZ->setText("Zoom: " + QString::number(camera.zoom));
}

void MainWindow::insertPMG(QString modelName, QString PMG, QString FRM) {

    Model *m = ui->glSurface->getModel(modelName);
    if (m == nullptr) {
        m = new Model(p, PMG, FRM);
        m->setName(modelName);
        ui->glSurface->addModel(m);
    } else {
        m->addPMG(PMG);
        ui->glSurface->updateModel(modelName);
    }
    this->setWindowTitle("MM:<" + PMG + "> [" + QString::number(ui->glSurface->getModelCount()) + " total object" + (ui->glSurface->getModelCount() != 1 ? "s]" : "]"));
    ui->glSurface->repaint();
}
