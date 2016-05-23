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

#ifndef MODEL_H
#define MODEL_H

#include <QImage>

#include <QtOpenGL>
#include <QColor>

#include "PackReader/packmanager.h"
#include "PackReader/pmgreader.h"
#include "PackReader/frmreader.h"
#include "bone.h"

struct PMGTexture {
    QString name;
    QImage img;
    GLuint texture = -1;
    bool loaded = false;
};

struct ModelColours {
    QColor hair, eye, skin;
};

struct PMGColours {
    QColor shoe1, shoe2, shoe3;
    QColor cloth1, cloth2, cloth3;
};

struct PMGModel {
    QList<PMG::Mesh*> meshes;
    QString name;
    PMGReader pmgReader;
    PMGColours colours;
};

class Model
{
public:
    Model(PackManager *packManager, QString PMGpath, QString FRMpath = "");
    Model();
    ~Model();
    QList<PMGTexture> textures;
    QList<PMGModel*> models;
    Bone* findBone(QString boneName);
    void addPMG(QString path);
    void setName(QString name);
    QList<PMG::Vertex*> vertices;
    bool hasBoneTree();
    QString getName();
    bool isVisible = true;
    Bone *getRootBone();
    ModelColours colours;
private:
    PackManager *packManager;
    FRMReader frmReader;
    QString name = "<null>";
    Bone *boneTree = nullptr;
};

#endif // MODEL_H
