#include "mabimeglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>

MabiMeGLWidget::MabiMeGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

void MabiMeGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    ti += 0.1;
    glTranslatef(0.0, 0.0, -2.0);
    //glRotatef(ti, 1.0, 0.0, 0.0);
    glRotatef(ti, 0.0, 1.0, 0.0);
    //glRotatef(0, 0.0, 0.0, 1.0);
    qglColor(Qt::red);
     glBegin(GL_QUADS);
         glNormal3f(0,0,-1);
         glVertex3f(-1,-1,0);
         glVertex3f(-1,1,0);
         glVertex3f(1,1,0);
         glVertex3f(1,-1,0);

     glEnd();
     glBegin(GL_TRIANGLES);
         glNormal3f(0,-1,0.707);
         glVertex3f(-1,-1,0);
         glVertex3f(1,-1,0);
         glVertex3f(0,0,1.2);
     glEnd();
     glBegin(GL_TRIANGLES);
         glNormal3f(1,0, 0.707);
         glVertex3f(1,-1,0);
         glVertex3f(1,1,0);
         glVertex3f(0,0,1.2);
     glEnd();
     glBegin(GL_TRIANGLES);
         glNormal3f(0,1,0.707);
         glVertex3f(1,1,0);
         glVertex3f(-1,1,0);
         glVertex3f(0,0,1.2);
     glEnd();
     glBegin(GL_TRIANGLES);
         glNormal3f(-1,0,0.707);
         glVertex3f(-1,1,0);
         glVertex3f(-1,-1,0);
         glVertex3f(0,0,1.2);
     glEnd();

}

void MabiMeGLWidget::initializeGL() {
    qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);

//    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void MabiMeGLWidget::resizeGL(int width, int height) {
    // viewport things
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void MabiMeGLWidget::renderPMGMesh(PMG::Mesh mesh) {
    glVertexPointer(3, GL_FLOAT, 0, mesh.cleanVertices);
// time to enter rendering hell
}
