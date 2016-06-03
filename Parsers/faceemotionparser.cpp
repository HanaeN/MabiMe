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

#include "faceemotionparser.h"
#include <QByteArray>
#include <QDebug>
#include <QPixmap>
#include <QStringList>

FaceEmotionParser::FaceEmotionParser(QString name, QByteArray xml, LocaleMapHelper *localeMap, PackManager *packManager) :
    XMLParser(name, xml, localeMap)
{
    this->packManager = packManager;
    if (isReady()) parseFile();
}

void FaceEmotionParser::parseFile() {
    QDomNodeList categories = doc.elementsByTagName("FaceEmotion2").at(0).childNodes();
    for (int i = 0; i < categories.count(); i++) {
        QDomNode styleCategory = categories.at(i);
        int id = EyeMouthStyle::UNKNOWN;
        QString typeString;
        if (styleCategory.nodeName() == "EyeEmotionList") {
            id = EyeMouthStyle::EYE_STYLE;
            typeString = "eye";
        }
        if (styleCategory.nodeName() == "MouthEmotionList") {
            id = EyeMouthStyle::MOUTH_STYLE;
            typeString = "mouth";
        }
        if (id != EyeMouthStyle::UNKNOWN) {
            int totalItems = 0;
            for (int n = 0; n < styleCategory.childNodes().count(); n++) {
                if (styleCategory.childNodes().at(n).nodeName() == "Emotion") totalItems++;
            }

            int currentItem = 0;
            for (int n = 0; n < styleCategory.childNodes().count(); n++) {
                QDomNode style = styleCategory.childNodes().at(n);
                if (style.nodeName() == "Emotion") {
                    packManager->currentLanguagePackProgress("Parsing " + typeString + " icons", currentItem, totalItems);
                    currentItem++;
                    QDomNamedNodeMap attributes = style.attributes();
                    EyeMouthStyle::Object *c = new EyeMouthStyle::Object;
                    c->name = localeMap->getValue(attributes.namedItem("Name").nodeValue());
                    c->categoryType = id;
                    c->entryID = attributes.namedItem("ID").nodeValue().toInt();
                    QStringList args = attributes.namedItem("FaceId").nodeValue().split(",", QString::SkipEmptyParts);
                    foreach (QString arg, args) {
                        if (arg.contains("~")) {
                            QStringList argLowHigh = arg.split("~");
                            int argLow = argLowHigh[0].toInt();
                            int argHigh = argLowHigh[1].toInt();
                            for (int i = argLow; i <= argHigh; i++) {
                                c->allowedHeads.append(i);
                            }
                        } else if (arg != "*") c->allowedHeads.append(arg.toInt());
                    }
                    QPixmap pix;
                    QByteArray bytes = packManager->extractFile("material\\char\\face\\" + typeString + "_" + QString::number(c->entryID) + ".dds");
                    if (bytes.length() > 0) {
                        pix.loadFromData(bytes);
                        QImage img = pix.toImage();
                        c->icon = img;
                    }
                    styles.append(c);
                }
            }
        }
    }
}

FaceEmotionParser::~FaceEmotionParser() {
    foreach (EyeMouthStyle::Object *style, styles) delete style;
    styles.clear();
}
