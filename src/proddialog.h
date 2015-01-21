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

#ifndef PRODDIALOG_H
#define PRODDIALOG_H

#include <QtGui/QDialog>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include "ui_proddialog.h"

class ProdDialog : public QDialog
{
    Q_OBJECT

public:
    ProdDialog(QWidget *parent = 0);
    ~ProdDialog();

    QStandardItemModel* prodFildModel();
    int cropUseId();


private slots:
	void comboBoxProdYear_currentIndexChanged(const QString &year);

private:
	QSqlQueryModel *yearModel;
	QStandardItemModel *m_prodFildModel;
	QSqlQueryModel *cropUseModel;
    Ui::ProdDialogClass ui;
};

#endif // PRODDIALOG_H
