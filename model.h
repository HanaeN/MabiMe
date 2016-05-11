#ifndef MODEL_H
#define MODEL_H

#include "PackReader/mabipackreader.h"
#include "PackReader/pmgreader.h"
#include "PackReader/frmreader.h"
#include "bone.h"

class Model
{
public:
    Model(MabiPackReader *packReader, QString PMGpath, QString FRMpath = "");
    Model();
private:
    FRMReader frmReader;
    PMGReader pmgReader;
    QString name = "<null>";
    Bone *boneTree = nullptr;
};

#endif // MODEL_H
