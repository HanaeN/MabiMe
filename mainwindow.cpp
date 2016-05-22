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
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>

#include "model.h"
#include "settingswindow.h"
#include "mabimelayerdelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setQuitOnLastWindowClosed(true);
    ui->setupUi(this);
    ui->l_layers->header()->close();
    ui->l_layers->setIndentation(0);
    ui->l_layers->setMouseTracking(true);
    ui->g_load->setCurrentIndex(1);
    for (int i = 0; i < ui->l_category->count(); i++) {
        ui->l_category->item(i)->setSizeHint(QSize(60, 60));
    }
    MabiMeLayerDelegate *d = new MabiMeLayerDelegate();
    connect(d, SIGNAL(repaintWidget()), SLOT(repaintLayers()));
    connect(d, SIGNAL(closeButtonClicked(QTreeWidgetItem*)), SLOT(onLayerCloseButtonClicked(QTreeWidgetItem*)));
    connect(d, SIGNAL(visibilityButtonClicked(QTreeWidgetItem*)), SLOT(onLayerVisibilityButtonClicked(QTreeWidgetItem*)));
    connect(ui->l_layers, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(onLayerItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    ui->l_layers->setItemDelegate(d);
    connect(ui->glSurface, SIGNAL(cameraChange(CameraInfo)), SLOT(cameraChange(CameraInfo)));
    p = new PackManager();
    connect(p, SIGNAL(currentLanguagePackProgress(QString,int,int)), SLOT(onLoadXMLUpdate(QString,int,int)));
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
    connect(&loadPackagesWatcher, SIGNAL(finished()), SLOT(onLoadPackages()));
    loadPackagesAction = QtConcurrent::run(p, &PackManager::loadPackages);
    loadPackagesWatcher.setFuture(loadPackagesAction);
}

void MainWindow::startTimer() {
    loadPackages();
}

void MainWindow::cameraChange(CameraInfo camera) {
    if (floor(camera.zoom) != ui->s_zoom->value()) {
        ui->s_zoom->setValue(floor(camera.zoom));
    }
}

void MainWindow::insertPMG(QString modelName, QString PMG, QString FRM) {

    Model *m = ui->glSurface->getModel(modelName);
    if (m == nullptr) {
        QTreeWidgetItem *i = new QTreeWidgetItem(ui->l_layers, 0);
        ui->l_layers->setRootIsDecorated(false);
        i->setExpanded(true);
        m = new Model(p, PMG, FRM);
        i->setData(0, LayerRole::LAYER_VISIBLE, true);
        i->setText(0, "$MODEL$" + modelName);
        i->setData(0, LayerRole::HAS_BONES, m->hasBoneTree());
        ui->l_layers->setCurrentItem(i);
        m->setName(modelName);
        if (m->hasBoneTree()) {
            i = new QTreeWidgetItem();
            i->setFlags(Qt::NoItemFlags);
            i->setData(0, LayerRole::LAYER_VISIBLE, true);
            i->setText(0, FRM.split("\\", QString::SkipEmptyParts).last());
            i->setData(0, LayerRole::HAS_BONES, true);
            ui->l_layers->findItems("$MODEL$" + modelName, Qt::MatchExactly)[0]->addChild(i);
        }
        i = new QTreeWidgetItem();
        i->setFlags(Qt::NoItemFlags);
        i->setData(0, LayerRole::LAYER_VISIBLE, true);
        i->setText(0, PMG.split("\\", QString::SkipEmptyParts).last());
        i->setData(0, LayerRole::HAS_BONES, false);
        ui->l_layers->findItems("$MODEL$" + modelName, Qt::MatchExactly)[0]->addChild(i);
        ui->glSurface->addModel(m);
    } else {
        QTreeWidgetItem *i = new QTreeWidgetItem();
        i->setText(0, PMG.split("\\", QString::SkipEmptyParts).last());
        i->setData(0, LayerRole::LAYER_VISIBLE, true);
        i->setData(0, LayerRole::HAS_BONES, false);
        i->setFlags(Qt::NoItemFlags);
        ui->l_layers->findItems("$MODEL$" + modelName, Qt::MatchExactly)[0]->addChild(i);
        m->addPMG(PMG);
        ui->glSurface->updateModel(modelName);
    }
    this->setWindowTitle("MM:<" + PMG + "> [" + QString::number(ui->glSurface->getModelCount()) + " total object" + (ui->glSurface->getModelCount() != 1 ? "s]" : "]"));
    ui->glSurface->repaint();
}

void MainWindow::on_action_Options_triggered()
{
    SettingsWindow* settings = new SettingsWindow(this, p->getPath());
    settings->setWindowModality(Qt::WindowModal);
    settings->show();
}

void MainWindow::on_action_Exit_MabiMe_triggered()
{
    this->close();
}

void MainWindow::repaintLayers() {
    ui->l_layers->repaint();
}

void MainWindow::onLayerCloseButtonClicked(QTreeWidgetItem *i) {
    if (i != nullptr) {
        QMessageBox msg(this);
        QString model = i->text(0).remove("$MODEL$");
        msg.setText("You are about to remove the '" + model + "' layer.");
        msg.setInformativeText("Are you sure?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setWindowTitle("MabiMe - Confirmation");
        msg.setIcon(QMessageBox::Warning);
        int result = msg.exec();
        switch (result) {
            case QMessageBox::Yes:
                ui->glSurface->deleteModel(model);
                delete i;
                break;
            case QMessageBox::No:
                break;
            default:
                break;
        }
    }
}

void MainWindow::onLayerVisibilityButtonClicked(QTreeWidgetItem *i) {
    if (i != nullptr) {
        Model *m = ui->glSurface->getModel(i->text(0).remove("$MODEL$"));
        m->isVisible = !m->isVisible;
        i->setData(0, LayerRole::LAYER_VISIBLE, m->isVisible);
        ui->glSurface->repaint();
        for (int n = 0; n < i->childCount(); n++) {
            i->setData(0, LayerRole::LAYER_VISIBLE, m->isVisible);
        }
        ui->l_layers->resizeColumnToContents(0);
//        ui->l_layers->repaint();
    }

}

void MainWindow::onLayerItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    selectedLayer = nullptr;
    if (current != nullptr) {
        if (current->text(0).startsWith("$MODEL$")) selectedLayer = current;
    }
    bool enableButtons = (current != nullptr) ? true : false;
    ui->b_duplicate_layer->setEnabled(enableButtons);
    ui->b_move_layer_up->setEnabled((ui->l_layers->indexOfTopLevelItem(current) > 0) ? enableButtons : false);
    ui->b_move_layer_down->setEnabled((ui->l_layers->indexOfTopLevelItem(current) < ui->l_layers->topLevelItemCount() - 1) ? enableButtons : false);
}

void MainWindow::on_b_move_layer_up_clicked()
{
    if (selectedLayer != nullptr) {
        int index = ui->l_layers->indexOfTopLevelItem(selectedLayer);
        if (index > 0) {
            selectedLayer = ui->l_layers->takeTopLevelItem(index);
            ui->l_layers->insertTopLevelItem(index - 1, selectedLayer);
            selectedLayer->setExpanded(true);
            ui->l_layers->setCurrentItem(selectedLayer);
            ui->l_layers->repaint();
        }
    }
}

void MainWindow::on_b_move_layer_down_clicked()
{
    if (selectedLayer != nullptr) {
        int index = ui->l_layers->indexOfTopLevelItem(selectedLayer);
        if (index < ui->l_layers->topLevelItemCount() - 1) {
            selectedLayer = ui->l_layers->takeTopLevelItem(index);
            ui->l_layers->insertTopLevelItem(index + 1, selectedLayer);
            selectedLayer->setExpanded(true);
            ui->l_layers->setCurrentItem(selectedLayer);
            ui->l_layers->repaint();
        }
    }
}

void MainWindow::on_s_zoom_sliderMoved(int position)
{
    if (floor(ui->glSurface->getCameraInfo().zoom) != position) ui->glSurface->setCameraZ(position);
}

void MainWindow::on_s_zoom_valueChanged(int value)
{
    on_s_zoom_sliderMoved(value);
}

void MainWindow::onLoadPackages() {
    if (loadPackagesAction.result()) {
        ui->g_load->setCurrentIndex(0);
        QString PMGpath = "gfx\\char\\human\\female\\";
//        insertPMG("human", PMGpath + "female_default_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "face\\female_adult01_f01", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "hair\\female_shana_hair02", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "wear\\female_summercloth01_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "shoes\\female_summercloth02_s04", PMGpath + "female_framework");
//        insertPMG("human", "gfx\\char\\chapter4\\human\\female\\mantle\\uni_c4_ego_swordwing01", PMGpath + "female_framework");
//        insertPMG("human", "gfx\\char\\chapter4\\human\\female\\mantle\\female_c4_contest2014_robe_korea01", PMGpath + "female_framework");
/*
        QString PMGpath = "gfx\\char\\monster\\mesh\\tabhartas\\tabhartas";
        insertPMG("tabhartas", PMGpath + "_mesh", PMGpath + "_framework");

        PMGpath = "gfx\\char\\chapter4\\monster\\mesh\\golem\\";
        insertPMG("golem", PMGpath + "mon_c4_albangolem_mesh", PMGpath + "mon_c4_albangolem_framework");

        PMGpath = "gfx\\char\\pet\\mesh\\";
        insertPMG("fox", PMGpath + "fox\\pet_star_fox_01_mesh", PMGpath + "fox\\pet_star_fox_01_mesh");

        PMGpath = "gfx\\char\\pet\\mesh\\bear\\";
        insertPMG("bear", PMGpath + "pet_ubipanda_mesh", PMGpath + "pet_redpanda_framework");

        PMGpath = "gfx\\char\\human\\female\\";
//        insertPMG("human", PMGpath + "female_default_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "face\\female_adult01_f01", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "hair\\female_hair46_t46", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "wear\\female_summercloth01_bss", PMGpath + "female_framework");
        insertPMG("human", PMGpath + "shoes\\female_summercloth02_s04", PMGpath + "female_framework");
        insertPMG("human", "gfx\\char\\chapter4\\human\\female\\mantle\\uni_c4_ego_swordwing01", PMGpath + "female_framework");

        PMGpath = "gfx\\char\\item\\mesh\\item_treasurechest01_i";
        insertPMG("box", PMGpath, PMGpath);
*/
    } else {
        QMessageBox msg(this);
        msg.setText("Could not detect the Mabinogi client install path.");
        msg.setInformativeText("Please check the settings and set a valid path.");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setWindowTitle("MabiMe - Warning");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        on_action_Options_triggered();
    }
}

void MainWindow::onLoadXMLUpdate(QString status, int current, int max) {
    ui->l_loadingstatus->setText("<center><b>MabiMe is loading...</b><br /><br />" + status);
    ui->p_loadingbar->setMaximum(max);
    ui->p_loadingbar->setValue(current);
}
