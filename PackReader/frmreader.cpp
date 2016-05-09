#include "frmreader.h"

#include <QDebug>

FRMReader::FRMReader()
{
    bones = QList<FRM::Bone*>();
}

bool FRMReader::LoadFRM(QByteArray stream) {
    char *data = stream.data();
    try {
        qDebug() << stream.length();
        if ((unsigned int)stream.length() > sizeof(FRM::FileHeader)) {
            header = *(FRM::FileHeader*)&data[0];
            uint32_t pos = sizeof(FRM::FileHeader);
            if (strcmp(header.magic, "pf!\x00") == 0 && header.version == 1) { // check if valid
                qDebug() << header.bonesCount << "bones";
                for (int n = 0; n < header.bonesCount; n++) {
                    FRM::Bone *b = new FRM::Bone();
                    memcpy(b, &data[pos], sizeof(FRM::Bone));
                    qDebug() << QString::number(b->boneID) << b->name << QString::number(b->parentID);
                    float d[16];
                    memcpy(d, b->localToGlobal, 64);
                    QMatrix4x4 m(d[0],  d[1],  d[2],  d[3],
                                 d[4],  d[5],  d[6],  d[7],
                                 d[8],  d[9],  d[10], d[11],
                                 d[12], d[13], d[14], d[15]);
                    memcpy(b->localToGlobal, m.data(), 64);
                    memcpy(d, b->globalToLocal, 64);
                    m = QMatrix4x4(d[0],  d[1],  d[2],  d[3],
                                 d[4],  d[5],  d[6],  d[7],
                                 d[8],  d[9],  d[10], d[11],
                                 d[12], d[13], d[14], d[15]);
                    memcpy(b->globalToLocal, m.data(), 64);
                    memcpy(d, b->link, 64);
                    m = QMatrix4x4(d[0],  d[1],  d[2],  d[3],
                                 d[4],  d[5],  d[6],  d[7],
                                 d[8],  d[9],  d[10], d[11],
                                 d[12], d[13], d[14], d[15]);
                    memcpy(b->link, m.data(), 64);
//                    m.toTransform()
                    pos += sizeof(FRM::Bone);
                    bones.append(b);
                }
            } else {
                qDebug() << "not a valid FRM file";
                return false;
            }
        } else {
            qDebug() << "invalid FRM size!";
            return false;
        }
    } catch (...) {
        qDebug() << "there was an error reading the FRM";
        return false;
    }

    return true;
}
