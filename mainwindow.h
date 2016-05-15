#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mabimeglwidget.h"
#include "PackReader/packmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void close();
    void show();
private slots:
    void startTimer();
    void cameraChange(CameraInfo camera);
private:
    void insertPMG(QString modelName, QString PMG, QString FRM = "");
    void loadPackages();
    PackManager *p;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
