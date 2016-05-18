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

#ifndef BONE_H
#define BONE_H


#include "PackReader/frmreader.h"
#include <QtOpenGL>
#include <QQuaternion>
#include <QMatrix4x4>

class Bone
{
public:
    Bone(QList<FRM::Bone*> *boneData, int boneIndex, Bone *parent = nullptr);
    const QString getName();
    const QMatrix4x4 getMatrix();
    void setX(GLfloat x);
    Bone *findBone(QString boneName);
    Bone *getParent();
    QList<Bone*> getChildren();
private:
    QVector3D translation;
    QVector3D scale;
    QQuaternion rotation;
    QMatrix4x4 worldMatrix;
    QString name;
    int id;
    Bone *parentBone;
    QList<Bone*> children;
    void updateBone();
};

#endif // BONE_H
