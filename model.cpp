#include "model.h"

Model::Model()
{
}

Model::Model(MabiPackReader *packReader, QString PMGpath, QString FRMpath)
{
    pmgReader.loadPMG(packReader->ExtractFile(PMGpath + ".pmg"));
    frmReader.loadFRM(packReader->ExtractFile(FRMpath + ".frm"));
    name = PMGpath;
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
    // load textures ready for rendering
    for (int i = 0; i < pmgReader.textures.count(); i++) {
        PMGTexture t;
        t.name = pmgReader.textures[i];
        QString textureName = packReader->FindTexture(t.name);
        QPixmap pix;
        QByteArray bytes = packReader->ExtractFile("material\\char\\highgloss\\item_treasurechest10_m.dds");
        if (bytes.length() > 0) {
            pix.loadFromData(bytes);
            QImage img = pix.toImage();
            t.img = img;
            textures.append(t);
        }
    }
    meshes = pmgReader.meshes;
}

