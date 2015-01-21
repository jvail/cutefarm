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

#include "cropusedialog.h"
#include "settingsdialog.h"

CropUseDialog::CropUseDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	ui.label_10->setText(tr("Withdrawal %1/unit (use positve figures e.g. in case of N fixation)")
	        .arg(SettingsDialog::nutUnit()));

	ui.dsbCaO->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbHumus->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbK2O->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbMgO->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbN->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbP2O5->setSuffix(" " + SettingsDialog::nutUnit());
	ui.dsbS->setSuffix(" " + SettingsDialog::nutUnit());


	qDebug() << "transaction" << QSqlDatabase::database().transaction();
	QSqlQuery q("SELECT crop_id, crop_name FROM crop ORDER BY crop_name ASC");
	int index = 0;
	while (q.next()) {
		ui.comboBoxCrop->insertItem(index, q.value(1).toString(), q.value(0));
		index++;
	}
	qDebug() << "commit" << QSqlDatabase::database().commit();
}

int CropUseDialog::cropId() const
{
	if (ui.comboBoxCrop->currentText().isEmpty() || !ui.checkBox->isChecked())
		return -1;
	return 	ui.comboBoxCrop->itemData(ui.comboBoxCrop->findText(
				ui.comboBoxCrop->currentText(), Qt::MatchFixedString), Qt::UserRole).toInt();
}

QString CropUseDialog::cropUseName() const
{
	return ui.lineEditCropUse->text().simplified();
}

QString CropUseDialog::unit() const
{
    return ui.comboUnit->currentText().trimmed();
}

CropUseDialog::~CropUseDialog()
{

}


double CropUseDialog::n() const
{
    return ui.dsbN->value();
}

double CropUseDialog::k2o() const
{
    return ui.dsbK2O->value();
}

double CropUseDialog::p2o5() const
{
    return ui.dsbP2O5->value();
}

double CropUseDialog::s() const
{
    return ui.dsbS->value();
}

double CropUseDialog::mgo() const
{
    return ui.dsbMgO->value();
}

double CropUseDialog::cao() const
{
    return ui.dsbCaO->value();
}

double CropUseDialog::humus() const
{
    return ui.dsbHumus->value();
}

