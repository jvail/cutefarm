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
#include <QCompleter>

#include "fertdialog.h"
#include "settingsdialog.h"

FertDialog::FertDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	m_FertId = 0;
	m_maxFertTypId = 0;
	m_newFertTyp = false;
	
	qDebug() << "transaction" << QSqlDatabase::database().transaction();
	
	QSqlQuery q;
	q.exec(" SELECT max(fert_id)+1 "
		   " FROM fert ");
	while (q.next())
		m_FertId = q.value(0).toInt();
		
	q.exec("SELECT fert_typ_id, fert_typ_name FROM fert_typ ORDER BY fert_typ_name ASC");
	int index = 0;
	while (q.next()) {
		ui.comboBoxType->insertItem(index, q.value(1).toString(), q.value(0));
		if (q.value(0).toInt() > m_maxFertTypId)
			m_maxFertTypId = q.value(0).toInt();
		index++;
	}
	
	qDebug() << "commit" << QSqlDatabase::database().commit();
	
	ui.comboBoxType->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    ui.label_11->setText(tr("Contents %1/unit (use negative figures e.g.in case of CaO/Humus withdrawal)")
            .arg(SettingsDialog::nutUnit()));

    ui.doubleSpinBoxCao->setSuffix(" " + SettingsDialog::nutUnit());
    ui.doubleSpinBoxK2o->setSuffix(" " + SettingsDialog::nutUnit());
    ui.dSBHumus->setSuffix(" " + SettingsDialog::nutUnit());
    ui.doubleSpinBoxMgo->setSuffix(" " + SettingsDialog::nutUnit());
    ui.doubleSpinBoxN->setSuffix(" " + SettingsDialog::nutUnit());
    ui.doubleSpinBoxP2o5->setSuffix(" " + SettingsDialog::nutUnit());
    ui.doubleSpinBoxS->setSuffix(" " + SettingsDialog::nutUnit());

}

QString FertDialog::fertName() const
{
	return ui.lineEdit->text().simplified();
}

int FertDialog::fertTypId()
{
    if (ui.comboBoxType->findText(ui.comboBoxType->currentText().simplified(), Qt::MatchFixedString) >= 0) {
    	return ui.comboBoxType->itemData(ui.comboBoxType->findText(ui.comboBoxType->currentText(), Qt::MatchFixedString)
    			, Qt::UserRole).toInt();
    }
    else {
    	m_newFertTyp = true;
    	return m_maxFertTypId+1;
    }
}

QString FertDialog::fertTypName() const
{
	return ui.comboBoxType->currentText().simplified();
}

int FertDialog::fertId() const
{
    return m_FertId;
}

bool FertDialog::newFertTyp() const
{
	return m_newFertTyp;
}

QString FertDialog::fertUnit() const
{
	return ui.comboBoxUnit->currentText().simplified();
}

double FertDialog::n() const
{
    return ui.doubleSpinBoxN->value();
}

double FertDialog::k2o() const
{
    return ui.doubleSpinBoxK2o->value();
}

double FertDialog::p2o5() const
{
    return ui.doubleSpinBoxP2o5->value();
}

double FertDialog::s() const
{
    return ui.doubleSpinBoxS->value();
}

double FertDialog::mgo() const
{
    return ui.doubleSpinBoxMgo->value();
}

double FertDialog::cao() const
{
    return ui.doubleSpinBoxCao->value();
}

double FertDialog::humus() const
{
    return ui.dSBHumus->value();
}

FertDialog::~FertDialog()
{

}
