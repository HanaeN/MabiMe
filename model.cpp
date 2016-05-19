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
    PMGModel *m = new PMGModel();
    m->pmgReader.loadPMG(packManager->extractFile(path + ".pmg"));
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
    if (handleLinks) {
        QString mName;
        int mID;
        int links = 0;
        if (hasBoneTree()) { // TO DO - multiply vertices by bone and minor matrix to work out neighbours, then the detection is done
            for (int i = 0; i < m->meshes.count(); i++) {
                Bone *b = boneTree->findBone(m->meshes[i]->boneName);
                for (int n = 0; n < m->meshes[i]->vertexCount; n++) {
                    PMG::Vertex *v = m->meshes[i]->vertices[n];
                    QVector3D vertexPos = QMatrix4x4(boneTree->findBone(m->meshes[i]->boneName)->getMatrix() * m->meshes[i]->minorMatrix).map(QVector3D(v->x, v->y, v->z));
                    m->meshes[i]->shaderVertices[n].xyz[0] = vertexPos.x();
                    m->meshes[i]->shaderVertices[n].xyz[1] = vertexPos.y();
                    m->meshes[i]->shaderVertices[n].xyz[2] = vertexPos.z();
                    if (v->skin.boneWeight < 2) {
                        float smallestDistance = 1;
                        PMG::Vertex *closestVertex = nullptr;
                        QString otherBoneName = "";
                        for (int ii = 0; ii < m->meshes.count(); ii++) {
                            bool allowed = false;
                            if (b->getParent() != nullptr) {
                                if (m->meshes[ii]->boneName == b->getParent()->getName()) allowed = true;
                            }
                            foreach (Bone *childBone, b->getChildren()) {
                                if (m->meshes[ii]->boneName == childBone->getName()) allowed = true;
                            }
                            //if (ii == i) allowed = true;
                            if (allowed) {
                                foreach (PMG::Vertex *v2, m->meshes[ii]->vertices) {
                                    if (v2 != v) {// && fabs((v->skin.boneWeight + v2->skin.boneWeight) - 1) < 0.001) {
                                        QVector3D vertexPos2 = QMatrix4x4(boneTree->findBone(m->meshes[ii]->boneName)->getMatrix() * m->meshes[ii]->minorMatrix).map(QVector3D(v2->x, v2->y, v2->z));
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
//                            m->meshes[i]->shaderVertices[n].boneWeight[0] = 0.5;
//                              m->meshes[i]->shaderVertices[n].boneWeight[1] = 0.5;
//                            qDebug() << m->meshes[i]->meshName << v->skin.boneWeight << otherBoneName << closestVertex->skin.b << closestVertex->skin.boneWeight;
                        } else {
//                            qDebug() << "failed to find link" << n << v->globalID << m->meshes[i]->boneName;
                        }
                    }
                }
            }
        }
        qDebug() << links << "links";
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
