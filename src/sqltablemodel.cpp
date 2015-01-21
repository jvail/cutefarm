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

#include <QDate>

#include "sqltablemodel.h"

SqlTableModel::SqlTableModel(QObject * parent )
	: QSqlTableModel(parent)
{
	m_pending = false;
	m_boolColumn = -1;
	hideZero = false;
	m_noEditCol = -1;
	sumRow = -1;
	sumCol = -1;
	showToolTip = false;

}

void SqlTableModel::setPendingTransaction( bool pending )
{
	m_pending = pending;
}

void SqlTableModel::setBoolColumn(int boolColumn)
{
	m_boolColumn = boolColumn;
}

void SqlTableModel::setZeroHidden(bool hide)
{
	hideZero = hide;
}

void SqlTableModel::setNoEditColumn( int col )
{
	m_noEditColumns << col;
}

void SqlTableModel::setSumRow(int row)
{
	sumRow = row;
}

void SqlTableModel::setSumCol(int col)
{
	sumCol = col;
}

void SqlTableModel::setToolTippShown(bool show)
{
	showToolTip = show;
}

void SqlTableModel::addDateColumn(int column)
{
	m_dateColumns << column;
}

void SqlTableModel::addTextColumn(int column)
{
	m_textColumns << column;
}


void SqlTableModel::setVertHeaderMap(QMap<int, QString> map)
{
	vertHeaderMap = map;
}

bool SqlTableModel::select()
{
	if(QSqlTableModel::select()){
		while(QSqlTableModel::canFetchMore()){
			QSqlTableModel::fetchMore();
		}
		m_pending = false;
		return true;
	}
	else{
		while(QSqlTableModel::canFetchMore()){
			QSqlTableModel::fetchMore();
		}
		return false;
	}
}

void SqlTableModel::sort(int column, Qt::SortOrder order)
{
	if (m_pending)
		return;
	QSqlTableModel::sort(column, order);

}

Qt::ItemFlags SqlTableModel::flags(const QModelIndex &index) const
{
	if (m_noEditColumns.contains(index.column()))
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable /*| Qt::ItemIsUserCheckable*/;

}

bool SqlTableModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
//	if(QSqlTableModel::setData(index, value, role)){
////		emit dataChanged(index, index);
//		return true;
//	}
//	else{
//		return false;
//	}


	if(index.row() == sumRow || index.column() == sumCol)
		return false;

	if( data( index, role ) != value ) {

		m_pending = true;
		return QSqlTableModel::setData( index, value, role );
	}

	else
		return false; //???
}

QVariant SqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
//	if (orientation == Qt::Horizontal && role == Qt::SizeHintRole) {
//	    QSize size = QSqlTableModel::headerData(section, orientation, role).toSize();
//	    size.setHeight(20 /* should be the height you want */);
//	    return size;
//	}



	if ( !vertHeaderMap.isEmpty() ) {

		if ( orientation == Qt::Vertical && role == Qt::DisplayRole )
			return vertHeaderMap.value(section);
		else
			return QSqlTableModel::headerData(section, orientation, role);

	}
	else
		return QSqlTableModel::headerData(section, orientation, role);
}

QVariant SqlTableModel::data(const QModelIndex & index, int role) const
{

	if (role != Qt::ForegroundRole && role != Qt::DisplayRole  && role != Qt::ToolTipRole)
		return QSqlTableModel::data(index, role);

	if (role == Qt::ToolTipRole)
	    return QSqlTableModel::data(index, Qt::DisplayRole);

	if (role == Qt::ForegroundRole && m_boolColumn >= 0) {
		if ( QSqlTableModel::data(index.sibling(index.row(), m_boolColumn),
				Qt::DisplayRole)== "false" ) {
			return Qt::lightGray;
		}

		if ( QSqlTableModel::data(index.sibling(index.row(), m_boolColumn),
				Qt::DisplayRole) == "true" )
			return QSqlTableModel::data(index, role);

		return QSqlTableModel::data(index, role);
	}


//	if ( showToolTip == true && index.row() != sumRow && index.column() != sumCol && role == Qt::ToolTipRole ) {
//
//        QString s1 = data(this->index(index.row(), sumCol, QModelIndex())).toString();
//        QString s2 = data(this->index(sumRow, index.column(), QModelIndex())).toString();
//        return tr("Flurstück: ") + s1 + " / " + tr("Schlag: ") + s2;
//
//	}
//
//
//	if ( QSqlTableModel::data(index, Qt::DisplayRole).toDouble() == 0.00 && hideZero == true &&
//				role == Qt::ForegroundRole && index.row() != sumRow && index.column() != sumCol )
//		return Qt::transparent;
//
	//Datentypen
	if (m_dateColumns.contains(index.column()) && (role == Qt::DisplayRole || role == Qt::EditRole))
			return QSqlTableModel::data(index, role).toDate().toString("dd.MM.yyyy");

	if (!m_textColumns.contains(index.column())) {
		bool ok;

		QSqlTableModel::data(index, role).toDouble(&ok);

		if (ok)
			return QSqlTableModel::data(index, role).toDouble();
	}


	return QSqlTableModel::data(index, role);
}
