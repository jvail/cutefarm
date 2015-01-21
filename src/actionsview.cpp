/*  CuteFarm
    Copyright (C) 2008-2011  Jan Vaillant

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation License 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include <QPainter>
#include <QApplication>
#include <QDebug>

#include "actionsview.h"
#include "actionsviewmodel.h"

ActionsViewDelegate::ActionsViewDelegate(QObject *parent)
     : QStyledItemDelegate(parent)
{

}

QWidget* ActionsViewDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &option,
     const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ActionsViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void ActionsViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void ActionsViewDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void ActionsViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    QStyledItemDelegate::initStyleOption(&opt, index);
    ActionsViewItem *item = static_cast<ActionsViewItem*>(index.internalPointer());

    switch (item->getType()) {
        case ActionsViewModel::Type_Oprt:
            opt.font.setBold(true);
            opt.fontMetrics = QFontMetrics(opt.font);
            opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
            painter->fillRect(option.rect, opt.palette.color(QPalette::Dark));
            break;
        case ActionsViewModel::Type_Sum:
        case ActionsViewModel::Type_Fild:
            painter->fillRect(option.rect, opt.palette.color(QPalette::AlternateBase));
            break;
        default:
            break;
    }

    opt.state &= ~QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, opt, index);

    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
//    painter->drawLine(opt.rect.right(), opt.rect.top(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}

QSize ActionsViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
//    QStyleOptionViewItemV4 opt = option;
//    QStyledItemDelegate::initStyleOption(&opt, index);
//    ActionsViewItem *item = static_cast<ActionsViewItem*>(index.internalPointer());
//
//    switch (item->getType()) {
//        case ActionsViewModel::Type_Oprt: {
//            opt.font.setBold(true);
//            opt.fontMetrics = QFontMetrics(opt.font);
//            QRect rect = opt.fontMetrics.boundingRect(index.data(Qt::DisplayRole).toString());
//            qDebug() << rect;
//            return QSize(rect.width()+5, 20);
//        }
//        default:
//            return QSize(QStyledItemDelegate::sizeHint(option, index).width()+3, 20);
//    }

    return QSize(QStyledItemDelegate::sizeHint(option, index).width()+3, 20);
}



ActionsView::ActionsView(QWidget *parent)
    : QTreeView(parent)
{
    mDark = palette().color(QPalette::Dark);
    setItemDelegate(new ActionsViewDelegate(this));
}

int ActionsView::sizeHintForRow(int row) const
{
    Q_UNUSED(row)

    return 45;
}

void ActionsView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    ActionsViewItem *item = static_cast<ActionsViewItem*>(index.internalPointer());
    switch (item->getType()) {
        case ActionsViewModel::Type_Oprt:
            opt.font.setBold(true);
            opt.fontMetrics = QFontMetrics(opt.font);
            opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
            painter->fillRect(option.rect, (opt.palette.color(QPalette::Dark).isValid()) ? opt.palette.color(QPalette::Dark) : Qt::gray);
            break;
        case ActionsViewModel::Type_Sum:
        case ActionsViewModel::Type_Fild:
            painter->fillRect(option.rect, opt.palette.color(QPalette::AlternateBase));
            break;
        default:
            break;
    }

    opt.state &= ~QStyle::State_HasFocus;

    QTreeView::drawRow(painter, opt, index);

    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
//    painter->drawLine(opt.rect.right(), opt.rect.top(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}


