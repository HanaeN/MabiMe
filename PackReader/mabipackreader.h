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

#ifndef MABIPACKREADER_H
#define MABIPACKREADER_H

#include <QString>
#include <QDataStream>
#include <Qfile>
#include <QList>
#include <random>

namespace MabiPack {
    static char ValidHeader[8] = {0x50, 0x41, 0x43, 0x4B, 0x02, 0x01, 0x00, 0x00};
    #pragma pack(1)
    struct FileHeader {
        char Signature[8];                                  // this tells the client if it is a valid pack file
        unsigned int Revision = 0;                          // file version
        int EntryCount;                                     // number of entries in the file
        long long Ft1;                                      // ? some kind of timestamp, doesn't seem to be unix
        long long Ft2;                                      // ? some kind of timestamp, doesn't seem to be unix
        char path[480];                                     // 480 bytes string
    };
    #pragma pack(1)
    struct PackageHeader {
        int EntryCount;                                     // number of entries in the package
        int InfoHeaderSize;                                 // size of the info header in bytes
        int BlankSize;                                      // a defined blank space size, for appending
        unsigned int DataSectionSize;                       // size of the content
        char Zero[16];                                      // ?
    };
    #pragma pack(1)
    struct PackageItemInfo {
        int Seed;
        int Zero;
        unsigned int Offset;
        int CompressedSize;
        int DecompressedSize;
        int IsCompressed;
        long long CreationTime;
        long long CreationTime2;
        long long LastAccessTime;
        long long ModifiedTime;
        long long ModifiedTime2;
    };
    struct Headers {
        FileHeader fileHeader;
        PackageHeader packageHeader;
    };
    struct PackageEntry {
        PackageItemInfo info;
        QString name;
    };
}

class MabiPackReader
{
private:
    QFile *packFile;
    QDataStream io;
    int64_t pos;
    MabiPack::Headers headers;
    QList<MabiPack::PackageEntry*> packageEntries;
    bool fileOpen = false;
    bool readFileHeader();
    bool readPackageHeader();
    bool readPackageInfos();
    QByteArray extractFile(MabiPack::PackageEntry *file);
public:
    QByteArray extractFile(QString filename);
    bool openPackage(QString filename);
    QString findTexture(QString texture);
    void closePackage();
    MabiPackReader();

};

#endif // MABIPACKREADER_H
