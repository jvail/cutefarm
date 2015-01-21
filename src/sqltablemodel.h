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

#ifndef SQLTABLEMODEL_H_
#define SQLTABLEMODEL_H_

#include <QSqlTableModel>
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QPalette>
#include <QDebug>
#include <QLocale>

class SqlTableModel : public QSqlTableModel
{
	Q_OBJECT

public:
	SqlTableModel(QObject * parent = 0);
	~SqlTableModel() {};

	bool pendingTransaction() { return m_pending; };
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	void setPendingTransaction( bool pending );
	void setBoolColumn(int boolColumn);
	int boolColumn() { return m_boolColumn; }
	void setSumRow(int row);
	void setSumCol(int col);
	void setZeroHidden(bool hide);
	void setNoEditColumn(int col);
	void setToolTippShown(bool show);
	void addDateColumn(int column);
	void addTextColumn(int column);
	void setVertHeaderMap(QMap<int, QString> map);
	bool select();
	bool setData(const QModelIndex &index, const QVariant &value,
            int role = Qt::EditRole);
	QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags (const QModelIndex &index) const;

private:

	bool m_pending;
	QLocale german;
	int m_boolColumn;
	int m_noEditCol;
	int sumRow;
	int sumCol;
	bool hideZero;
	bool showToolTip;
	QSet<int> m_dateColumns;
	QSet<int> m_textColumns;
	QSet<int> m_noEditColumns;
	QMap<int, QString> vertHeaderMap;
};

#endif /*SQLTABLEMODEL_H_*/
