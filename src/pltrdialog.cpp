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

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

#include "pltrdialog.h"

PltrDialog::PltrDialog(QWidget *parent, const QString &name,
        const QString &unit, int pltrId)
    : QDialog(parent)
{
	ui.setupUi(this);

	bool isUpdate = (pltrId > -1);

	m_pltrId = pltrId;

	pltrSubModel = new QStandardItemModel(0, 2, this);
	pltrTypModel = new QStandardItemModel(0, 2, this);

	QSqlQuery q;
	if (!QSqlDatabase::database().transaction()) {
		qDebug() << "rollback";
		QSqlDatabase::database().rollback();
		QSqlDatabase::database().transaction();
	}

	QSet<int> types;
	QSet<int> subs;

	if (!isUpdate) {
	q.exec(" SELECT ifnull(max(pltr_id)+1, 1) "
		   " FROM pltr ");
	while (q.next())
		m_pltrId = q.value(0).toInt();
	}
	else {
	    ui.lineEditName->setText(name);
	    if (!unit.trimmed().isEmpty()) {
            ui.comboBox->addItem(unit);
            ui.comboBox->setCurrentIndex(ui.comboBox->count()-1);
	    }
	    q.prepare("SELECT pltr_typ_id FROM pltr_x_pltr_typ "
	            " WHERE pltr_id=:pltr_id");
	    q.bindValue(":pltr_id", m_pltrId);
	    q.exec();
	    while (q.next())
	        types << q.value(0).toInt();

        q.prepare("SELECT pltr_sub_id FROM pltr_x_pltr_sub "
                " WHERE pltr_id=:pltr_id");
        q.bindValue(":pltr_id", m_pltrId);
        q.exec();
        while (q.next())
            subs << q.value(0).toInt();
	}

	q.exec(" SELECT pltr_sub_id, pltr_sub_name "
		   " FROM pltr_sub ORDER BY pltr_sub_name ASC ");

    int row = 0;
	while (q.next()) {
		for(int col = 0; col < 2; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 0){
	    		item->setData(q.value(col) , Qt::DisplayRole);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    		item->setCheckState((subs.contains(q.value(0).toInt())) ? Qt::Checked : Qt::Unchecked);
	    	}
	    	pltrSubModel->setItem(row, col, item);
	   }
        row++;
	}

	ui.listViewWirkstoff->setModel(pltrSubModel);
	ui.listViewWirkstoff->setModelColumn(1);


	q.exec(" SELECT pltr_typ_id, pltr_typ_name "
		   " FROM pltr_typ ORDER BY pltr_typ_name ASC ");

    row = 0;
	while (q.next()) {
		for(int col = 0; col < 2; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 0){
	    		item->setData(q.value(col) , Qt::DisplayRole);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    		item->setCheckState((types.contains(q.value(0).toInt())) ? Qt::Checked : Qt::Unchecked);
	    	}
	    	pltrTypModel->setItem(row, col, item);
	   }
        row++;
	}

	ui.listViewTyp->setModel(pltrTypModel);
	ui.listViewTyp->setModelColumn(1);

	QSqlDatabase::database().commit();

//	connect(ui.buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));

}

int PltrDialog::pltrId() const
{
	return m_pltrId;
}

QString PltrDialog::pltrName() const
{
	return ui.lineEditName->text().simplified();
}

QString PltrDialog::pltrUnit() const
{
	return ui.comboBox->currentText().simplified();
}

QList<int> PltrDialog::pltrSubIds()
{
    QList<int> subs;
	for (int i = 0; i < pltrSubModel->rowCount(); i++) {
		if (pltrSubModel->item(i, 1)->data(Qt::CheckStateRole) == Qt::Checked)
		    subs << pltrSubModel->item(i, 0)->data(Qt::DisplayRole).toInt();
	}
	return subs;
}

QList<int> PltrDialog::pltrTypIds()
{
    QList<int> types;
	for (int i = 0; i < pltrTypModel->rowCount(); i++) {
		if (pltrTypModel->item(i, 1)->data(Qt::CheckStateRole) == Qt::Checked)
		    types << pltrTypModel->item(i, 0)->data(Qt::DisplayRole).toInt();
	}
	return types;
}

//void PltrDialog::save()
//{
//	qDebug() << pltrTypIds();
//	qDebug() << pltrSubIds();
//	qDebug() << pltrName();
//	qDebug() << pltrId();
//	QString name = ui.lineEditName->text();
//	QString psmId;
//
//	QList<QString> wirkstoffIdList;
//	for (int i = 0; i < pltrSubModel->rowCount(); i++) {
//		if (pltrSubModel->item(i, 1)->data(Qt::CheckStateRole) == Qt::Checked)
//			wirkstoffIdList << pltrSubModel->item(i, 0)->data(Qt::DisplayRole).toString();
//	}
//
//
//	QList<QString> typIdList;
//	for (int i = 0; i < typModel->rowCount(); i++) {
//		if (typModel->item(i, 1)->data(Qt::CheckStateRole) == Qt::Checked)
//			typIdList << typModel->item(i, 0)->data(Qt::DisplayRole).toString();
//	}
//
//
//	if (name.isEmpty() || wirkstoffIdList.isEmpty() || typIdList.isEmpty()) {
//		fehlerMsg(tr("Mindestens ein Eingabefeld ist leer."));
//		return;
//	}
//
//	QSqlQuery q;
//	if (!QSqlDatabase::database().transaction()) {
//		qDebug() << "rollback";
//		QSqlDatabase::database().rollback();
//		QSqlDatabase::database().transaction();
//	}
//
//	q.exec(" SELECT max(psm_id)+1 "
//		   " FROM psm ");
//	while (q.next())
//		psmId = q.value(0).toString();
//
//	if (q.lastError().type() != QSqlError::NoError) {
//		QSqlDatabase::database().rollback();
//		fehlerMsg(q.lastError().text());
//		return;
//	}
//
//
//	q.prepare(" INSERT INTO psm "
//			  " VALUES(:psmId, :name) ");
//	q.bindValue(":psmId", psmId);
//	q.bindValue(":name", name);
//	q.exec();
//
//	if (q.lastError().type() != QSqlError::NoError) {
//		QSqlDatabase::database().rollback();
//		fehlerMsg(q.lastError().text() + tr("\nMöglicherweise ist der Name oder die Nummer bereits vergeben."));
//		return;
//	}
//
//	foreach (QString wirkstoffId, wirkstoffIdList) {
//		q.prepare(" INSERT INTO psm_x_wirkstoff "
//				  " VALUES(:psmId, :wirkstoffId) ");
//		q.bindValue(":psmId", psmId);
//		q.bindValue(":wirkstoffId", wirkstoffId);
//		q.exec();
//	}
//
//	if (q.lastError().type() != QSqlError::NoError) {
//		QSqlDatabase::database().rollback();
//		fehlerMsg(q.lastError().text() + tr("\nMöglicherweise ist der Name oder die Nummer bereits vergeben."));
//		return;
//	}
//
//	foreach (QString typId, typIdList) {
//		q.prepare(" INSERT INTO psm_x_typ "
//				  " VALUES(:psmId, :typId) ");
//		q.bindValue(":psmId", psmId);
//		q.bindValue(":typId", typId);
//		q.exec();
//	}
//
//	if (q.lastError().type() != QSqlError::NoError) {
//		QSqlDatabase::database().rollback();
//		fehlerMsg(q.lastError().text() + tr("\nMöglicherweise ist der Name oder die Nummer bereits vergeben."));
//		return;
//	}
//
//	if (!QSqlDatabase::database().commit()) {
//		QSqlDatabase::database().rollback();
//		fehlerMsg(q.lastError().text() + tr("\nMöglicherweise ist der Name oder die Nummer bereits vergeben."));
//		return;
//	}
//
//	for (int i = 0; i < pltrSubModel->rowCount(); i++) {
//		pltrSubModel->item(i, 1)->setData(Qt::Unchecked, Qt::CheckStateRole);
//	}
//
//	for (int i = 0; i < typModel->rowCount(); i++) {
//		typModel->item(i, 1)->setData(Qt::Unchecked, Qt::CheckStateRole);
//	}
//
//	QMessageBox::information(0, tr("Ok"), tr("PSM " ) + name + tr(" eingetragen."));
//
//	ui.lineEditName->clear();
//}

//void PltrDialog::fehlerMsg(const QString &msg)
//{
//	QMessageBox::information(0, tr("Error"), msg);
//}

PltrDialog::~PltrDialog()
{

}
