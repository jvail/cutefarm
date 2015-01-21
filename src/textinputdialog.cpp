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

#include "textinputdialog.h"

TextInputDialog::TextInputDialog(const QString &label, QIcon icon
		, const QString &title, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	ui.label->setText(label);
	setWindowIcon(icon);
	setWindowTitle(title);
}

QString TextInputDialog::text() const
{
	return ui.lineEdit->text().simplified();
}

TextInputDialog::~TextInputDialog()
{

}
