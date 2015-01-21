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

#include "workdialog.h"

WorkDialog::WorkDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

QString WorkDialog::name() const
{
	return ui.lineEditName->text().simplified();
}

double WorkDialog::ak() const
{
	return ui.doubleSpinBox->value();
}

WorkDialog::~WorkDialog()
{

}
