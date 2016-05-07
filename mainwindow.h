#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mabimeglwidget.h"
#include "PackReader/pmgreader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show();
private slots:
    void startTimer();
    void cameraChange(CameraInfo camera);
private:
    PMGReader r;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
