#include "mabimeglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>
#include <QDebug>
#include "GL/gl.h"
#include "GL/glu.h"


#if defined(Q_OS_WIN)
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
#elif defined (Q_OS_LINUX)
    void *GetAnyGLFuncAddress(const char *name)
    {
        void *p = (void *)glXGetProcAddress(name);
        return p;
    }
#endif


QByteArray readTextFile(QString filename) {
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString data = in.readAll();
    file.close();
    qDebug() << data.toLatin1();
    return data.toLatin1();
}

void MabiMeGLWidget::checkError(QString error, bool suppress) {
    QString finalError = "";
    GLenum a = glGetError();
    if (suppress) return;
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


MabiMeGLWidget::MabiMeGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::StencilBuffer | QGL::DirectRendering | QGL::SampleBuffers | QGL::DeprecatedFunctions), parent)
{
//    this->setFormat(QGLFormat());
}
void MabiMeGLWidget::draw() {
    for (int i = 0; i < objects.count(); i++) {
        PMGObject *o = objects[i];
        for (int n = 0; n < o->meshes.count(); n++) {
            PMGTexture *t = nullptr;
            FRM::Bone *b = nullptr;
            for (int i = 0; i < o->textures.count(); i++) {
                if (o->textures[i]->name == o->meshes[n]->textureName) {
                    t = o->textures[i];
                    break;
                }
            }
            for (int i = 0; i < o->bones.count(); i++) {
                if (o->bones[i]->name == o->meshes[n]->boneName) {
                    b = o->bones[i];
                    break;
                }
            }
            QList<FRM::Bone*> bl = QList<FRM::Bone*>();
            if (b != nullptr) {
                while (true) {
                    bl.insert(0, b);
                    char parent = b->parentID;
                    b = nullptr;
                    for (int i = 0; i < o->bones.count(); i++) {
                        if (o->bones[i]->boneID == parent) {
                            b = o->bones[i];
                            break;
                        }
                    }
                    if (b == nullptr) break;
                }
            }
            if (bl.count() > 0) {
                renderPMGMesh(*o->meshes[n], &bl, t);
            } else {
                renderPMGMesh(*o->meshes[n], nullptr, t);
            }
        }
    }
}

void MabiMeGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(camera.x, camera.y, camera.zoom);
    qglColor(Qt::red);

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glClearStencil(0);
    glClear( GL_STENCIL_BUFFER_BIT );
    glEnable( GL_STENCIL_TEST );
    glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    useShader(boneShader);
    setShaderVariableInt(boneShader, "isOutline", 0);
    setShaderTextures(boneShader);
    draw();
    glStencilFunc( GL_NOTEQUAL, 1, 0xFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
    glLineWidth(2.5f);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(0.0f, 0.0f, 0.0f );
    glDisableClientState(GL_COLOR_ARRAY);
    setShaderVariableInt(boneShader, "isOutline", 1);
    draw();
    endShader();
    glEnableClientState(GL_COLOR_ARRAY);
    glPopAttrib();
}

void MabiMeGLWidget::initializeGL() {
    // load extensions
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
    glUniform1fv                = (PFNGLUNIFORM1FVPROC)GetAnyGLFuncAddress("glUniform1fv");
    glUniform2f                 = (PFNGLUNIFORM2FPROC)GetAnyGLFuncAddress("glUniform2f");
    glUniform3f                 = (PFNGLUNIFORM3FPROC)GetAnyGLFuncAddress("glUniform3f");
    glUniform4f                 = (PFNGLUNIFORM4FPROC)GetAnyGLFuncAddress("glUniform4f");
    glUniform4fv                = (PFNGLUNIFORM4FVPROC)GetAnyGLFuncAddress("glUniform4fv");
    glUniformMatrix4fv          = (PFNGLUNIFORMMATRIX4FVPROC)GetAnyGLFuncAddress("glUniformMatrix4fv");
    glBindAttribLocation        = (PFNGLBINDATTRIBLOCATIONPROC)GetAnyGLFuncAddress("glBindAttribLocation");
    glGenFramebuffers           = (PFNGLGENFRAMEBUFFERSEXTPROC)GetAnyGLFuncAddress("glGenFramebuffersEXT");
    glBindFramebuffer           = (PFNGLBINDFRAMEBUFFEREXTPROC)GetAnyGLFuncAddress("glBindFramebufferEXT");
    glFramebufferTexture2D      = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GetAnyGLFuncAddress("glFramebufferTexture2D");
    glBlendFuncSeparate         = (PFNGLBLENDFUNCSEPARATEPROC)GetAnyGLFuncAddress("glBlendFuncSeparate");
    glBindBuffer                = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer");
    glBufferData                = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData");
    glGenBuffers                = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers");
    glActiveTexture             = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
    glClientActiveTexture       = (PFNGLCLIENTACTIVETEXTUREPROC)GetAnyGLFuncAddress("glClientActiveTexture");
    qglClearColor(QColor(200, 200, 200));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    boneShader = linkShader("Shaders/bone.v", "Shaders/bone.f");
    emit cameraChange(camera);
}

void Perspective( GLdouble fov, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    GLdouble m[16] = { 1,0,0,0,0,1,0,0,0,0,1,-1,0,0,0,0 };
    double sine, cotangent, deltaZ;
    double radians = fov / 2 * 3.14159265358979323846 / 180;

    deltaZ = zFar - zNear;
    sine = sin(radians);
    if ((deltaZ == 0) || (sine == 0) || (aspect == 0)) {
    return;
    }
    cotangent = cos(radians) / sine;

    m[0] = cotangent / aspect;
    m[5] = cotangent;
    m[10] = -(zFar + zNear) / deltaZ;
    m[14] = -2 * zNear * zFar / deltaZ;
    glMultMatrixd(&m[0]);
}

void MabiMeGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    Perspective(60, (GLfloat)width / (GLfloat)height, 1.0, 4000.0);
    glMatrixMode(GL_MODELVIEW);
}

void MabiMeGLWidget::renderPMGMesh(PMG::Mesh mesh, QList<FRM::Bone *> *bones, PMGTexture *t) {
    glPushMatrix();
    glRotatef(camera.rotation.pitch, 1.0, 0.0, 0.0);
    glRotatef(camera.rotation.yaw, 0.0, 1.0, 0.0);
    if (bones != nullptr) {
        QMatrix4x4 m, m2, mult;
        // build bone matrix
        for (int i = 0; i < bones->count(); i++) {
            memcpy(m2.data(), bones->at(i)->link, 64);
            if (strcmp(bones->at(i)->name, "head") == 0) m2.translate(camera.x / 1000, 0, 0);
            m = (i > 0) ? m * m2 : m2;
        }
        // translate to local space for morphing
        memcpy(m2.data(), bones->last()->globalToLocal, 64);
        m = m * m2;
        m.setRow(3, QVector4D(0, 0, 0, 1));
        setShaderArrayFloat(boneShader, "boneWeight[0]", mesh.cleanBoneWeights, mesh.cleanVertexCount);
        setShaderVariableMatrix(boneShader, "boneMatrix", m);
        setShaderVariableMatrix(boneShader, "worldMatrix", mesh.majorMatrix.transposed());
    } else {
        QMatrix4x4 m;
        m.setRow(0, QVector4D(1, 0, 0, 0));
        m.setRow(1, QVector4D(0, 1, 0, 0));
        m.setRow(2, QVector4D(0, 0, 1, 0));
        m.setRow(3, QVector4D(0, 0, 0, 1));
        setShaderVariableMatrix(boneShader, "boneMatrix", m);
        setShaderVariableMatrix(boneShader, "worldMatrix", mesh.majorMatrix.transposed());
    }
    glVertexPointer(3, GL_FLOAT, 0, mesh.cleanVertices);
    glColorPointer(4, GL_FLOAT, 0, mesh.cleanColours);
    glNormalPointer(GL_FLOAT, 0, mesh.cleanNormals);
    glTexCoordPointer(2, GL_FLOAT, 0, mesh.cleanTextureCoords);
    glActiveTexture(GL_TEXTURE0);
    if (t != nullptr) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, t->texture);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glDrawArrays(GL_TRIANGLES, 0, mesh.cleanVertexCount);
    glPopMatrix();
}

void MabiMeGLWidget::mousePressEvent(QMouseEvent *event) {
    if (isLeftDragging || isRightDragging) return;
    if (event->buttons() == Qt::LeftButton) {
        isLeftDragging = true;
        drag = QPoint(event->x(), event->y());
        oldCameraPos = QPoint(camera.x, camera.y);
        return;
    } else {
        isLeftDragging = false;
    }
    if (event->buttons() == Qt::RightButton) {
        isRightDragging = true;
        drag = QPoint(event->x(), event->y());
        oldCameraRotation = camera.rotation;
        return;
    } else {
        isLeftDragging = false;
    }
}
void MabiMeGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->buttons() != Qt::LeftButton) isLeftDragging = false;
    if (event->buttons() != Qt::RightButton) isRightDragging = false;
}
void MabiMeGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isLeftDragging) {
        camera.x = oldCameraPos.x() + ((event->x() - drag.x()) / 4);
        camera.y = oldCameraPos.y() - ((event->y() - drag.y()) / 4);
    }
    if (isRightDragging) {
        camera.rotation.pitch = oldCameraRotation.pitch + (event->y() - drag.y());
        camera.rotation.yaw = oldCameraRotation.yaw + (event->x() - drag.x());
        camera.rotation.pitch = camera.rotation.pitch % 360;
        if (camera.rotation.pitch < 0) camera.rotation.pitch += 360;
        camera.rotation.yaw = camera.rotation.yaw % 360;
        if (camera.rotation.yaw < 0) camera.rotation.yaw += 360;
    }
    if (event->x() != drag.x() || event->y() != drag.y()) {
        this->repaint();
        emit cameraChange(camera);
    }
}

void MabiMeGLWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        camera.zoom += 10;
        this->repaint();
        emit cameraChange(camera);
    } else if(event->angleDelta().y() < 0) {
        camera.zoom -= 10;
        this->repaint();
        emit cameraChange(camera);
    }
}

bool MabiMeGLWidget::loadTexture(PMGTexture *t, bool useFiltering) {
    glGenTextures(1, &t->texture);
    glBindTexture(GL_TEXTURE_2D, t->texture);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (!useFiltering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, t->img.width(), t->img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, t->img.bits());
    return true;
}

bool MabiMeGLWidget::addPMG(PMGObject *pmg) {
    try {
        for (int i = 0; i < pmg->textures.count(); i++) {
            loadTexture(pmg->textures[i], true);
        }
        objects.append(pmg);
        return true;
    } catch (...) {
        return false;
    }
}


GLhandleARB MabiMeGLWidget::linkShader(QString vp_str, QString fp_str)
{
    GLhandleARB v,f,p;
    QByteArray vs, fs;
    v = glCreateShader(GL_VERTEX_SHADER);
    checkError("glCreateShader(GL_VERTEX_SHADER)");
    f = glCreateShader(GL_FRAGMENT_SHADER);
    checkError("f = glCreateShader(GL_FRAGMENT_SHADER)");
    vs = readTextFile(vp_str);
    fs = readTextFile(fp_str);
    char *cvs = vs.data();
    char *cfs = fs.data();
    glShaderSource(v, 1, (const GLcharARB**) &cvs, NULL);
    checkError("glShaderSource(v, 1, (const GLcharARB**) &cvs, NULL)");
    glShaderSource(f, 1, (const GLcharARB**) &cfs, NULL);
    checkError("glShaderSource(f, 1, (const GLcharARB**) &cfs, NULL)");
    glCompileShader(v);
    checkError("glCompileShader(v)");
    glCompileShader(f);
    checkError("glCompileShader(f)");
    vs.clear();
    fs.clear();

    p = glCreateProgram();
    checkError("p = glCreateProgram()");
    glAttachShader(p,v);
    checkError("glAttachShader(p,v)");
    glAttachShader(p,f);
    checkError("glAttachShader(p,f)");
    glLinkProgram(p);
    checkError("glLinkProgram(p)");
    return p;
}

void MabiMeGLWidget::useShader(GLhandleARB shader) {
    glUseProgram(shader);
    checkError("glUseProgram(shader)");
}

void MabiMeGLWidget::endShader() {
    glUseProgram(0);
    checkError("glUseProgram(0)");
}

void MabiMeGLWidget::setShaderVariableInt(GLhandleARB shader, QString varname, int data) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform1i(id, data);
        checkError("glUniform1i(id [" + varname + "], " + QString::number(data) + ")");
    }
}

void MabiMeGLWidget::setShaderVariableFloat(GLhandleARB shader, QString varname, float data) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform1f(id, data);
        checkError("glUniform1f(id [" + varname + "], " + QString::number(data) + ")");
    }
}

void MabiMeGLWidget::setShaderTextures(GLhandleARB shader, int count) {
    GLint id = glGetUniformLocation(shader, "textureIn");
    checkError("glGetUniformLocation[textureIn]");
    if (id != -1) {
        glUniform1i(id, 0);
        checkError("glUniform1i(id [textureIn], 0)");
    }
    if (count > 1) {
        GLint id = glGetUniformLocation(shader, "texture2");
        checkError("glGetUniformLocation[texture2]");
        if (id != -1) {
            glUniform1i(id, 1);
            checkError("glUniform1i(id [texture2], 0)");
        }
    }
}

void MabiMeGLWidget::setShaderVariableMatrix(GLhandleARB shader, QString varname, QMatrix4x4 matrix) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniformMatrix4fv(id, 1, GL_FALSE, matrix.constData());
        checkError("glUniformMatrix4fv(id [" + varname + "], <ptr>)", true); // suppress error - there is a quirk in openGL to always return 0 for this
    }
}

void MabiMeGLWidget::setShaderArrayFloat(GLhandleARB shader, QString varname, float *data, int arraySize) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform1fv (id, arraySize, &data[0]);
        checkError("glUniform1fv(id [" + varname + "], <size>" + QString::number(arraySize) + ", <ptr>)", false);
    }
}
