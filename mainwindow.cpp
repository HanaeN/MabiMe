#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>


#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"
#include "model.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setQuitOnLastWindowClosed(true);
    ui->setupUi(this);
    connect(ui->glSurface, SIGNAL(cameraChange(CameraInfo)), SLOT(cameraChange(CameraInfo)));
    p = new MabiPackReader();
    if (p->OpenPackage("C:/Nexon/Library/mabinogi/appdata/package/183_full.pack")) {
        QTimer::singleShot(1, this, SLOT(startTimer()));
    } else {
        qDebug() << "failed to load";
    }
    objects = QList<RenderObject*>();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::close() {
    QMainWindow::close();
    p->ClosePackage();
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

        PMGpath = "gfx\\char\\human\\female\\";
        PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
        insertPMG(PMGpath, PMGpath);

//        insertPMG(PMGpath + "face\\female_adult01_f01", PMGpath + "female_framework");
//        insertPMG(PMGpath + "hair\\female_hair46_t46", PMGpath + "female_framework");
//        insertPMG(PMGpath + "wear\\female_summercloth01_bss", PMGpath + "female_framework");
//        insertPMG(PMGpath + "shoes\\female_summercloth02_s04", PMGpath + "female_framework");
//        insertPMG("gfx\\char\\chapter4\\human\\female\\mantle\\uni_c4_wing01", PMGpath + "female_framework");

//        ui->lMeshes->setText("Meshes: " + QString::number(r.meshes.count()));
}

void MainWindow::cameraChange(CameraInfo camera) {
    ui->lPitch->setText("Pitch: " + QString::number(camera.rotation.pitch));
    ui->lYaw->setText("Yaw: " + QString::number(camera.rotation.yaw));

    ui->lX->setText("X: " + QString::number(camera.x));
    ui->lY->setText("Y: " + QString::number(camera.y));
    ui->lZ->setText("Zoom: " + QString::number(camera.zoom));

}

void MainWindow::insertPMG(QString PMG, QString FRM) {
    Model *m = new Model(p, PMG, FRM);
    return;
    RenderObject *obj = new RenderObject();
    obj->r.loadPMG(p->ExtractFile(PMG + ".pmg"));
    obj->f.loadFRM(p->ExtractFile(FRM + ".frm"));
    obj->name = PMG;
    objects.append(obj);
    PMGObject *o = new PMGObject();
    o->bones = obj->f.bones;
    for (int i = 0; i < obj->r.textures.count(); i++) {
        PMGTexture *t = new PMGTexture;
        t->name = obj->r.textures[i];
        QPixmap pix;
        pix.loadFromData(p->ExtractFile(p->FindTexture(t->name)));
        t->img = pix.toImage();
        o->textures.append(t);
    }
    o->meshes = obj->r.meshes;
    ui->glSurface->addPMG(o);
    this->setWindowTitle("MM:<" + PMG + "> [" + QString::number(objects.count()) + " total object" + (objects.count() != 1 ? "s]" : "]"));
    ui->glSurface->repaint();
}
