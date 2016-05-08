#ifndef FRMREADER_H
#define FRMREADER_H

#include <QByteArray>
#include <QMatrix4x4>
#include <QtOpenGL>

namespace FRM {
    #pragma pack(1)
    struct FileHeader {
        char magic[4];
        short version, bonesCount;
    };
    #pragma pack(1)
    struct Bone {
        float globalToLocal[16], localToGlobal[16], link[16];
        char dummy;
        char name[31];
        char boneID, parentID;
        short empty;
        QQuaternion quat1, quat2;
    };
    struct CleanBone {

    };
}

class FRMReader
{
private:
    FRM::FileHeader header;
public:
    QList<FRM::Bone*> bones;
    FRMReader();
    bool LoadFRM(QByteArray stream);
};

#endif // FRMREADER_H
