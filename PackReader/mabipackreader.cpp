#include "mabipackreader.h"
#include <QDebug>
#include <QByteArray>
#include <QtEndian>
#include "mabimt.h"

MabiPackReader::MabiPackReader()
{
    packageEntries = QList<MabiPack::PackageEntry*>();
}

bool MabiPackReader::OpenPackage(QString filename) {
    if (fileOpen) ClosePackage(); // close old package first
    packFile = new QFile(filename);
    if (packFile->exists()) {
        if (packFile->open(QIODevice::ReadOnly)) {
            qDebug() << "Successefully opened " + filename + " for reading.";
            fileOpen = true;
            io.setDevice(packFile);
            if (ReadFileHeader()) {
                if (ReadPackageHeader()) {
                    ReadPackageInfos();
                    pos = packFile->pos();
                } else this->ClosePackage();
            } else this->ClosePackage();
        }
    }
    return fileOpen;
}

void MabiPackReader::ClosePackage() {
    if (fileOpen) {
        packFile->close();
        if (packageEntries.count() > 0) {
            for (int i = 0; i < packageEntries.count(); i++) {
                delete packageEntries[0];
                packageEntries.removeAt(0);
            }
        }
        delete packFile;
        fileOpen = false;
        qDebug() << "pack has been closed.";
    }
}

bool MabiPackReader::ReadFileHeader() {
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

bool MabiPackReader::ReadPackageHeader() {
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

bool MabiPackReader::ReadPackageInfos() {
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

QByteArray MabiPackReader::ExtractFile(MabiPack::PackageEntry file) {
    int fileSize = file.info.CompressedSize;
    if (file.info.IsCompressed == 0) fileSize = file.info.DecompressedSize;
    char *data = (char*)malloc(fileSize + (file.info.IsCompressed == 0 ? 0 : 4));
    packFile->seek(file.info.Offset + pos);
    packFile->read((data + 4), fileSize);
    std::mt19937 generator((unsigned int)((file.info.Seed << 7) ^ 0xA9C36DE1));
    for (int i = 0; i < fileSize; i++) {
        data[i + 4] = data[i + 4] ^ generator();
    }
    *(uint32_t*)&data[0] = qToBigEndian(file.info.DecompressedSize);
    QByteArray bytes(data, fileSize + (file.info.IsCompressed == 0 ? 0 : 4));
    if (file.info.IsCompressed != 0) bytes = qUncompress(bytes);
    free(data);
    return bytes;
}

QByteArray MabiPackReader::ExtractFile(QString filename) {
    for (int i = 0; i < packageEntries.count(); i++) {
        if (packageEntries[i]->name == filename) {
            return ExtractFile(*packageEntries.at(i));
        }
    }
    return QByteArray();
}
