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

#include "sqlquerymodel.h"
#include <QDate>
#include <QDebug>

SqlQueryModel::SqlQueryModel(QObject *parent)
    : QSqlQueryModel(parent)
{
	m_dateCol = -1;
}

void SqlQueryModel::setDateCol(int col)
{
	m_dateCol = col;
}

void SqlQueryModel::setQuery(const QString &query)
{
	QSqlQueryModel::setQuery(query);
	while (QSqlQueryModel::canFetchMore()) {
		qDebug() << "QSqlQueryModel::canFetchMore()";
		QSqlQueryModel::fetchMore();
	}
}

QVariant SqlQueryModel::data(const QModelIndex &index, int role) const
{

	if (index.column() == m_dateCol && role == Qt::DisplayRole)
		return QDate::fromString(QSqlQueryModel::data(index, role).toString(), "yyyy-MM-dd");

	if (role == Qt::ToolTipRole)
		return QSqlQueryModel::data(index, Qt::DisplayRole);

	bool ok;

	QSqlQueryModel::data(index, role).toDouble(&ok);

	if ( ok )
		return QSqlQueryModel::data(index, role).toDouble();

	return QSqlQueryModel::data(index, role);
}
