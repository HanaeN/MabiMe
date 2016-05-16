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
