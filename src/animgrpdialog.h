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

#ifndef ANIMGRPDIALOG_H
#define ANIMGRPDIALOG_H

#include <QtGui/QDialog>
#include "ui_animgrpdialog.h"

class AnimGrpDialog : public QDialog
{
    Q_OBJECT

public:
	AnimGrpDialog(QWidget *parent = 0);
    ~AnimGrpDialog();
    
    QString name() const;
    int animSpc() const;
    int count() const;
    double gv() const;

private:
    Ui::AnimGrpDialogClass ui;
};

#endif // ANIMGRPDIALOG_H
