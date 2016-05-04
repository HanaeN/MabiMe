#include "pmgreader.h"

#include <QDebug>

PMGReader::PMGReader()
{
    meshes = QList<PMG::Mesh>();
}

bool PMGReader::LoadPMG(QByteArray stream) {
    char *data = stream.data();
    try {
        qDebug() << stream.length();
        if (stream.length() > sizeof(PMG::FileHeader)) {
            header = *(PMG::FileHeader*)&data[0];
            uint32_t pos = sizeof(PMG::FileHeader) + 64;
            if (strcmp(header.magic, "pmg\x00") == 0 && header.version == 258) { // check if valid
                int meshCount = *(int*)&data[pos];
                pos += 208 * meshCount;
                qDebug() << meshCount << header.meshType;
                if (header.meshType == 1) {
                } else if (header.meshType == 5) {
                    pos += 272 * 4;
                } else {
                    qDebug() << "unsupported PMG mesh type";
                    return false;
                }
                qDebug() << meshCount;
                for (int i = 0; i < meshCount; i++) {
                    char pmMagic[4];
                    memcpy(&pmMagic, &data[pos], 4);
                    short pmVersion = *(short*)&data[pos + 4];
                    pos += 6;
                    qDebug() << pmMagic << pmVersion;
                    if (strcmp(pmMagic, "pm!\x00") != 0) {
                        qDebug() << "pm type not supported";
                        return false;
                    }
                    if (pmVersion != 1793 && pmVersion != 2) {
                        qDebug() << "pm version not supported:" << pmVersion;
                        return false;
                    }
                    // read mesh
                    PMG::Mesh mesh;
                    if (pmVersion == 1793) {
                        mesh.size = *(int*)&data[pos];
                        pos += 4;
                        memcpy(&mesh.boneName, &data[pos], 32);
                        pos += 32;
                        memcpy(&mesh.meshName, &data[pos], 128);
                        qDebug() << mesh.boneName << mesh.meshName;
                        // CONTINUE HERE // LOADPM17
                    }
                    if (pmVersion == 2) {

                    }
                    return true;
                }
            } else {
                qDebug() << "unsupported PMG format";
                return false;
            }
        }
    } catch (...) {
        qDebug() << "Failed to load PMG header";
        return false;
    }
    return true;
}
