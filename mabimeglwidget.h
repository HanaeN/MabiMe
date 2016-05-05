#ifndef MABIMEGLWIDGET_H
#define MABIMEGLWIDGET_H

#include <QGLWidget>
#include "PackReader/pmgreader.h"

class MabiMeGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MabiMeGLWidget(QWidget *parent = 0);
private:
    float ti = 0 ;
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void renderPMGMesh(PMG::Mesh mesh);
signals:

public slots:
};

#endif // MABIMEGLWIDGET_H
