#ifndef PMGREADER_H
#define PMGREADER_H

#include <QByteArray>
#include <QtOpenGL>

namespace PMG {
    #pragma pack(1)
    struct FileHeader {
        char magic[4];
        short version;
        int headSize;
        char meshName[128];
        int meshType;
    };
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float r,g,b,a;
        float u,v;
    };
    struct Skin {
        int n = 0;
        int a = 0;
        float weight = 1;
        int b = 1;
    };
    struct Mesh {
        int size;
        char boneName[32];
        char meshName[128];
        char textureName[32];
        QMatrix4x4 minorMatrix, majorMatrix;
        int partNo               = 0;
        int faceVertexCount      = 0;
        int faceCount            = 0;
        int stripFaceVertexCount = 0;
        int stripFaceCount       = 0;
        int vertexCount          = 0;
        int skinCount            = 0;
        QList<short*> vertexList = QList<short*>();
        QList<short*> stripVertexList = QList<short*>();
        QList<Vertex*> vertices = QList<Vertex*>();
        QList<Skin*> skins = QList<Skin*>();
    };
}

class PMGReader
{
private:
    PMG::FileHeader header;
    QList<PMG::Mesh> meshes;
public:
    PMGReader();
    bool LoadPMG(QByteArray stream);
};

#endif // PMGREADER_H
