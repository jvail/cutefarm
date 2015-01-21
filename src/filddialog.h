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

#ifndef FILDDIALOG_H
#define FILDDIALOG_H

#include <QtGui/QDialog>
#include "ui_filddialog.h"

class FildDialog : public QDialog
{
    Q_OBJECT

public:
    FildDialog(bool noLots = false, QWidget *parent = 0, int year = -1);
    ~FildDialog();

    int userId();
    int fildId();
    QString userName() const;
    bool newUser() const;
    QString fildName() const;
    int nr() const;
    int fildTyp() const;
    int year() const;
    double size() const;
    QList<int> schemas() const;
    int production() const;

public slots:
    void accept();

private:
	int m_maxUserId;
	bool m_newUser;
	int m_fildId;
    Ui::FildDialogClass ui;
};

#endif // FILDDIALOG_H
