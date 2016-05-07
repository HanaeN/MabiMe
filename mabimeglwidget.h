#ifndef MABIMEGLWIDGET_H
#define MABIMEGLWIDGET_H

#include <QGLWidget>
#include <QList>
#include "PackReader/pmgreader.h"

class MabiMeGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MabiMeGLWidget(QWidget *parent = 0);
    QList<PMG::Mesh*> meshes = QList<PMG::Mesh*>();
private:
    void CheckError(QString error);
    float ti = 0 ;
    PFNGLCREATEPROGRAMPROC           glCreateProgram;
    PFNGLCREATESHADERPROC            glCreateShader;
    PFNGLSHADERSOURCEPROC            glShaderSource;
    PFNGLCOMPILESHADERPROC           glCompileShader;
    PFNGLATTACHSHADERPROC            glAttachShader;
    PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
    PFNGLLINKPROGRAMPROC             glLinkProgram;
    PFNGLUSEPROGRAMPROC              glUseProgram;
    PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
    PFNGLUNIFORM1IPROC               glUniform1i;
    PFNGLUNIFORM1IVPROC              glUniform1iv;
    PFNGLUNIFORM1FPROC               glUniform1f;
    PFNGLUNIFORM2FPROC               glUniform2f;
    PFNGLUNIFORM3FPROC               glUniform3f;
    PFNGLUNIFORM4FPROC               glUniform4f;
    PFNGLUNIFORM4FVPROC              glUniform4fv;
    PFNGLBINDATTRIBLOCATIONPROC      glBindAttribLocation;
    PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
    PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
    PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLGENFRAMEBUFFERSEXTPROC      glGenFramebuffers;
    PFNGLBINDFRAMEBUFFEREXTPROC      glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
    PFNGLBLENDFUNCSEPARATEPROC       glBlendFuncSeparate;
    PFNGLBINDBUFFERPROC              glBindBuffer;
    PFNGLBUFFERDATAPROC              glBufferData;
    PFNGLGENBUFFERSPROC              glGenBuffers;
    PFNGLACTIVETEXTUREPROC           glActiveTexture;
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void renderPMGMesh(PMG::Mesh mesh);
signals:

public slots:
};

#endif // MABIMEGLWIDGET_H
