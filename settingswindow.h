#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0, QString path = "");
    ~SettingsWindow();
    QString detectedPath = "";

private slots:
    void on_c_autodetect_toggled(bool checked);

    void on_b_cancel_clicked();

    void on_b_savesettings_clicked();

private:
    Ui::SettingsWindow *ui;
    QSettings s;
};

#endif // SETTINGSWINDOW_H
