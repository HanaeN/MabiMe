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

#ifndef MABIMELAYERDELEGATE_H
#define MABIMELAYERDELEGATE_H

#include <QImage>
#include <QObject>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

class MabiMeLayerDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MabiMeLayerDelegate(QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QImage branchOpen = QImage(":/images/Images/branch-open.png");
    QImage branchClosed = QImage(":/images/Images/branch-closed.png");
signals:

public slots:
};

#endif // MABIMELAYERDELEGATE_H
