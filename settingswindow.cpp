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

    ui->l_categories->item(0)->setSizeHint(QSize(0, 40));
    ui->l_categories->setIconSize(QSize(24, 24));
    for (int i = 1; i < ui->l_categories->count(); i++) {
        ui->l_categories->item(i)->setSizeHint(QSize(0, 24));
    }
    ui->l_categories->setCurrentRow(1);
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
