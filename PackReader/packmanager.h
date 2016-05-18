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

#ifndef PACKMANAGER_H
#define PACKMANAGER_H

#include <QList>

#include "PackReader/mabipackreader.h"
#include "PackReader/packxmlmanager.h"

struct Pack {
    MabiPackReader *reader = nullptr;
    QString name;
    int id;
};

struct LanguagePack {
    MabiPackReader *reader = nullptr;
    QString name = "";
    QList<PackXMLManager*> files;
};

class PackManager
{
public:
    PackManager();
    PackManager(QString path);
    ~PackManager();
    void freePackages();
    QString getPath();
    bool loadPackages();
    QString findTexture(QString name);
    bool fileExists(QString path, bool useLanguagePack = false);
    QByteArray extractFile(QString path, bool useLanguagePack = false);
private:
    QString path = "";
    QList<Pack*> packs;
    LanguagePack languagePack;
    void loadXMLData();
    bool packsLoaded = false;
    bool findMabinogiPath();
};

#endif // PACKMANAGER_H
