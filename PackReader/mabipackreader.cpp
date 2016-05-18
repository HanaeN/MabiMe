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

#include "mabipackreader.h"
#include <QDebug>
#include <QByteArray>
#include <QtEndian>
#include "mabimt.h"

MabiPackReader::MabiPackReader()
{
    packageEntries = QList<MabiPack::PackageEntry*>();
}

bool MabiPackReader::openPackage(QString filename) {
    if (fileOpen) closePackage(); // close old package first
    packFile = new QFile(filename);
    if (packFile->exists()) {
        if (packFile->open(QIODevice::ReadOnly)) {
            fileOpen = true;
            io.setDevice(packFile);
            if (readFileHeader()) {
                if (readPackageHeader()) {
                    readPackageInfos();
                    pos = packFile->pos();
                } else this->closePackage();
            } else this->closePackage();
        }
    }
    return fileOpen;
}

void MabiPackReader::closePackage() {
    if (fileOpen) {
        packFile->close();
        foreach(MabiPack::PackageEntry *entry, packageEntries) delete entry;
        packageEntries.clear();
        delete packFile;
        fileOpen = false;
    }
}

bool MabiPackReader::readFileHeader() {
    if (!fileOpen) return false;
    try {
        MabiPack::FileHeader h;
        memset(&h, 0, sizeof(MabiPack::FileHeader));
        io.readRawData((char*)&h, sizeof(MabiPack::FileHeader));
        if (memcmp(&h.Signature[0], &MabiPack::ValidHeader[0], 8) == 0) { // is it a valid pack file?
            QByteArray a((char*)&h.Signature[4], 4);
            headers.fileHeader = h;
        } else {
            qDebug() << "NOT A VALID PACK FILE";
            return false;
        }
    } catch (...) {
        qDebug() << "failed to read file header";
        return false;
    }
    return true;
}

bool MabiPackReader::readPackageHeader() {
    if (!fileOpen) return false;
    try {
        MabiPack::PackageHeader p;
        memset(&p, 0, sizeof(MabiPack::PackageHeader));
        io.readRawData((char*)&p, sizeof(MabiPack::PackageHeader));
        headers.packageHeader = p;
    } catch (...) {
        qDebug() << "failed to read package header";
        return false;
    }
    return true;
}

bool MabiPackReader::readPackageInfos() {
    if (!fileOpen) return false;
    try {
        char* infoHeader = (char*)malloc(headers.packageHeader.InfoHeaderSize);
        io.readRawData(infoHeader, headers.packageHeader.InfoHeaderSize);
        int ptr = 0;
        for (int i = 0; i < headers.packageHeader.EntryCount; i++) {
            int nameSectionLength;
            if (infoHeader[ptr] < 4) {
                nameSectionLength = 0x10 * (infoHeader[ptr] + 1);
            } else if (infoHeader[ptr] == 4) {
                nameSectionLength = 0x60;
            } else {
                nameSectionLength = (*(int*)&infoHeader[ptr + 1]) + 5;
            }
            int lengthSize = (infoHeader[ptr] > 4 ? 5 : 1);
            int nameStart = ptr + lengthSize;
            int maxNameLength = nameSectionLength - lengthSize - 1;
            QString name(QByteArray((char*)&infoHeader[nameStart], maxNameLength));
            ptr += nameSectionLength;
            MabiPack::PackageItemInfo info;
            info = *(MabiPack::PackageItemInfo*)&infoHeader[ptr];
            if (info.Zero != 0) {
                qDebug () << "Entry " + name + " is corrupted!";
            } else {
                MabiPack::PackageEntry *e = new MabiPack::PackageEntry();
                e->info = info;
                e->name = name;
                packageEntries.append(e);
            }
            ptr += sizeof(MabiPack::PackageItemInfo);
        }
        free(infoHeader);
    } catch (...) {
        qDebug() << "failed to read package info";
        return false;
    }
    return true;
}

QByteArray MabiPackReader::extractFile(MabiPack::PackageEntry *file) {
    int fileSize = file->info.CompressedSize;
    if (file->info.IsCompressed == 0) fileSize = file->info.DecompressedSize;
    char *data = (char*)malloc(fileSize + (file->info.IsCompressed == 0 ? 0 : 4));
    packFile->seek(file->info.Offset + pos);
    packFile->read((data + 4), fileSize);
    std::mt19937 generator((unsigned int)((file->info.Seed << 7) ^ 0xA9C36DE1));
    for (int i = 0; i < fileSize; i++) {
        data[i + 4] = data[i + 4] ^ generator();
    }
    *(uint32_t*)&data[0] = qToBigEndian(file->info.DecompressedSize);
    QByteArray bytes(data, fileSize + (file->info.IsCompressed == 0 ? 0 : 4));
    if (file->info.IsCompressed != 0) bytes = qUncompress(bytes);
    free(data);
    return bytes;
}

QByteArray MabiPackReader::extractFile(QString filename) {
    // if it contains a *, allow a wildcard search
    if (filename.contains("*")) {
        QStringList s = filename.split("*", QString::SkipEmptyParts);
        if (s.count() == 2) {
            foreach(MabiPack::PackageEntry *entry, packageEntries) {
                if (entry->name.startsWith(s[0]) && entry->name.endsWith(s[1])) return extractFile(entry);
            }
        } else {
            qDebug() << "could not use wildcard, invalid count" << filename;
        }
    } else {
        foreach(MabiPack::PackageEntry *entry, packageEntries) {
            if (entry->name.compare(filename, Qt::CaseInsensitive) == 0) return extractFile(entry);
        }
    }
    return QByteArray();
}

QString MabiPackReader::findTexture(QString texture) {
    foreach(MabiPack::PackageEntry *entry, packageEntries) {
        QString entryName = entry->name;
        if (entryName.endsWith(".dds")) {
            if (entryName.split("\\", QString::SkipEmptyParts).last() == texture + ".dds") return entryName;
        }
    }
    return "";
}
