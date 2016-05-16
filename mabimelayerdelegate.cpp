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

#include "mabimelayerdelegate.h"

#include <QStyledItemDelegate>
#include <QLinearGradient>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

MabiMeLayerDelegate::MabiMeLayerDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QSize MabiMeLayerDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QTreeWidgetItem *t = (QTreeWidgetItem*)index.internalPointer();
    if (t->text(0).startsWith("$MODEL$")) {
        return QSize(0, 48);
    } else {
        return QSize(0, 32);
    }
}

void MabiMeLayerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QTreeWidgetItem *t = (QTreeWidgetItem*)index.internalPointer();
    bool isModel = t->text(0).startsWith("$MODEL$") ? true : false;
    int marginL = 15;
    int sX = option.rect.left() + marginL;
    int sY = option.rect.top();
    QLinearGradient fgGradient(0, sY, 0, sY + option.rect.height());
    fgGradient.setColorAt(0, QColor(255, 255, 255));
    fgGradient.setColorAt(0.1, QColor(200, 240, 255));
    fgGradient.setColorAt(1, QColor(230, 240, 255));

    QLinearGradient fgGradientHover(0, sY, 0, sY + option.rect.height());
    fgGradientHover.setColorAt(0, QColor(255, 255, 255));
    fgGradientHover.setColorAt(0.1, QColor(220, 245, 255));
    fgGradientHover.setColorAt(1, QColor(240, 250, 255));

    QLinearGradient fgGradientGrey(0, sY, 0, sY + option.rect.height());
    fgGradientGrey.setColorAt(0, QColor(255, 255, 255));
    fgGradientGrey.setColorAt(0.1, QColor(230, 230, 230));
    fgGradientGrey.setColorAt(1, QColor(240, 240, 240));

    if (isModel) {
        painter->fillRect(option.rect, fgGradientGrey);
        if (option.state & QStyle::State_Selected) painter->fillRect(option.rect, fgGradient);
        if (option.state & QStyle::State_MouseOver) {
            painter->fillRect(option.rect, fgGradientHover);
        }
        bool buttonHover = t->data(0, 0x0100).toBool();
        bool buttonDown = t->data(0, 0x0101).toBool();
        if (buttonHover && !buttonDown) {
            painter->drawImage(QRect(option.rect.right() - 24, sY + 14, 20, 20), smallButtonHover);
        } else if (buttonDown) {
            painter->drawImage(QRect(option.rect.right() - 24, sY + 14, 20, 20), smallButtonPressed);
        } else {
            painter->drawImage(QRect(option.rect.right() - 24, sY + 14, 20, 20), smallButton);
        }
        painter->drawImage(QRect(option.rect.right() - 24, sY + 14, 20, 20), smallButtonX);
    }

    QFont f = painter->font();
    f.setPixelSize(isModel ? 12 : 10);
    painter->setFont(f);

    painter->setPen(QColor(180, 200, 210));
    painter->drawRect(option.rect.adjusted(-1, 0, 1, 0));
    painter->setPen(Qt::SolidLine);
    if (t->childCount() > 0) {
        painter->drawImage(sX - 5, sY + 19, t->isExpanded() ? branchOpen : branchClosed);
    }
    if (isModel) {
        painter->drawText(QRect(sX + 12, sY, option.rect.width(), option.rect.height()), t->text(0).mid(7), QTextOption(Qt::AlignVCenter));
    } else {
        painter->drawText(QRect(sX + 12, sY, option.rect.width(), option.rect.height()), t->text(0), QTextOption(Qt::AlignVCenter));
    }
}

bool MabiMeLayerDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    QTreeWidgetItem *t = (QTreeWidgetItem*)index.internalPointer();
    bool isModel = t->text(0).startsWith("$MODEL$") ? true : false;
    if (isModel) {
        QMouseEvent *e = (QMouseEvent*)event;
        int sY = option.rect.top();
        bool bH = t->data(0, 0x0100).toBool();
        buttonDown = t->data(0, 0x0101).toBool();
        buttonHover = false;
        if (e->pos().x() >= option.rect.right() - 24 && e->pos().x() < option.rect.right() - 4) {
            if (e->pos().y() >= sY + 14 && e->pos().y() < sY + 34) buttonHover = true;
        }
        if (buttonDown && !buttonHover) {
            buttonDown = false;
            t->setData(0, 0x0101, buttonDown);
            emit repaintWidget();
        }
        t->setData(0, 0x0100, buttonHover);
        if (event->type() == QEvent::MouseButtonPress) {
            if (e->buttons() == Qt::LeftButton && buttonHover) {
                buttonDown = true;
                t->setData(0, 0x0101, buttonDown);
                emit repaintWidget();
                return true;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            if (e->buttons() != Qt::LeftButton) {
                emit closeButtonClicked((buttonDown && buttonHover) ? t : nullptr);
                return true;
            }
        }
        emit repaintWidget();
    }
}
