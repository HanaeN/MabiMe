#include "bone.h"

Bone::Bone(QList<FRM::Bone*> *boneData, int boneIndex)
{
    FRM::Bone *bone = boneData->at(boneIndex);
    id = bone->boneID;
    name = bone->name;
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

