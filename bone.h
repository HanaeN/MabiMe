#ifndef BONE_H
#define BONE_H


#include "PackReader/frmreader.h"
#include <QtOpenGL>
#include <QQuaternion>
#include <QMatrix4x4>

class Bone
{
public:
    Bone(QList<FRM::Bone*> *boneData, int boneIndex);
    const QString getName();
    const QMatrix4x4 getMatrix();
    Bone *findBone(QString boneName);
private:
    QVector3D translation;
    QVector3D scale;
    QQuaternion rotation;
    QMatrix4x4 worldMatrix;
    QString name;
    int id;
    Bone *boneParent;
    QList<Bone*> children;
    void updateBone();
};

#endif // BONE_H
