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

#include "characterstyleparser.h"

#include <QDebug>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>

CharacterStyleParser::CharacterStyleParser(QString name, QByteArray xml, LocaleMapHelper *localeMap) :
    XMLParser(name, xml, localeMap)
{
    if (isReady()) parseFile();
}

void CharacterStyleParser::parseFile() {
    QDomNodeList categories = doc.elementsByTagName("CharacterStyle").at(0).childNodes();
    for (int i = 0; i < categories.count(); i++) {
        QDomNode styleCategory = categories.at(i);
        int id = CharacterStyle::UNKNOWN_COLOUR;
        if (styleCategory.nodeName() == "SkinColor")         id = CharacterStyle::SKIN_COLOUR;
        if (styleCategory.nodeName() == "HairColor")         id = CharacterStyle::HAIR_COLOUR;
        if (styleCategory.nodeName() == "EyeColor")          id = CharacterStyle::EYE_COLOUR;
        if (styleCategory.nodeName() == "MaleFace")          id = CharacterStyle::MALE_FACE;
        if (styleCategory.nodeName() == "FemaleFace")        id = CharacterStyle::FEMALE_FACE;
        if (styleCategory.nodeName() == "MaleHairStyle")     id = CharacterStyle::MALE_HAIR_STYLE;
        if (styleCategory.nodeName() == "FemaleHairStyle")   id = CharacterStyle::FEMALE_HAIR_STYLE;
        for (int n = 0; n < styleCategory.childNodes().count(); n++) {
            QDomNode style = styleCategory.childNodes().at(n);
            if (style.nodeName() == "CharacterStyle") {
                QDomNamedNodeMap attributes = style.attributes();
                CharacterStyle::Object *c = new CharacterStyle::Object;
                c->name = localeMap->getValue(attributes.namedItem("Name").nodeValue());
                c->categoryType = id;
                c->entryID = attributes.namedItem("ID").nodeValue().toInt();
                styles.append(c);
            }
        }
    }
}

CharacterStyleParser::~CharacterStyleParser() {
    foreach (CharacterStyle::Object *style, styles) delete style;
    styles.clear();
}
