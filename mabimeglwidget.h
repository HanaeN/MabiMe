#ifndef MABIMEGLWIDGET_H
#define MABIMEGLWIDGET_H

#include <QGLWidget>
#include <QList>
#include <QPoint>
#include <QVector3D>

#include "PackReader/pmgreader.h"
#include "PackReader/frmreader.h"

struct Rotation {
    int pitch = 0, yaw = 0, roll = 0;
};

struct CameraInfo {
    float x = 0, y = 40, zoom = -170;
    Rotation rotation;
};
struct PMGTexture {
    QString name;
    QImage img;
    GLuint texture = -1;
};

struct PMGObject {
    QList<PMG::Mesh*> meshes;
    QList<PMGTexture*> textures;
    QList<FRM::Bone*> bones;
};

class MabiMeGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MabiMeGLWidget(QWidget *parent = 0);
    CameraInfo getCameraInfo();
    bool addPMG(PMGObject *pmg);
private:
    GLuint frmTexture;
    CameraInfo camera;
    QPoint drag;
    QPointF oldCameraPos = QPointF(0, 40);
    QList<PMGObject*> objects = QList<PMGObject*>();
    Rotation oldCameraRotation;
    GLhandleARB boneShader;
    bool isLeftDragging = false;
    bool isRightDragging = false;
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void checkError(QString error, bool suppress = false);
    bool loadTexture(PMGTexture *t, bool useFiltering = false);
    GLuint loadTexture(QString filename, bool useFiltering = false);
    GLfloat vertexList[72] = {
        1, -1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1,
        1, -1, 1, 1, 1, 1, -1, 1, 1, -1, -1, 1,
        1, -1, -1, 1, 1, -1, 1, 1, 1, 1, -1, 1,
        -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1,
        1, 1, 1, 1, 1, -1, -1, 1, -1, -1, 1, 1,
        1, -1, -1, 1, -1, 1, -1, -1, 1, -1, -1, -1
    };
    GLfloat vertexUV[48] = {
        0, 0, 0, 1, 1, 0, 1, 1,
        0, 0, 0, 1, 1, 0, 1, 1,
        0, 0, 0, 1, 1, 0, 1, 1,
        0, 0, 0, 1, 1, 0, 1, 1
    };
    float ti = 0 ;
    PFNGLCLIENTACTIVETEXTUREPROC     glClientActiveTexture;
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
    PFNGLUNIFORM1FVPROC              glUniform1fv;
    PFNGLUNIFORM2FPROC               glUniform2f;
    PFNGLUNIFORM3FPROC               glUniform3f;
    PFNGLUNIFORM4FPROC               glUniform4f;
    PFNGLUNIFORM4FVPROC              glUniform4fv;
    PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
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
    void draw();
    void resizeGL(int width, int height);
    void renderPMGMesh(PMG::Mesh mesh, QList<FRM::Bone*> *bones = nullptr, PMGTexture *t = nullptr);
    GLhandleARB linkShader(QString vp_str, QString fp_str);
    void useShader(GLhandleARB shader);
    void endShader();
    void setShaderVariableInt(GLhandleARB shader, QString varname, int data);
    void setShaderVariableFloat(GLhandleARB shader, QString varname, float data);
    void setShaderArrayFloat(GLhandleARB shader, QString varname, float *data, int arraySize);
    void setShaderVariableMatrix(GLhandleARB shader, QString varname, QMatrix4x4 matrix);
    void setShaderTextures(GLhandleARB shader, int count = 1);
signals:
    void cameraChange(CameraInfo camera);
public slots:
};

#endif // MABIMEGLWIDGET_H
