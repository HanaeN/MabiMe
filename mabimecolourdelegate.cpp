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

#include "mabimecolourdelegate.h"

#include <QListWidgetItem>
#include <QPainter>

MabiMeColourDelegate::MabiMeColourDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QSize MabiMeColourDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QSize(27, 27);
}

void MabiMeColourDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QListWidgetItem *i = (QListWidgetItem*)index.internalPointer();
    QColor bg = QColor(i->data(0x100).toInt(), i->data(0x101).toInt(), i->data(0x102).toInt());
    painter->fillRect(option.rect, bg);
    painter->setPen(QColor(0, 0, 0, 50));
    painter->drawRect(option.rect.adjusted(1, 1, -1, -1));
    QPen p(QColor(0, 0, 0, 120));
    painter->setPen(p);

    QLinearGradient fgGradient(0, option.rect.top(), 0, option.rect.bottom());
    fgGradient.setColorAt(0, QColor(255, 255, 255, 100));
    fgGradient.setColorAt(0.1, QColor(180, 180, 180, 10));
    fgGradient.setColorAt(0.6, QColor(125, 125, 125, 0));
    fgGradient.setColorAt(1, QColor(255, 255, 255, 30));

    painter->fillRect(option.rect, fgGradient);

    if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
        if (option.state & QStyle::State_MouseOver) painter->fillRect(option.rect, QColor(255, 255, 255, 20));
        painter->drawRect(option.rect.adjusted(1, 1, -1, -1));
        painter->drawRect(option.rect.adjusted(2, 2, -2, -2));
        p.setColor(QColor(255, 255, 255, 120));
        painter->setPen(p);
        painter->drawRect(option.rect.adjusted(3, 3, -3, -3));
    }
}
