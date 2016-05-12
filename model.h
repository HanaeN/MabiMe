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
    bool loaded = false;
};

struct PMGModel {
    QList<PMG::Mesh*> meshes;
    QString name;
    PMGReader pmgReader;
};

class Model
{
public:
    Model(MabiPackReader *packReader, QString PMGpath, QString FRMpath = "");
    Model();
    QList<PMGTexture> textures;
    QList<PMGModel*> models;
    Bone* findBone(QString boneName);
    void addPMG(QString path);
    void setName(QString name);
    QList<PMG::Vertex*> vertices;
    bool hasBoneTree();
    QString getName();
private:
    MabiPackReader *packReader;
    FRMReader frmReader;
    QString name = "<null>";
    Bone *boneTree = nullptr;
};

#endif // MODEL_H
