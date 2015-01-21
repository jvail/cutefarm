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

#ifndef LOTDIALOG_H
#define LOTDIALOG_H

#include <QtGui/QDialog>
#include "ui_lotdialog.h"

class LotDialog : public QDialog
{
    Q_OBJECT

public:
    LotDialog(QWidget *parent = 0, int year = -1);
    ~LotDialog();

    QString gark() const;
    QString flur() const;
    int lotNr() const;
    int lotSubNr() const;
    double lotSize() const;
    int year() const;

public slots:
    void accept();

private:
    Ui::LotDialogClass ui;
};

#endif // LOTDIALOG_H

