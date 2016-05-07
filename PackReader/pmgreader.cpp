#include "pmgreader.h"

#include <QDebug>

PMGReader::PMGReader()
{
    meshes = QList<PMG::Mesh*>();
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
                    PMG::Mesh *mesh = new PMG::Mesh();
                    if (pmVersion == 1793) {
                        mesh->size = *(int*)&data[pos];
                        pos += 4;
                        memcpy(&mesh->boneName, &data[pos], 32);
                        pos += 32;
                        memcpy(&mesh->meshName, &data[pos], 128);
                        pos += 256;
                        memcpy(mesh->minorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        memcpy(mesh->majorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        mesh->partNo = *(int*)&data[pos];
                        pos += 12;
                        memcpy(mesh->textureName, &data[pos], 32);
                        pos += 72;
                        mesh->faceVertexCount = *(int*)&data[pos];
                        pos += 4;
                        mesh->faceCount = *(int*)&data[pos];
                        pos += 4;
                        mesh->stripFaceVertexCount = *(int*)&data[pos];
                        pos += 4;
                        mesh->stripFaceCount = *(int*)&data[pos];
                        pos += 4;
                        mesh->vertexCount = *(int*)&data[pos];
                        pos += 4;
                        mesh->skinCount = *(int*)&data[pos];
                        pos += 124;
                        for (int i = 0; i < mesh->faceVertexCount; i++) {
                            short *v = (short*)malloc(2);
                            memcpy(v, &data[pos], 2);
                            pos += 2;
                            mesh->vertexList.append(v);
                        }
                        for (int i = 0; i < mesh->stripFaceVertexCount; i++) {
                            short *v = (short*)malloc(2);
                            memcpy(v, &data[pos], 2);
                            pos += 2;
                            mesh->stripVertexList.append(v);
                        }
//                        mesh->cleanVertices = (GLfloat*)malloc(4 * mesh->vertexCount * 3);
//                        mesh->cleanColours = (GLfloat*)malloc(4 * mesh->vertexCount * 4);
//                        mesh->cleanVertexCount = mesh->vertexCount;
                        for (int i = 0; i < mesh->vertexCount; i++) {
                            PMG::Vertex *v = new PMG::Vertex();
                            memcpy(v, &data[pos], sizeof(PMG::Vertex));
                            pos += sizeof(PMG::Vertex);
                            mesh->vertices.append(v);
//                            mesh->cleanVertices[i * 3] = v->x;
//                            mesh->cleanVertices[i * 3 + 1] = v->y;
//                            mesh->cleanVertices[i * 3 + 2] = v->z;
//                            mesh->cleanColours[i * 3] = 1;
//                            mesh->cleanColours[i * 3 + 1] = 1;
//                            mesh->cleanColours[i * 3 + 2] = 1;
//                            mesh->cleanColours[i * 3 + 3] = 1;
                        }

                        mesh->cleanVertices = (GLfloat*)malloc(4 * mesh->faceVertexCount * 3);
                        mesh->cleanColours = (GLfloat*)malloc(4 * mesh->faceVertexCount * 4);
                        mesh->cleanVertexCount = mesh->faceVertexCount;
                        for (int i = 0; i < mesh->faceVertexCount; i++) {
                            short off = *mesh->vertexList[i];
                            PMG::Vertex *v = mesh->vertices[off];
                            mesh->cleanVertices[i * 3] = v->x;
                            mesh->cleanVertices[i * 3 + 1] = v->y;
                            mesh->cleanVertices[i * 3 + 2] = v->z;
                            mesh->cleanColours[i * 4] = ((GLfloat)v->r / 255.0);
                            mesh->cleanColours[i * 4 + 1] = ((GLfloat)v->g / 255.0);
                            mesh->cleanColours[i * 4 + 2] = ((GLfloat)v->b / 255.0);
                            mesh->cleanColours[i * 4 + 3] = ((GLfloat)v->a / 255.0);
                        }

                        for (int i = 0; i < mesh->skinCount; i++) {
                            PMG::Skin *s = new PMG::Skin();
                            memcpy(s, &data[pos], sizeof(PMG::Skin));
                            pos += sizeof(PMG::Skin);
                            mesh->skins.append(s);
                        }
                    }
                    if (pmVersion == 2) {
// DO LATER.
                    }
                    meshes.append(mesh);
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
