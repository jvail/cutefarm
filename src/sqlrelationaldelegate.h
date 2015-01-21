/* 	CuteFarm
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

#ifndef SQLRELATIONALDELEGATE_H_
#define SQLRELATIONALDELEGATE_H_

#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

class SqlRelationalDelegate : public QSqlRelationalDelegate
{

public:
	SqlRelationalDelegate(QObject *parent = 0)
	: QSqlRelationalDelegate(parent)
	{}


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
    {
        const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
        QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
        if (!childModel)
            return QItemDelegate::createEditor(parent, option, index);

        while (childModel->canFetchMore())
        	childModel->fetchMore();

        QComboBox *combo = new QComboBox(parent);
        combo->setModel(childModel);
        combo->setModelColumn(childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn()));
        combo->installEventFilter(const_cast<SqlRelationalDelegate *>(this));

        return combo;

    }

};

#endif /* SQLRELATIONALDELEGATE_H_ */
