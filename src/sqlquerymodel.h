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

#ifndef SQLQUERYMODEL_H_
#define SQLQUERYMODEL_H_

#include <QSqlQueryModel>
#include <QLocale>

class SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT

public:
	SqlQueryModel(QObject *parent = 0);

	void setDateCol(int col);
	void setQuery(const QString &query);
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;

private:
	int m_dateCol;

};


#endif /*SQLQUERYMODEL_H_*/
