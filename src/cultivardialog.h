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

#ifndef CULTIVARDIALOG_H
#define CULTIVARDIALOG_H

#include <QtGui/QDialog>
#include "ui_cultivardialog.h"

class CultivarDialog : public QDialog
{
    Q_OBJECT

public:
    CultivarDialog(QWidget *parent = 0);
    ~CultivarDialog();

    int cropUseId() const;
    QString cultivarName() const;
    QString seedUnit() const;

private:
    Ui::CultivarDialogClass ui;
};

#endif // CULTIVARDIALOG_H
