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

#include "animgrpdialog.h"

AnimGrpDialog::AnimGrpDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	
	qDebug() << "transaction" << QSqlDatabase::database().transaction();
	QSqlQuery q("SELECT anim_spc_id, anim_spc_name FROM anim_spc ORDER BY anim_spc_name ASC");
	int index = 0;
	while (q.next()) {
		ui.comboBoxAnimSpc->insertItem(index, q.value(1).toString(), q.value(0));
		index++;
	}
	qDebug() << "commit" << QSqlDatabase::database().commit();
}

QString AnimGrpDialog::name() const
{
	return ui.lineEditName->text().simplified();
}

int AnimGrpDialog::animSpc() const
{
	return 	ui.comboBoxAnimSpc->itemData(ui.comboBoxAnimSpc->findText(
				ui.comboBoxAnimSpc->currentText(), Qt::MatchFixedString), Qt::UserRole).toInt();
}

int AnimGrpDialog::count() const
{
	return ui.spinBoxCount->value();
}

double AnimGrpDialog::gv() const
{
	return ui.doubleSpinBoxGv->value();
}

AnimGrpDialog::~AnimGrpDialog()
{

}
