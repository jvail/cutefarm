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

#ifndef FERTDIALOG_H
#define FERTDIALOG_H

#include <QtGui/QDialog>
#include "ui_fertdialog.h"

class FertDialog : public QDialog
{
    Q_OBJECT

public:
    FertDialog(QWidget *parent = 0);
    ~FertDialog();

	QString fertName() const;
	int fertId() const;
	QString fertTypName() const;
	int fertTypId();
	bool newFertTyp() const;
	QString fertUnit() const;
	double n() const;
	double k2o() const;
	double p2o5() const;
	double s() const;
	double mgo() const;
	double cao() const;
	double humus() const;

private:
	int m_FertId;
	int m_maxFertTypId;
	bool m_newFertTyp;
    Ui::FertDialogClass ui;
};

#endif // FERTDIALOG_H
