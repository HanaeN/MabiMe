/*
 MabiMe Character Simulator - by Yai (Sophie N)
 Email: sinoc300@gmail.com
 Copyright (C) 2016

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>

#include "model.h"
#include "settingswindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setQuitOnLastWindowClosed(true);
    ui->setupUi(this);
    connect(ui->glSurface, SIGNAL(cameraChange(CameraInfo)), SLOT(cameraChange(CameraInfo)));
    p = new PackManager();
    QTimer::singleShot(1, this, SLOT(startTimer()));
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

void MainWindow::loadPackages() {
    if (p->loadPackages()) {
//        QString PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
//        QString PMGpath = "gfx\\char\\monster\\mesh\\tabhartas\\tabhartas";
        QString PMGpath = "gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01";

//        insertPMG("gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01_boss_mesh", "gfx\\char\\chapter4\\monster\\mesh\\ogre\\c4_ogre01_framework");

//        PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
//        insertPMG("box", PMGpath, PMGpath);

//        PMGpath = "gfx\\char\\pet\\mesh\\";
//        insertPMG("fox", PMGpath + "fox\\pet_star_fox_01_mesh", PMGpath + "fox\\pet_star_fox_01_mesh");

//        PMGpath = "gfx\\char\\pet\\mesh\\bear\\";
//        insertPMG("bear", PMGpath + "pet_ubipanda_mesh", PMGpath + "pet_redpanda_framework");

//        PMGpath = "gfx\\char\\chapter4\\human\\female\\";
//        insertPMG("human", PMGpath + "wear\\female_c4_captainyoo_bsl", PMGpath + "female_framework");
        PMGpath = "gfx\\char\\human\\female\\";
//        insertPMG("human", PMGpath + "female_default_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "face\\female_adult01_f01", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "hair\\female_hair46_t46", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "wear\\female_summercloth01_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "shoes\\female_summercloth02_s04", PMGpath + "female_framework");
        insertPMG("human", "gfx\\char\\chapter4\\human\\female\\mantle\\uni_c4_ego_swordwing01", PMGpath + "female_framework");
//        ui->lMeshes->setText("Meshes: " + QString::number(r.meshes.count()));
    } else {
        QMessageBox msg(this);
        msg.setText("Could not detect the Mabinogi client install path.");
        msg.setInformativeText("Please check the settings and set a valid path.");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setWindowTitle("MabiMe - Warning");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();

        SettingsWindow* settings = new SettingsWindow(this, p->getPath());
        settings->setWindowModality(Qt::WindowModal);
        //QRect rect = QStyle::alignedRect(layoutDirection(), Qt::AlignBottom, QSize(width(), height()/3), geometry());
        //popUp->setGeometry(rect);
        settings->show();
    }
}

void MainWindow::startTimer() {
    loadPackages();
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
