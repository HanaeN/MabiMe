#ifndef MABIMEGLWIDGET_H
#define MABIMEGLWIDGET_H

#include <QGLWidget>

class MabiMeGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MabiMeGLWidget(QWidget *parent = 0);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
signals:

public slots:
};

#endif // MABIMEGLWIDGET_H
