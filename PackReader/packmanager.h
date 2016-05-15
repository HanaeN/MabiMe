#ifndef PACKMANAGER_H
#define PACKMANAGER_H

#include <QList>

#include "PackReader/mabipackreader.h"

struct Pack {
    MabiPackReader *reader;
    QString name;
    int id;
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
    QByteArray extractFile(QString path);
private:
    QString path = "";
    QList<Pack*> packs;
    bool packsLoaded = false;
    bool findMabinogiPath();
};

#endif // PACKMANAGER_H
