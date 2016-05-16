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

#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QDebug>
#include <QSettings>

SettingsWindow::SettingsWindow(QWidget *parent, QString path) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    setWindowFlags(Qt::Dialog);
    this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    detectedPath = path;
#if defined(Q_OS_WIN)
    bool canAutoDetect = true;
#else
    bool canAutoDetect = false;
#endif
    ui->c_autodetect->setChecked(s.value("AutoDetectEnabled", canAutoDetect).toBool());
    ui->e_autodetect->setText(s.value("ClientPath", "").toString());
    ui->l_categories->setIconSize(QSize(24, 24));
    for (int i = 1; i < ui->l_categories->count(); i++) {
        ui->l_categories->item(i)->setSizeHint(QSize(0, 40));
    }
    ui->l_categories->setCurrentRow(0);
    this->on_c_autodetect_toggled(ui->c_autodetect->isChecked());

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_c_autodetect_toggled(bool checked)
{
    ui->l_autodetect->setEnabled(!checked);
    ui->b_autodetect->setEnabled(!checked);
    ui->e_autodetect->setEnabled(!checked);
    ui->e_autodetect->setText(checked ? "detected: <" + detectedPath + ">" : s.value("ClientPath", "").toString());
}

void SettingsWindow::on_b_cancel_clicked()
{
    this->close();
}

void SettingsWindow::on_b_savesettings_clicked()
{
    s.setValue("AutoDetectEnabled", ui->c_autodetect->isChecked());
    s.setValue("ClientPath", ui->c_autodetect->isChecked() ? "" : ui->e_autodetect->text());
    this->close();
}
