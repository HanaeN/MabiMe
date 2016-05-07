#include "pmgreader.h"

#include <QDebug>

PMGReader::PMGReader()
{
    meshes = QList<PMG::Mesh*>();
    textures = QStringList();
}

void PMGReader::FreePMG() {
    for (int n = 0; n < meshes.count(); n++) {
        PMG::Mesh *m = meshes[n];
        free(m->cleanColours);
        free(m->cleanNormals);
        free(m->cleanTextureCoords);
        free(m->cleanVertices);
        for (int i = 0; i < m->skins.count(); i++) {
            delete m->skins[i];
        }
        for (int i = 0; i < m->stripVertexList.count(); i++) {
            delete m->stripVertexList[i];
        }
        for (int i = 0; i < m->vertexList.count(); i++) {
            delete m->vertexList[i];
        }
        for (int i = 0; i < m->vertices.count(); i++) {
            delete m->vertices[i];
        }
        delete m;
    }
    meshes.clear();
}

bool PMGReader::LoadPMG(QByteArray stream) {
    FreePMG();
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
                    if (memcmp(&data[pos], "pm!\x00", 4) != 0) {
                        qDebug() << "pm type not supported";
                        return false;
                    }
                    char pmVersion[2];
                    memcpy(&pmVersion, &data[pos + 4], 2);
                    if ((pmVersion[0] != 1 && pmVersion[1] != 7) && (pmVersion[0] != 2 && pmVersion[1] != 0)) {
                        qDebug() << "pm version not supported:" << pmVersion;
                        return false;
                    }
                    pos += 6;
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
                        mesh->meshName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName2 = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->boneName3 = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        pos += 4;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->colourMap = QString::fromLatin1(&data[pos], length);
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
                    mesh->cleanTextureCoords = (GLfloat*)malloc(4 * (mesh->faceVertexCount + mesh->stripFaceVertexCount) * 2);
                    mesh->cleanVertexCount = mesh->faceVertexCount + mesh->stripFaceVertexCount;
                    QVector2D uv;
                    QVector3D xyz;
                    QVector4D rgba;
                    for (int n = 0; n < mesh->faceVertexCount + mesh->stripFaceVertexCount; n++) {
                        short off;
                        if (n < mesh->faceVertexCount) off = *mesh->vertexList[n];
                            else off = *mesh->stripVertexList[n - mesh->faceVertexCount];
                        PMG::Vertex *v = mesh->vertices[off];
                        xyz = QVector3D(v->x, v->y, v->z);
                        memcpy(&mesh->cleanVertices[n * 3], &xyz, 12);
                        rgba = QVector4D((GLfloat)v->r / 255.0, (GLfloat)v->g / 255.0, (GLfloat)v->b / 255.0, (GLfloat)v->a / 255.0);
                        memcpy(&mesh->cleanColours[n * 4], &rgba, 16);
                        xyz = QVector3D(v->nx, v->ny, v->nz);
                        memcpy(&mesh->cleanNormals[n * 3], &xyz, 12);
                        uv = QVector2D(v->u, v->v);
                        memcpy(&mesh->cleanTextureCoords[n * 2], &uv, 8);
                    }
                    for (int n = 0; n < mesh->skinCount; n++) {
                        PMG::Skin *s = new PMG::Skin();
                        memcpy(s, &data[pos], sizeof(PMG::Skin));
                        pos += sizeof(PMG::Skin);
                        mesh->skins.append(s);
                    }

                    QMatrix4x4 m; // swap the matrix for openGL reading
                    int index = 0;
                    for (int yy = 0; yy < 4; yy++) {
                        for (int xx = 0; xx < 4; xx++) {
                            m.data()[index] = mesh->majorMatrix.data()[xx * 4 + yy];
                            index++;
                        }
                    }
                    memcpy(mesh->majorMatrix.data(), m.data(), 64);

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

PMGReader::~PMGReader() {
    this->FreePMG();
}
