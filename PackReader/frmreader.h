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

#ifndef FRMREADER_H
#define FRMREADER_H

#include <QByteArray>
#include <QMatrix4x4>
#include <QtOpenGL>

namespace FRM {
    #pragma pack(1)
    struct FileHeader {
        char magic[4];
        short version, bonesCount;
    };
    #pragma pack(1)
    struct Bone {
        float globalToLocal[16], localToGlobal[16], link[16];
        char dummy;
        char name[31];
        char boneID, parentID;
        short empty;
        QQuaternion quat1, quat2;
    };
    struct CleanBone {

    };
}

class FRMReader
{
private:
    FRM::FileHeader header;
public:
    static void flipMatrix(float *matrix);
    QList<FRM::Bone*> bones;
    FRMReader();
    bool loadFRM(QByteArray stream);
};

#endif // FRMREADER_H
