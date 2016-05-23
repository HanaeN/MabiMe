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

#include "mabimeglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLContext>
#include <QDebug>
#include <QMouseEvent>
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
    // init
}



void MabiMeGLWidget::draw() {
    for (int i = 0; i < models.count(); i++) {
        Model *o = models[i];
        if (o->isVisible) {
            for (int index = 0; index < o->models.count(); index++) {
                PMGModel *pmgModel = o->models[index];
                for (int n = 0; n < pmgModel->meshes.count(); n++) {
                    PMGTexture t;
                    for (int i = 0; i < o->textures.count(); i++) {
                        if (o->textures[i].name == pmgModel->meshes[n]->textureName) {
                            t = o->textures.at(i);
                            break;
                        }
                    }
                    QList<Bone*> bones = QList<Bone*>();
                    if (o->hasBoneTree()) {
                        for (int i = 0; i < pmgModel->meshes[n]->boneNames.count(); i++) {
                            Bone *b = o->findBone(pmgModel->meshes[n]->boneNames[i]);
                            // first entry is always boneName - if it fails, try boneName2, similar to bone link detection (why devCAT??)
                            if (i == 0 && b == nullptr) b = o->findBone(pmgModel->meshes[n]->boneName2);
                            bones.append(b != nullptr ? b : o->getRootBone());
                        }
                    }
                    renderPMGMesh(*pmgModel->meshes[n], &o->colours, &pmgModel->colours, bones, t.texture);
                }
            }
        }
    }
}

void MabiMeGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();


    // draw grid
    glDisable(GL_LIGHTING);
//    glDisableClientState(GL_COLOR_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);

/*
    glVertexPointer(3, GL_FLOAT, 0, vertexList);
    glTexCoordPointer(2, GL_FLOAT, 0, vertexUV);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, frmTexture);
    glDrawArrays(GL_QUADS, 0, 4);
*/
//    glEnableClientState(GL_COLOR_ARRAY);
//    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);

    glTranslatef(camera.x, camera.y, camera.zoom);
    qglColor(Qt::red);

    glColor3f(1.0f, 1.0f, 1.0f );
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
    setShaderVariableInt(boneShader, "isOutline", 1);
    draw();
    endShader();
    glPopAttrib();
}

void MabiMeGLWidget::initializeGL() {
    // load extensions
    glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray  = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glDisableVertexAttribArray");
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
    glGetShaderiv               = (PFNGLGETSHADERIVPROC)GetAnyGLFuncAddress("glGetShaderiv");
    glGenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFuncAddress("glGenVertexArrays");
    glBindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFuncAddress("glBindVertexArray");
    glDebugMessageControl       = (PFNGLDEBUGMESSAGECONTROLPROC)GetAnyGLFuncAddress("glDebugMessageControl");


    bool isLogging = false; // openGL logging
    if (isLogging) {
        logger = new QOpenGLDebugLogger(this);
        connect(logger, SIGNAL(messageLogged(QOpenGLDebugMessage)), SLOT(debugLog(QOpenGLDebugMessage)));
        logger->initialize();
        logger->startLogging();
    }

    qglClearColor(QColor(200, 200, 200));
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 1);

    frmTexture = loadTexture("Images/frm.png");
    boneShader = linkShader("Shaders/bone.v", "Shaders/bone.f");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    attribVertexXYZ = glGetAttribLocation(boneShader, "vertexPos");
    attribVertexNXYZ = glGetAttribLocation(boneShader, "vertexNormal");
    attribVertexRGBA = glGetAttribLocation(boneShader, "vertexRGBA");
    attribVertexUV = glGetAttribLocation(boneShader, "vertexUV");
    attribVertexBoneWeight = glGetAttribLocation(boneShader, "boneWeight");
    attribVertexBoneID = glGetAttribLocation(boneShader, "boneID");
    qDebug() << attribVertexXYZ << attribVertexNXYZ << attribVertexRGBA << attribVertexUV << attribVertexBoneWeight << attribVertexBoneID;
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

void MabiMeGLWidget::renderPMGMesh(PMG::Mesh mesh, ModelColours *colours, PMGColours *pmgColours, QList<Bone*> bones, GLuint texture) {
    if (!mesh.showMesh) return;
    glPushMatrix();
    glRotatef(camera.rotation.pitch, 1.0, 0.0, 0.0);
    glRotatef(camera.rotation.yaw, 0.0, 1.0, 0.0);
    if (bones.count() > 0) {
        for (int n = 0; n < bones.count(); n++) {
            setShaderVariableMatrix(boneShader, "boneMatrix[" + QString::number(n) + "]", bones[n]->getLocalMatrix());
        }
    } else glMultMatrixf(mesh.majorMatrix.constData());
    glActiveTexture(GL_TEXTURE0);
    if (texture > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, texture);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PMG::ShaderVertex) * mesh.vertexCount, mesh.shaderVertices, GL_STATIC_DRAW);
    if (attribVertexRGBA != -1) {
        glEnableVertexAttribArray(attribVertexRGBA);
        glVertexAttribPointer(attribVertexRGBA, 4, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), (void*)(offsetof(PMG::ShaderVertex, rgba)));
    }
    if (attribVertexXYZ != -1) {
        glEnableVertexAttribArray(attribVertexXYZ);
        glVertexAttribPointer(attribVertexXYZ, 3, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), 0);
    }
    if (attribVertexNXYZ != -1) {
        glEnableVertexAttribArray(attribVertexNXYZ);
        glVertexAttribPointer(attribVertexNXYZ, 3, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), (void*)(offsetof(PMG::ShaderVertex, nxyz)));
    }
    if (attribVertexUV != -1) {
        glEnableVertexAttribArray(attribVertexUV);
        glVertexAttribPointer(attribVertexUV, 2, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), (void*)(offsetof(PMG::ShaderVertex, uv)));
    }
    if (attribVertexBoneWeight != -1) {
        glEnableVertexAttribArray(attribVertexBoneWeight);
        glVertexAttribPointer(attribVertexBoneWeight, 2, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), (void*)(offsetof(PMG::ShaderVertex, boneWeight)));
    }
    if (attribVertexBoneID != -1) {
        glEnableVertexAttribArray(attribVertexBoneID);
        glVertexAttribPointer(attribVertexBoneID, 1, GL_FLOAT, GL_FALSE, sizeof(PMG::ShaderVertex), (void*)(offsetof(PMG::ShaderVertex, boneID)));
    }
    // map dye parts
    QVector4D modifyRGB(0, 0, 0, 0);
    QColor colourGrab = QColor(0, 0, 0, 0);
    if (mesh.colourMap == "skin") colourGrab = colours->skin;
    if (mesh.colourMap == "hair" || mesh.colourMap == "h") colourGrab = colours->hair;
    if (mesh.colourMap == "e") colourGrab = colours->eye;

    modifyRGB.setX(colourGrab.red() / 255.0);
    modifyRGB.setY(colourGrab.green() / 255.0);
    modifyRGB.setZ(colourGrab.blue() / 255.0);
    modifyRGB.setW(colourGrab.alpha() / 255.0);
/*
} else {
        modifyRGB.setX(30.0 / 255.0);
        modifyRGB.setY(30.0 / 255.0);
        modifyRGB.setZ(30.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "c1") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(255.0 / 255.0);
        modifyRGB.setZ(255.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "c2") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(140.0 / 255.0);
        modifyRGB.setZ(140.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "c3" || mesh.colourMap == "s2") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(120.0 / 255.0);
        modifyRGB.setZ(120.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "s1") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(255.0 / 255.0);
        modifyRGB.setZ(255.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "e") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(50.0 / 255.0);
        modifyRGB.setZ(50.0 / 255.0);
        modifyRGB.setW(1);
    }
    if (mesh.colourMap == "hair") {
        modifyRGB.setX(255.0 / 255.0);
        modifyRGB.setY(80.0 / 255.0);
        modifyRGB.setZ(80.0 / 255.0);
        modifyRGB.setW(1);
    }
*/
    setShaderVariableFloat(boneShader, "modifyRGB", modifyRGB);

    glDrawElements(GL_TRIANGLES, mesh.faceVertexCount, GL_UNSIGNED_INT, mesh.vertexList);
    if (attribVertexXYZ != -1) glDisableVertexAttribArray(attribVertexXYZ);
    if (attribVertexNXYZ != -1) glDisableVertexAttribArray(attribVertexNXYZ);
    if (attribVertexUV != -1) glDisableVertexAttribArray(attribVertexUV);
    if (attribVertexRGBA != -1) glDisableVertexAttribArray(attribVertexRGBA);
    if (attribVertexBoneWeight != -1) glDisableVertexAttribArray(attribVertexBoneWeight);
    if (attribVertexBoneID != -1) glDisableVertexAttribArray(attribVertexBoneID);
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
//        getModel("human")->findBone("chest")->setY(camera.x);
//        getModel("human")->findBone("leg1l")->setX(camera.x);
//        getModel("human")->findBone("leg1r")->setX(-camera.x);
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
    t->loaded = true;
    return true;
}

GLuint MabiMeGLWidget::loadTexture(QString filename, bool useFiltering) {
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
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
    QImage t(filename);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, t.width(), t.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, t.bits());
    return result;
}

bool MabiMeGLWidget::addModel(Model *model) {
    try {
        // load textures that havent been loaded yet
        for (int i = 0; i < model->textures.count(); i++) {
            if (!model->textures[i].loaded) loadTexture(&model->textures[i], true);
        }
        // add the model if it doesnt exist
        if (!models.contains(model)) {
            models.append(model);
        }
        return true;
    } catch (...) {
        qDebug() << "failed to add model";
        return false;
    }
}

void MabiMeGLWidget::updateModel(QString modelName) {
    try {
        for (int i = 0; i < models.count(); i++) {
            if (models[i]->getName() == modelName) {
                addModel(models[i]);
                return;
            }
        }
    } catch (...) {
        qDebug() << "failed to update model";
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

    GLint success = 0;
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        qDebug() << "failed to compile vertex shader";
        GLint maxLength = 0;
        glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);
        qDebug() << errorLog.data();
    }

    success = 0;
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        qDebug() << "failed to compile fragment shader";
        GLint maxLength = 0;
        glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);
        qDebug() << errorLog.data();
    }

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
        checkError("glUniformMatrix4fv(id [" + varname + "], <ptr>)"); // suppress error - there is a quirk in openGL to always return 0 for this
    } else qDebug() << "setShaderVariableMatrix - could not find " << varname;
}
void MabiMeGLWidget::setShaderArrayMatrix(GLhandleARB shader, QString varname) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
//        GLfloat *matrix = (GLfloat*)malloc(64 * matrices.count());
//        for (int n = 0; n < matrices.count(); n++) {
//            memcpy(&matrix + (n * 64), matrices[n].constData(), 64);
//        }
        glUniformMatrix4fv(id, 4, GL_FALSE, &boneMatriceBuffer[0]);
//        free(matrix);
        checkError("glUniformMatrix4fv(id [" + varname + "], <ptr>)", true); // suppress error - there is a quirk in openGL to always return 0 for this
    }
}
void MabiMeGLWidget::setShaderVariableFloat(GLhandleARB shader, QString varname, QVector3D vec3) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform3f(id, vec3.x(), vec3.y(), vec3.z());
        checkError("glUniform3f(id [" + varname + "], )");
    }
}
void MabiMeGLWidget::setShaderVariableFloat(GLhandleARB shader, QString varname, QVector4D vec4) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform4f(id, vec4.x(), vec4.y(), vec4.z(), vec4.w());
        checkError("glUniform4f(id [" + varname + "], )");
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
void MabiMeGLWidget::setShaderArrayInt(GLhandleARB shader, QString varname, int *data, int arraySize) {
    GLint id = glGetUniformLocation(shader, varname.toLatin1());
    checkError("glGetUniformLocation[" + varname + "]");
    if (id != -1) {
        glUniform1iv (id, arraySize, &data[0]);
        checkError("glUniform1iv(id [" + varname + "], <size>" + QString::number(arraySize) + ", <ptr>)", false);
    }
}
int MabiMeGLWidget::getModelCount() {
    return models.count();
}

Model* MabiMeGLWidget::getModel(QString modelName) {
    for (int i = 0; i < models.count(); i++) {
        Model *m = models[i];
        if (m->getName() == modelName) return m;
    }
    return nullptr;
}

void MabiMeGLWidget::deleteModel(QString modelName) {
    Model *m = getModel(modelName);
    int index = models.indexOf(m);
    if (m != nullptr) delete m;
    models.removeAt(index);
}

void MabiMeGLWidget::setCameraZ(float z) {
    camera.zoom = z;
    this->repaint();
}

CameraInfo MabiMeGLWidget::getCameraInfo() {
    return camera;
}

void MabiMeGLWidget::debugLog(QOpenGLDebugMessage msg) {
    if (msg.type() == QOpenGLDebugMessage::OtherType) return;
    qDebug() << "GLERROR" << msg;
}

MabiMeGLWidget::~MabiMeGLWidget() {
    if (logger != nullptr) {
        logger->stopLogging();
        delete logger;
    }
}
