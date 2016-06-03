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

#ifndef FACEMOTIONPARSER_H
#define FACEMOTIONPARSER_H

#include <QString>
#include <QByteArray>
#include <QImage>

#include "Parsers/localemaphelper.h"
#include "Parsers/xmlparser.h"
#include "PackReader/packmanager.h"

namespace EyeMouthStyle {
    const int EYE_STYLE             = 0;
    const int MALE_EYE_STYLE        = 1;
    const int FEMALE_EYE_STYLE      = 2;
    const int MOUTH_STYLE           = 3;
    const int MALE_MOUTH_STYLE      = 4;
    const int FEMALE_MOUTH_STYLE    = 5;

    const int UNKNOWN               = -1;

    struct Object {
        int categoryType;
        QImage icon;
        QString name = "";
        int entryID = 0;
        QList<int> allowedHeads;
    };
};


class FaceEmotionParser : public XMLParser
{
public:
    FaceEmotionParser(QString name, QByteArray xml, LocaleMapHelper *localeMap = nullptr, PackManager *packManager = nullptr);
    QList<EyeMouthStyle::Object*> styles;
    ~FaceEmotionParser();
protected:
    PackManager *packManager;
    virtual void parseFile();
};

#endif // FACEMOTIONPARSER_H
