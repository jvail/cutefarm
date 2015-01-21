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

#ifndef SQLRELATIONALTABLEMODEL_H_
#define SQLRELATIONALTABLEMODEL_H_

#include <QSqlRelationalTableModel>
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QPalette>
#include <QDebug>
#include <QMessageBox>

class SqlRelationalTableModel : public QSqlRelationalTableModel
{
	Q_OBJECT

public:
	SqlRelationalTableModel(QObject * parent = 0);
	~SqlRelationalTableModel() {};

	bool select();
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	bool pendingTransaction() { return m_pending; };
	void setPendingTransaction( bool pending );
	QSqlTableModel * relationModel ( int column ) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	void setRelation ( int column, const QSqlRelation & relation );
	Qt::ItemFlags flags (const QModelIndex &index) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	void setNoEditColumn(int col);
	void setBoolColumn(int boolColumn);
	int boolColumn() { return m_boolColumn; }
	void addDateColumn(int column);
	void addTextColumn(int column);

private:
	bool m_pending;
	int noEditCol;
	int m_boolColumn;
	QSet<int> relColumn;
	QSet<int> m_dateColumns;
	QSet<int> m_textColumns;
	QSet<int> m_noEditColumns;

};

#endif /*SQLRELATIONALTABLEMODEL_H_*/
