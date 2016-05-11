#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mabimeglwidget.h"
#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"
#include "PackReader/frmreader.h"

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
    void insertPMG(QString PMG, QString FRM = "");
    MabiPackReader *p;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
