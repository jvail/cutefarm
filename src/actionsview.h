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

#ifndef TREEVIEW_H_
#define TREEVIEW_H_

#include <QStyledItemDelegate>
#include <QTreeView>


class ActionsViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ActionsViewDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;

};


class ActionsView : public QTreeView {

    Q_OBJECT

public:
    ActionsView(QWidget *parent);

    int sizeHintForRow(int row) const;
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index ) const;

private:
    QColor mDark;
};


#endif /* TREEVIEW_H_ */
