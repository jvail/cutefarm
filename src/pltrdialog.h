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

#ifndef PLTRDIALOG_H
#define PLTRDIALOG_H

#include <QtGui/QDialog>
#include <QStandardItemModel>
#include "ui_pltrdialog.h"

class PltrDialog : public QDialog
{
    Q_OBJECT

public:
	PltrDialog(QWidget *parent, const QString &name = QString(),
	        const QString &unit = QString(), int pltrId = -1);
    ~PltrDialog();
    
    int pltrId() const;
    QString pltrName() const;
    QString pltrUnit() const;
    QList<int> pltrSubIds();
    QList<int> pltrTypIds();
    

//private slots:
//	void save();
    
private:
//	void fehlerMsg(const QString &msg);
	QStandardItemModel *pltrSubModel;
	QStandardItemModel *pltrTypModel;
	int m_pltrId;
    Ui::PltrDialogClass ui;
};

#endif // PLTRDIALOG_H
