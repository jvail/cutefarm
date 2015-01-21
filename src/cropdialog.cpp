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

#include <QDebug>

#include "cropdialog.h"

CropDialog::CropDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

QString CropDialog::cropAbbr() const
{
    return ui.lineEditCropAbbr->text().simplified();
}

QString CropDialog::cropName() const
{
    return ui.lineEditCrop->text().simplified();
}

QString CropDialog::cropNameLat() const
{
    return ui.lineEditCropLat->text().simplified();
}

CropDialog::~CropDialog() {}

