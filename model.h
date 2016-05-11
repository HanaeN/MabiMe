#ifndef MODEL_H
#define MODEL_H

#include <QImage>

#include <QtOpenGL>

#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"
#include "PackReader/frmreader.h"
#include "bone.h"

struct PMGTexture {
    QString name;
    QImage img;
    GLuint texture = -1;
};

class Model
{
public:
    Model(MabiPackReader *packReader, QString PMGpath, QString FRMpath = "");
    Model();
    QList<PMG::Mesh*> meshes;
    QList<PMGTexture> textures;
private:
    FRMReader frmReader;
    PMGReader pmgReader;
    QString name = "<null>";
    Bone *boneTree = nullptr;
};

#endif // MODEL_H
