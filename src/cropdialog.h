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

#ifndef CROPDIALOG_H
#define CROPDIALOG_H

#include <QtGui/QDialog>
#include "ui_cropdialog.h"

class CropDialog : public QDialog
{
    Q_OBJECT

public:
    CropDialog(QWidget *parent = 0);
    ~CropDialog();

    QString cropAbbr() const;
    QString cropName() const;
    QString cropNameLat() const;


private:
    Ui::CropDialogClass ui;
};

#endif // CROPDIALOG_H
