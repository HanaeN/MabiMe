#include "mabimeglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>
#include <QDebug>
#include "GL/gl.h"

void MabiMeGLWidget::CheckError(QString error) {
    QString finalError = "";
    GLenum a = glGetError();
    if (a == GL_INVALID_ENUM) finalError = "GL_INVALID_ENUM";
    if (a == GL_INVALID_VALUE) finalError = "GL_INVALID_VALUE";
    if (a == GL_INVALID_OPERATION) finalError = "GL_INVALID_OPERATION";
    if (a == GL_INVALID_FRAMEBUFFER_OPERATION) finalError = "GL_INVALID_FRAMEBUFFER_OPERATION";
    if (a == GL_OUT_OF_MEMORY) finalError = "GL_OUT_OF_MEMORY";
    if (a == GL_STACK_UNDERFLOW) finalError = "GL_STACK_UNDERFLOW";
    if (a == GL_STACK_OVERFLOW) finalError = "GL_STACK_OVERFLOW";
    if (finalError.length() > 0) {
        finalError = error + " - " + finalError;
        qDebug() << finalError;
    }

}


void *GetAnyGLFuncAddress(const char *name)
{
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1) )
  {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }

  return p;
}

MabiMeGLWidget::MabiMeGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

void MabiMeGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    ti += 0.1;
    for (int n = 0; n < meshes.count(); n++) {
        renderPMGMesh(*meshes[n]);
    }
    glTranslatef(0.0, 0.0, 0.0);
    //glRotatef(ti, 1.0, 0.0, 0.0);
    //glRotatef(ti, 0.0, 1.0, 0.0);
    //glRotatef(0, 0.0, 0.0, 1.0);
    qglColor(Qt::red);

}

void MabiMeGLWidget::initializeGL() {
    #if defined(Q_OS_WIN)           // load extensions
        glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray");
        glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribPointer");
        glGetAttribLocation         = (PFNGLGETATTRIBLOCATIONPROC)GetAnyGLFuncAddress("glGetAttribLocation");
        glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)GetAnyGLFuncAddress("glGetShaderInfoLog");
        glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)GetAnyGLFuncAddress("glGetProgramInfoLog");
        glCreateProgram             = (PFNGLCREATEPROGRAMPROC)GetAnyGLFuncAddress("glCreateProgram");
        glCreateShader              = (PFNGLCREATESHADERPROC)GetAnyGLFuncAddress("glCreateShader");
        glShaderSource              = (PFNGLSHADERSOURCEPROC)GetAnyGLFuncAddress("glShaderSource");
        glCompileShader             = (PFNGLCOMPILESHADERPROC)GetAnyGLFuncAddress("glCompileShader");
        glAttachShader              = (PFNGLATTACHSHADERPROC)GetAnyGLFuncAddress("glAttachObjectARB");
        glLinkProgram               = (PFNGLLINKPROGRAMPROC)GetAnyGLFuncAddress("glLinkProgram");
        glUseProgram                = (PFNGLUSEPROGRAMPROC)GetAnyGLFuncAddress("glUseProgramObjectARB");
        glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation");
        glUniform1i                 = (PFNGLUNIFORM1IPROC)GetAnyGLFuncAddress("glUniform1i");
        glUniform1iv                = (PFNGLUNIFORM1IVPROC)GetAnyGLFuncAddress("glUniform1iv");
        glUniform1f                 = (PFNGLUNIFORM1FPROC)GetAnyGLFuncAddress("glUniform1f");
        glUniform2f                 = (PFNGLUNIFORM2FPROC)GetAnyGLFuncAddress("glUniform2f");
        glUniform3f                 = (PFNGLUNIFORM3FPROC)GetAnyGLFuncAddress("glUniform3f");
        glUniform4f                 = (PFNGLUNIFORM4FPROC)GetAnyGLFuncAddress("glUniform4f");
        glUniform4fv                = (PFNGLUNIFORM4FVPROC)GetAnyGLFuncAddress("glUniform4fv");
        glBindAttribLocation        = (PFNGLBINDATTRIBLOCATIONPROC)GetAnyGLFuncAddress("glBindAttribLocation");
        glGenFramebuffers           = (PFNGLGENFRAMEBUFFERSEXTPROC)GetAnyGLFuncAddress("glGenFramebuffersEXT");
        glBindFramebuffer           = (PFNGLBINDFRAMEBUFFEREXTPROC)GetAnyGLFuncAddress("glBindFramebufferEXT");
        glFramebufferTexture2D      = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GetAnyGLFuncAddress("glFramebufferTexture2D");
        glBlendFuncSeparate         = (PFNGLBLENDFUNCSEPARATEPROC)GetAnyGLFuncAddress("glBlendFuncSeparate");
        glBindBuffer                = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer");
        glBufferData                = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData");
        glGenBuffers                = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers");
        glActiveTexture             = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
    #elif defined(Q_OS_LINUX)
        glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress("glEnableVertexAttribArray");
        glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress("glVertexAttribPointer");
        glGetAttribLocation         = (PFNGLGETATTRIBLOCATIONPROC)glXGetProcAddress("glGetAttribLocation");
        glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress("glGetShaderInfoLog");
        glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress("glGetProgramInfoLog");
        glCreateProgram             = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress("glCreateProgram");
        glCreateShader              = (PFNGLCREATESHADERPROC)glXGetProcAddress("glCreateShader");
        glShaderSource              = (PFNGLSHADERSOURCEPROC)glXGetProcAddress("glShaderSource");
        glCompileShader             = (PFNGLCOMPILESHADERPROC)glXGetProcAddress("glCompileShader");
        glAttachShader              = (PFNGLATTACHSHADERPROC)glXGetProcAddress("glAttachObjectARB");
        glLinkProgram               = (PFNGLLINKPROGRAMPROC)glXGetProcAddress("glLinkProgram");
        glUseProgram                = (PFNGLUSEPROGRAMPROC)glXGetProcAddress("glUseProgramObjectARB");
        glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress("glGetUniformLocation");
        glUniform1i                 = (PFNGLUNIFORM1IPROC)glXGetProcAddress("glUniform1i");
        glUniform1iv                = (PFNGLUNIFORM1IVPROC)glXGetProcAddress("glUniform1iv");
        glUniform1f                 = (PFNGLUNIFORM1FPROC)glXGetProcAddress("glUniform1f");
        glUniform2f                 = (PFNGLUNIFORM2FPROC)glXGetProcAddress("glUniform2f");
        glUniform3f                 = (PFNGLUNIFORM3FPROC)glXGetProcAddress("glUniform3f");
        glUniform4f                 = (PFNGLUNIFORM4FPROC)glXGetProcAddress("glUniform4f");
        glUniform4fv                = (PFNGLUNIFORM4FVPROC)glXGetProcAddress("glUniform4fv");
        glBindAttribLocation        = (PFNGLBINDATTRIBLOCATIONPROC)glXGetProcAddress("glBindAttribLocation");
        glGenFramebuffers           = (PFNGLGENFRAMEBUFFERSEXTPROC)glXGetProcAddress("glGenFramebuffersEXT");
        glBindFramebuffer           = (PFNGLBINDFRAMEBUFFEREXTPROC)glXGetProcAddress("glBindFramebufferEXT");
        glFramebufferTexture2D      = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glXGetProcAddress("glFramebufferTexture2D");
        glBlendFuncSeparate         = (PFNGLBLENDFUNCSEPARATEPROC)glXGetProcAddress("glBlendFuncSeparate");
        glBindBuffer                = (PFNGLBINDBUFFERPROC)glXGetProcAddress("glBindBuffer");
        glBufferData                = (PFNGLBUFFERDATAPROC)glXGetProcAddress("glBufferData");
        glGenBuffers                = (PFNGLGENBUFFERSPROC)glXGetProcAddress("glGenBuffers");
        glActiveTexture             = (PFNGLACTIVETEXTUREPROC)glXGetProcAddress("glActiveTexture");
    #endif
    qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnableClientState(GL_COLOR_ARRAY);
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
    qglColor(Qt::red);
    /* WORKS
      glBegin(GL_QUADS);
          glNormal3f(0,0,-1);
          glVertex3f(-1,-1,0);
          glVertex3f(-1,1,0);
          glVertex3f(1,1,0);
          glVertex3f(1,-1,0);

      glEnd();
      */
    // doesnt ????
    glPushMatrix();
    qDebug() << mesh.cleanVertices[0] << mesh.cleanVertices[1] << mesh.cleanVertices[2];
    mesh.cleanVertices[0] = -1;
    mesh.cleanVertices[1] = -1;
    mesh.cleanVertices[2] = 0;
    mesh.cleanVertices[3] = -1;
    mesh.cleanVertices[4] = 1;
    mesh.cleanVertices[5] = 0;
    mesh.cleanVertices[6] = 1;
    mesh.cleanVertices[7] = 1;
    mesh.cleanVertices[8] = 0;
    mesh.cleanVertices[9] = 1;
    mesh.cleanVertices[10] = -1;
    mesh.cleanVertices[11] = 0;
    glVertexPointer(3, GL_FLOAT, 0, mesh.cleanVertices);
    CheckError("glVertexPointer");
    glColorPointer(4, GL_FLOAT, 0, mesh.cleanColours);
    CheckError("glColorPointer");
    glDrawArrays(GL_QUADS, 0, 1);
    CheckError("glDrawArrays");
    glPopMatrix();
}
