#include "bone.h"
#include <QMatrix4x4>
#include <QMatrix3x3>

Bone::Bone(QList<FRM::Bone*> *boneData, int boneIndex, Bone* parent)
{
    FRM::Bone *bone = boneData->at(boneIndex);
    id = bone->boneID;
    name = bone->name;
    parentBone = parent;

    // extract the bone data from the FRM raw and convert it into easy to use values
    QMatrix4x4 matrix(bone->link);
    worldMatrix = matrix.transposed();
    translation = matrix.transposed().column(3).toVector3D();
    scale = QVector3D(matrix.transposed().row(0).toVector3D().length(),
                      matrix.transposed().row(1).toVector3D().length(),
                      matrix.transposed().row(2).toVector3D().length());
    rotation = QQuaternion::fromRotationMatrix(matrix.transposed().toGenericMatrix<3,3>());
    // search and add children recursively to build a bone tree
    for (int i = 0; i < boneData->count(); i++) {
        FRM::Bone *b = boneData->at(i);
        if (b->parentID == id) {
            Bone *childBone = new Bone(boneData, i, this);
            children.append(childBone);
        }
    }
}

const QString Bone::getName() {
    return name;
}

void Bone::updateBone() {
    // create a matrix from the stored translation, rotation and scale
    QMatrix4x4 matrix;
    matrix.translate(translation);
    matrix.rotate(rotation);
    matrix.scale(scale);
    // generate new matrix. if a child multiply it from the parent matrix first
    worldMatrix = matrix;
//    if (boneParent != nullptr) worldMatrix = boneParent->worldMatrix * matrix;
//        else worldMatrix = matrix;
    for (int i = 0; i < children.count(); i++) {
        children[i]->updateBone();
    }
}

const QMatrix4x4 Bone::getMatrix() {
    if (parentBone != nullptr) return parentBone->getMatrix() * worldMatrix;
    return worldMatrix;
}

Bone* Bone::findBone(QString boneName) {
    if (boneName == name) return this;
    for (int i = 0; i < children.count(); i++) {
        Bone *b = children[i]->findBone(boneName);
        if (b != nullptr) return b;
    }
    return nullptr;
}

void Bone::setX(GLfloat x) {
    translation.setX(x);
    updateBone();
}
