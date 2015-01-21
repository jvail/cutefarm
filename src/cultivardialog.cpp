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

#include "cultivardialog.h"

CultivarDialog::CultivarDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	qDebug() << "transaction" << QSqlDatabase::database().transaction();
	QSqlQuery q("SELECT crop_use_id, crop_use_name "
			" FROM crop_use "
			" WHERE crop_id IS NOT NULL "
			" AND crop_use_is_actv='true' "
			" ORDER BY crop_use_name ASC");
	int index = 0;
	while (q.next()) {
		ui.comboBoxCropUse->insertItem(index, q.value(1).toString(), q.value(0));
		index++;
	}
	qDebug() << "commit" << QSqlDatabase::database().commit();
}

int CultivarDialog::cropUseId() const
{
	if (ui.comboBoxCropUse->currentText().isEmpty())
		return -1;
	return 	ui.comboBoxCropUse->itemData(ui.comboBoxCropUse->findText(
				ui.comboBoxCropUse->currentText(), Qt::MatchFixedString), Qt::UserRole).toInt();
}

QString CultivarDialog::cultivarName() const
{
	return ui.lineEditCultivar->text().simplified();
}

QString CultivarDialog::seedUnit() const
{
    return ui.comboUnit->currentText().trimmed();
}

CultivarDialog::~CultivarDialog()
{

}
