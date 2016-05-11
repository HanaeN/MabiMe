#include "model.h"

Model::Model()
{
}

Model::Model(MabiPackReader *packReader, QString PMGpath, QString FRMpath)
{
    this->packReader = packReader;
    frmReader.loadFRM(packReader->ExtractFile(FRMpath + ".frm"));
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
    m->pmgReader.loadPMG(packReader->ExtractFile(path + ".pmg"));
    m->name = path;
    // load textures ready for rendering
    for (int i = 0; i < m->pmgReader.textures.count(); i++) {
        PMGTexture t;
        t.name = m->pmgReader.textures[i];
        QString textureName = packReader->FindTexture(t.name);
        QPixmap pix;
        QByteArray bytes = packReader->ExtractFile(textureName);
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
