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

#include "frmreader.h"

#include <QDebug>

FRMReader::FRMReader()
{
    bones = QList<FRM::Bone*>();
}
void FRMReader::flipMatrix(float *matrix) {
    float d[16];
    memcpy(d, matrix, 64);
    QMatrix4x4 m(d[0],  d[1],  d[2],  d[3],
                 d[4],  d[5],  d[6],  d[7],
                 d[8],  d[9],  d[10], d[11],
                 d[12], d[13], d[14], d[15]);
    memcpy(matrix, m.data(), 64);
}

bool FRMReader::loadFRM(QByteArray stream) {
    char *data = stream.data();
    try {
        if ((unsigned int)stream.length() > sizeof(FRM::FileHeader)) {
            header = *(FRM::FileHeader*)&data[0];
            uint32_t pos = sizeof(FRM::FileHeader);
            if (strcmp(header.magic, "pf!\x00") == 0 && header.version == 1) { // check if valid
                qDebug() << header.bonesCount << "bones";
                for (int n = 0; n < header.bonesCount; n++) {
                    FRM::Bone *b = new FRM::Bone();
                    memcpy(b, &data[pos], sizeof(FRM::Bone));
                    flipMatrix(b->localToGlobal);
                    flipMatrix(b->globalToLocal);
                    flipMatrix(b->link);
                    pos += sizeof(FRM::Bone);
                    bones.append(b);
                }
            } else {
                qDebug() << "not a valid FRM file";
                return false;
            }
        } else {
            qDebug() << "invalid FRM size!";
            return false;
        }
    } catch (...) {
        qDebug() << "there was an error reading the FRM";
        return false;
    }

    return true;
}
