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

#include<QDate>

#include "sqlrelationaltablemodel.h"

SqlRelationalTableModel::SqlRelationalTableModel(QObject * parent )
	: QSqlRelationalTableModel(parent)
{
	noEditCol = -1;
	m_boolColumn = -1;
	m_pending = false;
}

void SqlRelationalTableModel::setNoEditColumn(int col)
{
	m_noEditColumns << col;
}

void SqlRelationalTableModel::setBoolColumn(int boolColumn)
{
	m_boolColumn = boolColumn;
}

void SqlRelationalTableModel::addDateColumn(int column)
{
	m_dateColumns << column;
}

void SqlRelationalTableModel::addTextColumn(int column)
{
	m_textColumns << column;
}

void SqlRelationalTableModel::setPendingTransaction( bool pending )
{
	m_pending = pending;
}

Qt::ItemFlags SqlRelationalTableModel::flags(const QModelIndex &index) const
{
	if (m_noEditColumns.contains(index.column()))
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable /*| Qt::ItemIsUserCheckable*/;

}

bool SqlRelationalTableModel::select()
{
//	QSqlRelationalTableModel::revertAll();
	if(QSqlRelationalTableModel::select()){
		while(QSqlRelationalTableModel::canFetchMore()){
			QSqlRelationalTableModel::fetchMore();
		}
//		relationModel(relColumn.first())->select();
		QSetIterator<int> i(relColumn);
        while (i.hasNext()){
        	int col = i.next();
//        	QMessageBox::information(0, this->objectName(), QString::number(col));
        	if(QSqlRelationalTableModel::relationModel(col)->select()){
        		while(QSqlRelationalTableModel::relationModel(col)->canFetchMore()){
        			QSqlRelationalTableModel::relationModel(col)->fetchMore();
        		}
        	}
        }
        m_pending = false;
		return true;
	}
	else{
		return false;
	}
}

void SqlRelationalTableModel::sort ( int column, Qt::SortOrder order)
{
	if (m_pending)
		return;
	QSqlRelationalTableModel::sort(column, order);

}

bool SqlRelationalTableModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
	qDebug() << QSqlRelationalTableModel::data(index, role) << value << role;

	if ( relColumn.contains(index.column()) ) {

		qDebug() << QSqlRelationalTableModel::data(index, role) << value;

		if ( QSqlRelationalTableModel::data(index, role) != value  && role == Qt::DisplayRole ) {

			m_pending = true;
			return QSqlRelationalTableModel::setData(index, value, role);

		}

		else if ( role == Qt::EditRole ) {

			int col = QSqlRelationalTableModel::relationModel(index.column())->fieldIndex(
					QSqlRelationalTableModel::relation(index.column()).displayColumn());

			QModelIndex idx = QSqlRelationalTableModel::relationModel(index.column())->match(
					QSqlRelationalTableModel::relationModel(index.column())->index(0,0),
					Qt::DisplayRole, value, 1).first();

			if ( idx.sibling(idx.row(), col).data() != QSqlRelationalTableModel::data(index, Qt::EditRole) ) {

				qDebug() << idx.sibling(idx.row(), col).data() << QSqlRelationalTableModel::data(index, Qt::EditRole);
				m_pending = true;
				return QSqlRelationalTableModel::setData(index, value, role);

			}

			else
				return true; //???
		}
		else
			return true; //???
	}

	else {

		if ( QSqlRelationalTableModel::data(index, role) != value ) {

			qDebug() << QSqlRelationalTableModel::data(index, role) << value;
			m_pending = true;
			return QSqlRelationalTableModel::setData(index, value, role);

		}

		else
			return true; //???
	}

}

QSqlTableModel* SqlRelationalTableModel::relationModel ( int column ) const
{
	QSqlTableModel *model = QSqlRelationalTableModel::relationModel(column);
	while (model->canFetchMore())
		model->fetchMore();
	return model;
}

void SqlRelationalTableModel::setRelation ( int column, const QSqlRelation & relation )
{
	QSqlRelationalTableModel::setRelation(column, relation);
	relColumn << column;
}

QVariant SqlRelationalTableModel::data(const QModelIndex & index, int role) const
{

	if (role != Qt::ForegroundRole && role != Qt::DisplayRole  && role != Qt::ToolTipRole)
		return QSqlTableModel::data(index, role);

    if (role == Qt::ToolTipRole)
        return QSqlTableModel::data(index, Qt::DisplayRole);

	if ( role == Qt::ForegroundRole && m_boolColumn >= 0 ) {

		if ( QSqlRelationalTableModel::data(index.sibling(index.row(), m_boolColumn),
				Qt::DisplayRole)== "false" ) {
			return Qt::lightGray;
		}

		else if ( QSqlRelationalTableModel::data(index.sibling(index.row(), m_boolColumn),
				Qt::DisplayRole) == "true" ) {
			return QSqlRelationalTableModel::data(index, role);
		}

		else
			return QSqlRelationalTableModel::data(index, role);
	}

	//Datentypen
	if (m_dateColumns.contains(index.column()) && (role == Qt::DisplayRole || role == Qt::EditRole))
			return QSqlRelationalTableModel::data(index, role).toDate().toString("dd.MM.yyyy");



	if (!m_textColumns.contains(index.column())) {
		bool ok;
		QSqlRelationalTableModel::data(index, role).toDouble(&ok);

		if ( ok )
			return QSqlRelationalTableModel::data(index, role).toDouble();
	}

	return QSqlRelationalTableModel::data(index, role);
}
