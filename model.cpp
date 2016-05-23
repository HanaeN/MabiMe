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

#include "model.h"

Model::Model()
{
}

Model::Model(PackManager *packManager, QString PMGpath, QString FRMpath)
{
    this->packManager = packManager;
    frmReader.loadFRM(packManager->extractFile(FRMpath + ".frm"));
    // loop through and find parent bone
    for (int i = 0; i < frmReader.bones.count(); i++) {
        FRM::Bone *bone = frmReader.bones[i];
        if (bone->parentID == -1) {
            boneTree = new Bone(&frmReader.bones, i);
            break;
        }
    }
    if (boneTree != nullptr) {
    } else qDebug() << "no root bone";
    addPMG(PMGpath);
}

Bone* Model::findBone(QString boneName) {
    return boneTree->findBone(boneName);
}

void Model::addPMG(QString path) {
    // resolve the path to check what category it is in
    path = packManager->resolvePath(path + ".pmg");
    PMGModel *m = new PMGModel();
    PMG::MeshType meshType = PMG::MeshType::none;
    if (path.contains("mantle", Qt::CaseInsensitive)) meshType = PMG::MeshType::Robe;
    if (path.contains("face", Qt::CaseInsensitive)) meshType = PMG::MeshType::Head;
    if (path.contains("hair", Qt::CaseInsensitive)) meshType = PMG::MeshType::Hair;
    if (path.contains("glove", Qt::CaseInsensitive)) meshType = PMG::MeshType::Gloves;
    if (path.contains("shoes", Qt::CaseInsensitive)) meshType = PMG::MeshType::Shoes;
    if (path.contains("wear", Qt::CaseInsensitive)) meshType = PMG::MeshType::Chest;
    if (path.contains("helmet", Qt::CaseInsensitive)) meshType = PMG::MeshType::Hat;
    m->pmgReader.loadPMG(packManager->extractFile(path), meshType);
    m->name = path;

    // load textures ready for rendering
    for (int i = 0; i < m->pmgReader.textures.count(); i++) {
        PMGTexture t;
        t.name = m->pmgReader.textures[i];
        QString textureName = packManager->findTexture(t.name);
        QPixmap pix;
        QByteArray bytes = packManager->extractFile(textureName);
        if (bytes.length() > 0) {
            pix.loadFromData(bytes);
            QImage img = pix.toImage();
            t.img = img;
            textures.append(t);
        }
    }
    m->meshes = m->pmgReader.meshes;
    bool handleLinks = true;
    if (m->name.contains("hair")) handleLinks = false;
    if (handleLinks) {
        QString mName;
        int mID;
        int links = 0;
        if (hasBoneTree()) {
            for (int i = 0; i < m->meshes.count(); i++) {
                // grab the bone linked to this mesh
                Bone *b = boneTree->findBone(m->meshes[i]->boneName);
                // if the main bone could not be found, try the second bone name
                if (b == nullptr) b = boneTree->findBone(m->meshes[i]->boneName2);
                for (int n = 0; n < m->meshes[i]->vertexCount; n++) {
                    PMG::Vertex *v = m->meshes[i]->vertices[n];
                    QVector3D vertexPos = QMatrix4x4(b->getMatrix() * m->meshes[i]->minorMatrix).map(QVector3D(v->x, v->y, v->z));
                    m->meshes[i]->shaderVertices[n].xyz[0] = vertexPos.x();
                    m->meshes[i]->shaderVertices[n].xyz[1] = vertexPos.y();
                    m->meshes[i]->shaderVertices[n].xyz[2] = vertexPos.z();
                    if (v->skin.boneWeight < 1) {
                        float smallestDistance = 2;
                        PMG::Vertex *closestVertex = nullptr;
                        QString otherBoneName = "";
                        for (int ii = 0; ii < m->meshes.count(); ii++) {
                            bool allowed = true;
                            // grab the bone linked to this mesh
                            Bone *b2 = boneTree->findBone(m->meshes[ii]->boneName);
                            // if the main bone could not be found, try the second bone name
                            if (b2 == nullptr) b2 = boneTree->findBone(m->meshes[ii]->boneName2);
/*
                            if (b->getParent() != nullptr) {
                                if (m->meshes[ii]->boneName == b->getParent()->getName()) allowed = true;
                            }
                            foreach (Bone *childBone, b->getChildren()) {
                                if (m->meshes[ii]->boneName == childBone->getName()) allowed = true;
                            }
*/
                            if (ii == i) allowed = false;
                            if (allowed) {
                                foreach (PMG::Vertex *v2, m->meshes[ii]->vertices) {
                                    if (v2 != v) {
                                        QVector3D vertexPos2 = QMatrix4x4(b2->getMatrix() * m->meshes[ii]->minorMatrix).map(QVector3D(v2->x, v2->y, v2->z));
                                        float distance = vertexPos.distanceToPoint(vertexPos2);
                                        if (distance <= smallestDistance) {
                                            smallestDistance = distance;
                                            closestVertex = v2;
                                            otherBoneName = m->meshes[ii]->boneName;
                                        }
                                    }
                                }
                            }
                        }
                        if (closestVertex != nullptr) {
                            links++;
                            mName = otherBoneName;
                            mID = m->meshes[i]->boneNames.indexOf(mName);
                            if (mID == -1) {
                                mID = m->meshes[i]->boneNames.count();
                                m->meshes[i]->boneNames.append(mName);
                            }
                            m->meshes[i]->shaderVertices[n].boneID = mID;
                        } else {
//                            qDebug() << "failed to find link" << n << v->globalID << m->meshes[i]->boneName;
                        }
                    }
                }
            }
        }
        qDebug() << links << "links";
    } else {
        if (hasBoneTree()) {
            for (int i = 0; i < m->meshes.count(); i++) {
                // grab the bone linked to this mesh
                Bone *b = boneTree->findBone(m->meshes[i]->boneName);
                // if the main bone could not be found, try the second bone name
                if (b == nullptr) b = boneTree->findBone(m->meshes[i]->boneName2);
                for (int n = 0; n < m->meshes[i]->vertexCount; n++) {
                    PMG::Vertex *v = m->meshes[i]->vertices[n];
                    QVector3D vertexPos = QMatrix4x4(b->getMatrix() * m->meshes[i]->minorMatrix).map(QVector3D(v->x, v->y, v->z));
                    m->meshes[i]->shaderVertices[n].xyz[0] = vertexPos.x();
                    m->meshes[i]->shaderVertices[n].xyz[1] = vertexPos.y();
                    m->meshes[i]->shaderVertices[n].xyz[2] = vertexPos.z();
                }
            }
        }
    }
    models.append(m);
}

void Model::setName(QString name) {
    this->name = name;
}

QString Model::getName() {
    return name;
}

bool Model::hasBoneTree() {
    return (boneTree == nullptr) ? false : true;
}

Bone* Model::getRootBone() {
    return boneTree;
}

Model::~Model() {
    if (boneTree != nullptr) delete boneTree;
    for (int i = 0; i < textures.count(); i++) {
        glDeleteTextures(1, &textures.at(i).texture);
    }
    textures.clear();
    foreach (PMGModel *m, models) {
        m->pmgReader.freePMG();
        m->meshes.clear();
        delete m;
    }
}
