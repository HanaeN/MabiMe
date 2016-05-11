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
//    o->bones = obj->f.bones;
/*
    for (int i = 0; i < obj->r.textures.count(); i++) {
        PMGTexture *t = new PMGTexture;
        t->name = obj->r.textures[i];
        QPixmap pix;
        pix.loadFromData(p->ExtractFile(p->FindTexture(t->name)));
        t->img = pix.toImage();
        o->textures.append(t);
    }
    o->meshes = obj->r.meshes;
    ui->glSurface->addPMG(o);
    this->setWindowTitle("MM:<" + PMG + "> [" + QString::number(objects.count()) + " total object" + (objects.count() != 1 ? "s]" : "]"));
    ui->glSurface->repaint();
    */
}

