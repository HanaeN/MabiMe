#include "mabimeglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>

MabiMeGLWidget::MabiMeGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

void MabiMeGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void MabiMeGLWidget::initializeGL() {
    qglClearColor(Qt::black);
}

void MabiMeGLWidget::resizeGL(int width, int height) {
    // viewport things
}
