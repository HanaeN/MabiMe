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

#ifndef PMGREADER_H
#define PMGREADER_H

#include <QByteArray>
#include <QtOpenGL>
#include <GL/gl.h>

namespace PMG {
    enum MeshType {
        none, Hair, Hat, Robe, Chest, Shoes, Gloves, Head
    };

    #pragma pack(1)
    struct FileHeader {
        char magic[4];
        short version;
        int headSize;
        char meshName[128];
        int meshType;
    };
    struct Skin {
        int vertexID = 0;
        int a = 0;
        float boneWeight = 0.5;
        int b = 1;
    };
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        unsigned char r,g,b,a;
        float u,v;
        Skin skin;
        int globalID = 0;
    };
    struct ShaderVertex {
        GLfloat xyz[3], nxyz[3];
        GLfloat rgba[4];
        GLfloat uv[2];
        GLfloat boneWeight[2];
        GLfloat boneID;
    };
    struct Mesh {
        int size;
        QString boneName, boneName2, statName, normalName, meshName, textureName, colourMap;
        QStringList boneNames;
        char parts2Length;
        int count;
        QMatrix4x4 minorMatrix, majorMatrix;
        int partNo               = 0;
        int faceVertexCount      = 0;
        int faceCount            = 0;
        int stripFaceVertexCount = 0;
        int stripFaceCount       = 0;
        int vertexCount          = 0;
        int skinCount            = 0;
        bool showMesh            = true;
        GLuint *vertexList;
        QList<short> stripVertexList = QList<short>();
        QList<Vertex*> vertices = QList<Vertex*>();
        QList<int> globalVertices = QList<int>();
        ShaderVertex *shaderVertices;
        int cleanVertexCount;
        QList<Skin*> skins = QList<Skin*>();
    };
}

class PMGReader
{
private:
    PMG::FileHeader header;
    bool hasLoaded = false;
    int appendVertex(PMG::Vertex *v);
public:
    void freePMG();
    QStringList textures;
    QList<PMG::Vertex*> vertices = QList<PMG::Vertex*>();
    QList<PMG::Mesh*> meshes;
    PMG::MeshType meshType = PMG::MeshType::none;
    PMGReader();
    ~PMGReader();
    bool loadPMG(QByteArray stream, PMG::MeshType meshType);
};

#endif // PMGREADER_H
