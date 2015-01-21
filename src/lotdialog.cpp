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
#include <QDate>
#include <QSqlQuery>

#include "lotdialog.h"

LotDialog::LotDialog(QWidget *parent, int year)
    : QDialog(parent)
{
	ui.setupUi(this);

	if (year < 0) {
        ui.spinBoxYear->setEnabled(true);
        ui.spinBoxYear->setValue(QDate::currentDate().year());
    }
    else {
        ui.spinBoxYear->setEnabled(false);
        ui.spinBoxYear->setValue(year);
    }

	QSqlQuery q;
	q.exec("SELECT ifnull(max(lot_nr)+1, 1) FROM lot");
	while (q.next())
		ui.spinBoxLotNr->setValue(q.value(0).toInt());

}

QString LotDialog::gark() const
{
	if (ui.lineEditGark->text().isEmpty())
		return "";
	else
		return ui.lineEditGark->text().simplified();
}

QString LotDialog::flur() const
{
	if (ui.lineEditFlur->text().isEmpty())
		return "";
	else
		return ui.lineEditFlur->text().simplified();
}

int LotDialog::lotNr() const
{
	return ui.spinBoxLotNr->value();
}

int LotDialog::lotSubNr() const
{
	return ui.spinBoxLotSubNr->value();
}

double LotDialog::lotSize() const
{
	return ui.doubleSpinBoxLotSize->value();
}

int LotDialog::year() const
{
	return ui.spinBoxYear->value();
}

void LotDialog::accept()
{
    if (gark().trimmed().isEmpty() || flur().trimmed().isEmpty()  ||  lotSize() <= 0)
        return;

    QDialog::accept();
}

LotDialog::~LotDialog()
{

}
