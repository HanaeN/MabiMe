#include "bone.h"
#include <QMatrix4x4>
#include <QMatrix3x3>

Bone::Bone(QList<FRM::Bone*> *boneData, int boneIndex)
{
    FRM::Bone *bone = boneData->at(boneIndex);
    id = bone->boneID;
    name = bone->name;
    QMatrix4x4 matrix(bone->link);
    worldMatrix = matrix;
    translation = matrix.column(3).toVector3D();
    scale = QVector3D(matrix.row(0).toVector3D().length(),
                      matrix.row(1).toVector3D().length(),
                      matrix.row(2).toVector3D().length());
    rotation = QQuaternion::fromRotationMatrix(matrix.toGenericMatrix<3,3>());
    // search and add children recursively to build a bone tree
    for (int i = 0; i < boneData->count(); i++) {
        FRM::Bone *b = boneData->at(i);
        if (b->parentID == id) {
            Bone *childBone = new Bone(boneData, i);
            children.append(childBone);
        }
    }
}

const QString Bone::getName() {
    return name;
}

void Bone::updateBone() {
    QMatrix4x4 matrix;
    matrix.translate(translation);
    matrix.rotate(rotation);
    matrix.scale(scale);
    if (boneParent != nullptr) worldMatrix = boneParent->worldMatrix * matrix;
        else worldMatrix = matrix;
    for (int i = 0; i < children.count(); i++) {
        children[i]->updateBone();
    }
}
