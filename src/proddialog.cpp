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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

#include "proddialog.h"

ProdDialog::ProdDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	m_prodFildModel = new QStandardItemModel(this);

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

    yearModel = new QSqlQueryModel(this);
    yearModel->setQuery("SELECT DISTINCT year_of_harvest FROM fild ORDER BY year_of_harvest DESC");
    while(yearModel->canFetchMore())
    	yearModel->fetchMore();
    ui.comboBoxProdYear->setModel(yearModel);

    cropUseModel = new QSqlQueryModel(this);
    cropUseModel->setQuery(" SELECT crop_use_id, crop_use_name "
    		            " FROM crop_use "
						" WHERE crop_use_is_actv='true' "
    		            " ORDER BY crop_use_name ASC");
    while(cropUseModel->canFetchMore())
    	cropUseModel->fetchMore();

    qDebug() << "commit" << QSqlDatabase::database().commit();

    connect(ui.comboBoxProdYear, SIGNAL(currentIndexChanged(QString)),
    		this, SLOT(comboBoxProdYear_currentIndexChanged(QString)));

    if (!ui.comboBoxProdYear->currentText().isEmpty())
    	comboBoxProdYear_currentIndexChanged(ui.comboBoxProdYear->currentText());

	ui.comboBoxProdCrop->setModel(cropUseModel);
	ui.comboBoxProdCrop->setModelColumn(1);
	ui.comboBoxProdCrop->setCurrentIndex(0);
    ui.listViewProdFild->setModel(m_prodFildModel);
}

QStandardItemModel* ProdDialog::prodFildModel()
{
	return m_prodFildModel;
}

int ProdDialog::cropUseId()
{
	if (ui.comboBoxProdCrop->currentText().isEmpty())
		return -1;

	return cropUseModel->data(cropUseModel->index(ui.comboBoxProdCrop->currentIndex(), 0)).toInt();
}

void ProdDialog::comboBoxProdYear_currentIndexChanged(const QString &year)
{
	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	QSqlQuery qFild(" SELECT DISTINCT fild_id, fild_name "
			        " FROM fild "
			        " WHERE year_of_harvest=" + year + " "
			        " AND fild_id NOT IN (SELECT fild_id FROM prod WHERE year_of_harvest=" + year + ")"
			        " ORDER BY fild_name ASC ");
	m_prodFildModel->removeRows(0, m_prodFildModel->rowCount(QModelIndex()), QModelIndex());
    int row = 0;
	while(qFild.next()){
    	QStandardItem *item = new QStandardItem(qFild.value(1).toString());
    	item->setData(qFild.value(0), Qt::UserRole+1);
    	item->setCheckable(true);
    	item->setEnabled(true);
        m_prodFildModel->setItem(row, 0, item);
        row++;
    }
//	if(year.isEmpty())
//		return;
//	else
//		prodModel->setFilter("prod.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + year + ")");

	qDebug() << "commit" << QSqlDatabase::database().commit();
}

ProdDialog::~ProdDialog()
{

}
