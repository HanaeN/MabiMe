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
#include <QDebug>

FaceEmotionParser::FaceEmotionParser(QString name, QByteArray xml, LocaleMapHelper *localeMap) :
    XMLParser(name, xml, localeMap)
{
    if (isReady()) parseFile();
}

void FaceEmotionParser::parseFile() {
    QDomNodeList categories = doc.elementsByTagName("FaceEmotion2").at(0).childNodes();
    for (int i = 0; i < categories.count(); i++) {
        QDomNode styleCategory = categories.at(i);
        int id = EyeMouthStyle::UNKNOWN;
        if (styleCategory.nodeName() == "EyeEmotionList")    id = EyeMouthStyle::EYE_STYLE;
        if (styleCategory.nodeName() == "MouthEmotionList")  id = EyeMouthStyle::MOUTH_STYLE;
        if (id != EyeMouthStyle::UNKNOWN) {
            for (int n = 0; n < styleCategory.childNodes().count(); n++) {
                QDomNode style = styleCategory.childNodes().at(n);
                if (style.nodeName() == "Emotion") {
                    QDomNamedNodeMap attributes = style.attributes();
                    EyeMouthStyle::Object *c = new EyeMouthStyle::Object;
                    c->name = localeMap->getValue(attributes.namedItem("Name").nodeValue());
                    c->categoryType = id;
                    c->entryID = attributes.namedItem("ID").nodeValue().toInt();
                    qDebug() << id << c->name << c->entryID << attributes.namedItem("integer44").nodeValue().toInt();
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
