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
                pos = header.headSize;
                for (int i = 0; i < meshCount; i++) {
                    char pmMagic[4];
                    memcpy(&pmMagic, &data[pos], 4);
                    char pmVersion[4];
                    memcpy(&pmVersion, &data[pos + 4], 4);
                    pos += 6;
                    if (strcmp(pmMagic, "pm!\x00") != 0) {
                        qDebug() << "pm type not supported";
                        return false;
                    }
                    if ((pmVersion[0] != 1 && pmVersion[1] != 7) && (pmVersion[0] != 2 && pmVersion[1] != 0)) {
                        qDebug() << "pm version not supported:" << pmVersion;
                        return false;
                    }
                    // read mesh
                    PMG::Mesh *mesh = new PMG::Mesh();
                    if (pmVersion[0] == 1 && pmVersion[1] == 7) {
                        mesh->size = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName = QString::fromLatin1(&data[pos], 32);
                        pos += 32;
                        mesh->meshName = QString::fromLatin1(&data[pos], 128);
                        pos += 256;
                        memcpy(mesh->minorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        memcpy(mesh->majorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        mesh->partNo = *(int*)&data[pos];
                        pos += 12;
                        mesh->textureName = QString::fromLatin1(&data[pos], 32);
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
                    }
                    if (pmVersion[0] == 2 && pmVersion[1] == 0) {
                        mesh->size = *(int*)&data[pos];
                        pos += 4;
                        memcpy(mesh->minorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        memcpy(mesh->majorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        mesh->partNo = *(int*)&data[pos];
                        pos += 12;
                        mesh->count = *(int*)&data[pos];
                        pos += 40;
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
                        pos += 60;
                        int length = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->meshName = QString::fromLatin1(&data[pos], length);;
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName2 = QString::fromLatin1(&data[pos], length);;
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName3 = QString::fromLatin1(&data[pos], length);;
                        pos += length;
                        pos += 4;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->colourMap = QString::fromLatin1(&data[pos], length);;
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->textureName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        pos += 64;
                    }
                    for (int n = 0; n < mesh->faceVertexCount; n++) {
                        short *v = (short*)malloc(2);
                        memcpy(v, &data[pos], 2);
                        pos += 2;
                        mesh->vertexList.append(v);
                    }
                    for (int n = 0; n < mesh->stripFaceVertexCount; n++) {
                        short *v = (short*)malloc(2);
                        memcpy(v, &data[pos], 2);
                        pos += 2;
                        mesh->stripVertexList.append(v);
                    }
                    for (int n = 0; n < mesh->vertexCount; n++) {
                        PMG::Vertex *v = new PMG::Vertex();
                        memcpy(v, &data[pos], sizeof(PMG::Vertex));
                        pos += sizeof(PMG::Vertex);
                        mesh->vertices.append(v);
                    }
                    mesh->cleanVertices = (GLfloat*)malloc(4 * (mesh->faceVertexCount + mesh->stripFaceVertexCount) * 3);
                    mesh->cleanColours = (GLfloat*)malloc(4 * (mesh->faceVertexCount + mesh->stripFaceVertexCount) * 4);
                    mesh->cleanNormals = (GLfloat*)malloc(4 * (mesh->faceVertexCount + mesh->stripFaceVertexCount) * 3);
                    mesh->cleanVertexCount = mesh->faceVertexCount + mesh->stripFaceVertexCount;
                    for (int n = 0; n < mesh->faceVertexCount; n++) {
                        short off = *mesh->vertexList[n];
                        PMG::Vertex *v = mesh->vertices[off];
                        mesh->cleanVertices[n * 3] = v->x;
                        mesh->cleanVertices[n * 3 + 1] = v->y;
                        mesh->cleanVertices[n * 3 + 2] = v->z;
                        mesh->cleanColours[n * 4] = ((GLfloat)v->r / 255.0);
                        mesh->cleanColours[n * 4 + 1] = ((GLfloat)v->g / 255.0);
                        mesh->cleanColours[n * 4 + 2] = ((GLfloat)v->b / 255.0);
                        mesh->cleanColours[n * 4 + 3] = ((GLfloat)v->a / 255.0);
                        mesh->cleanNormals[n * 3] = v->nx;
                        mesh->cleanNormals[n * 3 + 1] = v->ny;
                        mesh->cleanNormals[n * 3 + 2] = v->nz;
                    }
                    for (int n = 0; n < mesh->stripFaceVertexCount; n++) {
                        short off = *mesh->stripVertexList[n];
                        PMG::Vertex *v = mesh->vertices[off];
                        mesh->cleanVertices[(n + mesh->faceVertexCount) * 3] = v->x;
                        mesh->cleanVertices[(n + mesh->faceVertexCount) * 3 + 1] = v->y;
                        mesh->cleanVertices[(n + mesh->faceVertexCount) * 3 + 2] = v->z;
                        mesh->cleanColours[(n + mesh->faceVertexCount) * 4] = ((GLfloat)v->r / 255.0);
                        mesh->cleanColours[(n + mesh->faceVertexCount) * 4 + 1] = ((GLfloat)v->g / 255.0);
                        mesh->cleanColours[(n + mesh->faceVertexCount) * 4 + 2] = ((GLfloat)v->b / 255.0);
                        mesh->cleanColours[(n + mesh->faceVertexCount) * 4 + 3] = ((GLfloat)v->a / 255.0);
                        mesh->cleanNormals[(n + mesh->faceVertexCount) * 3] = v->x;
                        mesh->cleanNormals[(n + mesh->faceVertexCount) * 3 + 1] = v->y;
                        mesh->cleanNormals[(n + mesh->faceVertexCount) * 3 + 2] = v->z;
                    }

                    for (int n = 0; n < mesh->skinCount; n++) {
                        PMG::Skin *s = new PMG::Skin();
                        memcpy(s, &data[pos], sizeof(PMG::Skin));
                        pos += sizeof(PMG::Skin);
                        mesh->skins.append(s);
                    }
                    meshes.append(mesh);
                }
                return true;
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
