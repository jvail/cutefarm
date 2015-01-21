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
#include <QCompleter>
#include <QDate>
#include <QDebug>

#include "filddialog.h"

FildDialog::FildDialog(bool noLots, QWidget *parent, int year)
    : QDialog(parent)
{
	ui.setupUi(this);

	qDebug() << "FildDialog"  << year << QDate::currentDate().year();

    if (year < 0) {
        ui.spinBoxYear->setEnabled(true);
        ui.spinBoxYear->setValue(QDate::currentDate().year());
    }
    else {
        ui.spinBoxYear->setEnabled(false);
        ui.spinBoxYear->setValue(year);
    }

	m_maxUserId = 0;
	m_newUser = false;

	if (!noLots)
	{
		ui.labelSize->setHidden(true);
		ui.doubleSpinBoxSize->setHidden(true);
	}

	QSqlDatabase::database().transaction();

	QSqlQuery q;

	q.exec("SELECT user_id, user_name FROM user ORDER BY user_name ASC");
	int index = 0;
	while (q.next()) {
		ui.comboBoxUser->insertItem(index, q.value(1).toString(), q.value(0));
		if (q.value(0).toInt() > m_maxUserId)
			m_maxUserId = q.value(0).toInt();
		index++;
	}

	q.exec("SELECT fild_use_id, fild_use_name FROM fild_use WHERE fild_use_is_actv='true'");
	index = 0;
	while (q.next()) {
		ui.comboBoxFildTyp->insertItem(index, q.value(1).toString(), q.value(0));
		index++;
	}

	q.exec("SELECT ifnull(max(fild_id)+1, 1) FROM fild");
	while (q.next())
		m_fildId = q.value(0).toInt();

	q.exec("SELECT ifnull(max(fild_nr)+1, 1) FROM fild");
	while (q.next())
		ui.spinBoxFildNumber->setValue(q.value(0).toInt());

	qDebug() << "fildId" << m_fildId;

	q.exec("SELECT evnt_prg_id, CASE WHEN length(evnt_prg_abbr) = 0 THEN evnt_prg_name ELSE evnt_prg_abbr END FROM evnt_prg ORDER BY evnt_prg_name");
	while (q.next()) {
	    QListWidgetItem *item = new QListWidgetItem(q.value(1).toString());
	    item->setCheckState(Qt::Unchecked);
	    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	    item->setData(Qt::UserRole, q.value(0));
	    ui.listWidgetEnvSchemas->addItem(item);
	}

    q.exec("SELECT crop_use_id, crop_use_name FROM crop_use ORDER BY crop_use_name");
    while (q.next())
        ui.comboProd->addItem(q.value(1).toString(), q.value(0));

	QSqlDatabase::database().commit();

	ui.lineEditFildName->setText(tr("Field%1").arg(m_fildId));

	ui.comboBoxUser->completer()->setCaseSensitivity(Qt::CaseInsensitive);

}

int FildDialog::userId()
{
    if (ui.comboBoxUser->findText(ui.comboBoxUser->currentText().simplified(), Qt::MatchFixedString) >= 0) {
    	return ui.comboBoxUser->itemData(ui.comboBoxUser->findText(ui.comboBoxUser->currentText(), Qt::MatchFixedString)
    			, Qt::UserRole).toInt();
    }
    else {
    	m_newUser = true;
    	return m_maxUserId+1;
    }

}

int FildDialog::fildId()
{
	return m_fildId;
}

QString FildDialog::userName() const
{
	return ui.comboBoxUser->currentText().simplified();
}

bool FildDialog::newUser() const
{
	return m_newUser;
}

QString FildDialog::fildName() const
{
	return ui.lineEditFildName->text().simplified();
}

int FildDialog::nr() const
{
	return ui.spinBoxFildNumber->value();
}

int FildDialog::fildTyp() const
{
	return 	ui.comboBoxFildTyp->itemData(ui.comboBoxFildTyp->findText(
				ui.comboBoxFildTyp->currentText(), Qt::MatchFixedString), Qt::UserRole).toInt();
}

int FildDialog::year() const
{
	return ui.spinBoxYear->value();
}

double FildDialog::size() const
{
	return ui.doubleSpinBoxSize->value();
}

QList<int> FildDialog::schemas() const
{
    QList<int> schemas;
    for (int row = 0; row < ui.listWidgetEnvSchemas->count(); ++row) {
        if (ui.listWidgetEnvSchemas->item(row)->checkState() == Qt::Checked)
            schemas << ui.listWidgetEnvSchemas->item(row)->data(Qt::UserRole).toInt();
    }

    return schemas;
}

int FildDialog::production() const
{
    if (ui.comboProd->count() > 0)
        return ui.comboProd->itemData(ui.comboProd->currentIndex(), Qt::UserRole).toInt();
    else
        return -1;
}

void FildDialog::accept()
{
    if (fildName().trimmed().isEmpty() || nr() < 0  ||  production() < 0)
        return;

    QDialog::accept();
}

FildDialog::~FildDialog()
{

}
