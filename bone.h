#ifndef BONE_H
#define BONE_H


#include "PackReader/frmreader.h"

class Bone
{
public:
    Bone(QList<FRM::Bone*> *boneData, int boneIndex);
    const QString getName();
private:
    QString name;
    int id;
    Bone *boneParent;
    QList<Bone*> children;
};

#endif // BONE_H
