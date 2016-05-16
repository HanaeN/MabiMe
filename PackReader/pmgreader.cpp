/*
 MabiMe Character Simulator - by Yai (Sophie N)
 Email: sinoc300@gmail.com
 Copyright (C) 2016

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pmgreader.h"

#include <QDebug>

PMGReader::PMGReader()
{
    meshes = QList<PMG::Mesh*>();
    textures = QStringList();
}

void PMGReader::freePMG() {
    hasLoaded = false;
    for (int n = 0; n < meshes.count(); n++) {
        PMG::Mesh *m = meshes[n];
        free(m->cleanColours);
        free(m->cleanNormals);
        free(m->cleanTextureCoords);
        free(m->cleanVertices);
        free(m->cleanBoneWeights);
        for (int i = 0; i < m->skins.count(); i++) {
            delete m->skins[i];
        }
        for (int i = 0; i < m->vertices.count(); i++) {
            delete m->vertices[i];
        }
        delete m;
    }
    meshes.clear();
    textures.clear();
}

bool PMGReader::loadPMG(QByteArray stream) {
    if (hasLoaded) freePMG();
    char *data = stream.data();
    try {
        if ((unsigned int)stream.length() > sizeof(PMG::FileHeader)) {
            header = *(PMG::FileHeader*)&data[0];
            uint32_t pos = sizeof(PMG::FileHeader) + 64;
            if (strcmp(header.magic, "pmg\x00") == 0 && header.version == 258) { // check if valid
                int meshCount = *(int*)&data[pos];
                pos = header.headSize;
                for (int i = 0; i < meshCount; i++) {
                    if (memcmp(&data[pos], "pm!\x00", 4) != 0) {
                        qDebug() << "pm type not supported";
                        break;
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
                        if ((int)data[pos + 31] == 0) mesh->boneName = QString::fromLatin1(&data[pos]);
                            else mesh->boneName = QString::fromLatin1(&data[pos], 32);
                        pos += 32;
                        if ((int)data[pos + 127] == 0) mesh->meshName = QString::fromLatin1(&data[pos]);
                            else mesh->meshName = QString::fromLatin1(&data[pos], 128);
                        pos += 256;
                        memcpy(mesh->minorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        memcpy(mesh->majorMatrix.data(), &data[pos], 64);
                        pos += 64;
                        mesh->partNo = *(int*)&data[pos];
                        pos += 12;
                        if ((int)data[pos + 31] == 0) mesh->textureName = QString::fromLatin1(&data[pos]);
                            else mesh->textureName = QString::fromLatin1(&data[pos], 32);
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
                        mesh->statName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->normalName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->colourMap = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        length = *(int*)&data[pos];
                        pos += 4;
                        mesh->textureName = QString::fromLatin1(&data[pos], length);
                        pos += length;
                        pos += 64;
                        qDebug() << mesh->meshName << mesh->boneName << mesh->boneName2 << mesh->statName << mesh->normalName << mesh->colourMap << mesh->textureName;
                    }
                    mesh->boneNames = QStringList(mesh->boneName);
                    for (int n = 0; n < mesh->faceVertexCount; n++) {
                        short v = *(short*)&data[pos];
                        mesh->vertexList.append(v);
                        pos += 2;
                    }
                    for (int n = 0; n < mesh->stripFaceVertexCount; n++) {
                        short v = *(short*)&data[pos];
                        mesh->stripVertexList.append(v);
                        pos += 2;
                    }
                    for (int n = 0; n < mesh->vertexCount; n++) {
                        PMG::Vertex *v = new PMG::Vertex();
                        memcpy(v, &data[pos], 36);
                        pos += 36;
                        mesh->globalVertices.append(appendVertex(v));
                        mesh->vertices.append(v);
                    }
                    if (!textures.contains(mesh->textureName)) textures.append(mesh->textureName);
                    mesh->cleanVertices = (GLfloat*)malloc(4 * mesh->faceVertexCount * 3);
                    mesh->cleanColours = (GLfloat*)malloc(4 * mesh->faceVertexCount * 4);
                    mesh->cleanNormals = (GLfloat*)malloc(4 * mesh->faceVertexCount * 3);
                    mesh->cleanTextureCoords = (GLfloat*)malloc(4 * mesh->faceVertexCount * 2);
                    mesh->cleanBoneWeights = (GLfloat*)malloc(4 * mesh->faceVertexCount);
                    mesh->cleanBoneIDs = (GLint*)malloc(4 * mesh->faceVertexCount);
                    mesh->cleanVertexCount = mesh->faceVertexCount;

                    QVector2D uv;
                    QVector3D xyz;
                    QVector4D rgba;

                    for (int n = 0; n < mesh->skinCount; n++) {
                        PMG::Skin *s = new PMG::Skin();
                        memcpy(s, &data[pos], sizeof(PMG::Skin));
                        vertices[mesh->globalVertices[s->vertexID]]->skin = *s;
                        pos += sizeof(PMG::Skin);
                        mesh->skins.append(s);
                    }


                    for (int n = 0; n < mesh->faceVertexCount; n++) {
                        PMG::Vertex *v = mesh->vertices[mesh->vertexList[n]];
                        xyz = QVector3D(v->x, v->y, v->z);
                        memcpy(&mesh->cleanVertices[n * 3], &xyz, 12);
                        rgba = QVector4D((GLfloat)v->r / 255.0, (GLfloat)v->g / 255.0, (GLfloat)v->b / 255.0, (GLfloat)v->a / 255.0);
                        memcpy(&mesh->cleanColours[n * 4], &rgba, 16);
                        xyz = QVector3D(v->nx, v->ny, v->nz);
                        memcpy(&mesh->cleanNormals[n * 3], &xyz, 12);
                        uv = QVector2D(v->u, v->v);
                        memcpy(&mesh->cleanTextureCoords[n * 2], &uv, 8);
                        mesh->cleanBoneWeights[n] = 1.0;//v->skin.boneWeight;
                        mesh->cleanBoneIDs[n] = 0;
                    }
                    float d[16];
                    memcpy(d, mesh->majorMatrix.data(), 64);
                    QMatrix4x4 m(d[0],  d[1],  d[2],  d[3],
                                 d[4],  d[5],  d[6],  d[7],
                                 d[8],  d[9],  d[10], d[11],
                                 d[12], d[13], d[14], d[15]);
                    memcpy(mesh->majorMatrix.data(), m.data(), 64);
                    memcpy(d, mesh->minorMatrix.data(), 64);
                    m = QMatrix4x4(d[0],  d[1],  d[2],  d[3],
                                   d[4],  d[5],  d[6],  d[7],
                                   d[8],  d[9],  d[10], d[11],
                                   d[12], d[13], d[14], d[15]);
                    memcpy(mesh->minorMatrix.data(), m.data(), 64);
                    meshes.append(mesh);
                }
                /*
                QString mName;
                int mID;
                int links = 0;
                float tolerance = 0.000001;
                for (int i = 0; i < meshes.count(); i++) {
                    for (int n = 0; n < meshes[i]->faceVertexCount; n++) {
                        PMG::Vertex *v = vertices[meshes[i]->globalVertices[meshes[i]->vertexList[n]]];
                        for (int ii = 0; ii < meshes.count(); ii++) {
                            if (ii != i) {
                                for (int nn = 0; nn < meshes[ii]->vertexCount; nn++) {
                                    PMG::Vertex *v2 = vertices[meshes[ii]->globalVertices[meshes[ii]->vertexList[nn]]];
                                    if (fabs(v2->x - v->x) < tolerance && fabs(v2->y - v->y) < tolerance && fabs(v2->z - v->z) < tolerance) {
                                        links++;
                                        mName = meshes[ii]->boneName;
                                        if (mName != meshes[i]->boneName) {
                                            mID = meshes[i]->boneNames.indexOf(mName);
                                            if (mID == -1) {
                                                mID = meshes[i]->boneNames.count();
                                                meshes[i]->boneNames.append(mName);
                                            }
                                            meshes[i]->cleanBoneIDs[n] = mID;
                                            qDebug() << "mesh" << meshes[i]->meshName << "boneName" << meshes[i]->boneName << "to" << meshes[ii]->boneName << mID;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                qDebug() << links << "links";
                */
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
    hasLoaded = true;
    return true;
}

PMGReader::~PMGReader() {
    this->freePMG();
}

int PMGReader::appendVertex(PMG::Vertex *v) {
    float tolerance = 0.000001;
    for (int i = 0; i < vertices.count(); i++) {
        PMG::Vertex *v2 = vertices[i];
        if (fabs(v2->x - v->x) < tolerance && fabs(v2->y - v->y) < tolerance && fabs(v2->z - v->z) < tolerance) return i;
    }
    vertices.append(v);
    return vertices.count() - 1;
}
