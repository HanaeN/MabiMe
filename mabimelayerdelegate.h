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
#include <QTreeWidgetItem>
#include <QStyledItemDelegate>

namespace LayerRole {
    const int BUTTON_DOWN = 0x100;
    const int BUTTON_HOVER = 0x101;
    const int BUTTON_HOVER_VISIBLE = 0x102;
    const int LAYER_VISIBLE = 0x103;
}

class MabiMeLayerDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MabiMeLayerDelegate(QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
     bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
    bool buttonDown = false;
    bool buttonHover = false;
    QImage branchOpen = QImage(":/images/Images/branch-open.png");
    QImage branchClosed = QImage(":/images/Images/branch-closed.png");
    QImage smallButton = QImage(":/images/Images/button-small.png");
    QImage smallButtonHover = QImage(":/images/Images/button-small-hover.png");
    QImage smallButtonPressed = QImage(":/images/Images/button-small-down.png");
    QImage smallButtonX = QImage(":/images/Images/button-small-x.png");
    QImage smallButtonVisible = QImage(":/images/Images/button-small-visible.png");
    QImage smallButtonInvisible = QImage(":/images/Images/button-small-invisible.png");
signals:
    void closeButtonClicked(QTreeWidgetItem *i);
    void visibilityButtonClicked(QTreeWidgetItem *i);
    void repaintWidget();
public slots:
};

#endif // MABIMELAYERDELEGATE_H
