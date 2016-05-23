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

#ifndef MABIMEGLWIDGET_H
#define MABIMEGLWIDGET_H

#include <QGLWidget>
#include <QList>
#include <QPoint>
#include <QOpenGLDebugMessage>
#include <QOpenGLDebugLogger>
#include <QVector3D>

#include "model.h"

struct Rotation {
    int pitch = 0, yaw = 0, roll = 0;
};

struct CameraInfo {
    float x = 0, y = 40, zoom = -170;
    Rotation rotation;
};

class MabiMeGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MabiMeGLWidget(QWidget *parent = 0);
    ~MabiMeGLWidget();
    CameraInfo getCameraInfo();
    bool addModel(Model *model);
    void deleteModel(QString modelName);
    void updateModel(QString modelName);
    int getModelCount();
    Model* getModel(QString modelName);
    void setCameraZ(float z);
private:
    GLuint frmTexture;
    CameraInfo camera;
    QPoint drag;
    QPointF oldCameraPos = QPointF(0, 40);
    QList<Model*> models = QList<Model*>();
    Rotation oldCameraRotation;
    GLhandleARB boneShader;
    GLint attribVertexXYZ, attribVertexNXYZ, attribVertexRGBA, attribVertexUV, attribVertexBoneWeight, attribVertexBoneID;
    QOpenGLDebugLogger *logger = nullptr;
    bool isLeftDragging = false;
    bool isRightDragging = false;
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void checkError(QString error, bool suppress = false);
    bool loadTexture(PMGTexture *t, bool useFiltering = false);
    GLuint loadTexture(QString filename, bool useFiltering = false);
    GLfloat vertexList[12] = {
       -50, -50, -10, -50, 50, -10, 50, 50, -10, 50, -50, -10
    };
    GLfloat vertexUV[8] = {
        0, 0, 0, 50, 50, 50, 50, 0
    };
    float ti = 0 ;
    PFNGLCLIENTACTIVETEXTUREPROC        glClientActiveTexture;
    PFNGLCREATEPROGRAMPROC              glCreateProgram;
    PFNGLCREATESHADERPROC               glCreateShader;
    PFNGLSHADERSOURCEPROC               glShaderSource;
    PFNGLCOMPILESHADERPROC              glCompileShader;
    PFNGLATTACHSHADERPROC               glAttachShader;
    PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
    PFNGLLINKPROGRAMPROC                glLinkProgram;
    PFNGLUSEPROGRAMPROC                 glUseProgram;
    PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
    PFNGLUNIFORM1IPROC                  glUniform1i;
    PFNGLUNIFORM1IVPROC                 glUniform1iv;
    PFNGLUNIFORM1FPROC                  glUniform1f;
    PFNGLUNIFORM1FVPROC                 glUniform1fv;
    PFNGLUNIFORM2FPROC                  glUniform2f;
    PFNGLUNIFORM3FPROC                  glUniform3f;
    PFNGLUNIFORM4FPROC                  glUniform4f;
    PFNGLUNIFORM4FVPROC                 glUniform4fv;
    PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;
    PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation;
    PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
    PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
    PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation;
    PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
    PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffers;
    PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC       glFramebufferTexture2D;
    PFNGLBLENDFUNCSEPARATEPROC          glBlendFuncSeparate;
    PFNGLBINDBUFFERPROC                 glBindBuffer;
    PFNGLBUFFERDATAPROC                 glBufferData;
    PFNGLGENBUFFERSPROC                 glGenBuffers;
    PFNGLACTIVETEXTUREPROC              glActiveTexture;
    PFNGLGETSHADERIVPROC                glGetShaderiv;
    PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays;
    PFNGLBINDVERTEXARRAYPROC            glBindVertexArray;
    PFNGLDEBUGMESSAGECONTROLPROC        glDebugMessageControl;
protected:
    void initializeGL();
    void paintGL();
    void draw();
    void resizeGL(int width, int height);
    void renderPMGMesh(PMG::Mesh mesh, ModelColours *colours = nullptr, PMGColours *pmgColours = nullptr, QList<Bone *> bones = QList<Bone*>(), GLuint texture = 0);
    GLhandleARB linkShader(QString vp_str, QString fp_str);
    void useShader(GLhandleARB shader);
    void endShader();
    GLuint vao, vbo;
    void setShaderVariableInt(GLhandleARB shader, QString varname, int data);
    void setShaderVariableFloat(GLhandleARB shader, QString varname, float data);
    void setShaderVariableFloat(GLhandleARB shader, QString varname, QVector3D vec3);
    void setShaderVariableFloat(GLhandleARB shader, QString varname, QVector4D vec4);
    void setShaderArrayFloat(GLhandleARB shader, QString varname, float *data, int arraySize);
    void setShaderArrayInt(GLhandleARB shader, QString varname, int *data, int arraySize);
    void setShaderVariableMatrix(GLhandleARB shader, QString varname, QMatrix4x4 matrix);
    void setShaderArrayMatrix(GLhandleARB shader, QString varname);
    void setShaderTextures(GLhandleARB shader, int count = 1);
    GLfloat boneMatriceBuffer[64 * 4] = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1
    };
signals:
    void cameraChange(CameraInfo camera);
public slots:
    void debugLog(QOpenGLDebugMessage msg);
};

#endif // MABIMEGLWIDGET_H
