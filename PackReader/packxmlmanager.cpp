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

#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "packxmlmanager.h"

PackXMLManager::PackXMLManager(QString name, MabiPackReader *reader)
{
    this->name = name;
    QString txtFile = reader->findFile("xml\\" + name + ".*.txt");
    if (txtFile.length() > 0) {
        QByteArray b = reader->extractFile(txtFile);
        QTextStream txtStream(b);
        QString line = txtStream.readLine();
        QRegularExpression regex("(\\S+)\\s+(.*)");
        QRegularExpressionMatch match;
        while (!line.isNull()) {
            match = regex.match(line);
            if (match.hasMatch()) lookup[match.captured(1)] = match.captured(2);
            line = txtStream.readLine();
        }
    } else {
        qDebug() << "FATAL ERROR: PackXMLManager could not find " << name << "txt";
    }
}

