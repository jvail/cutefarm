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

#ifndef LOTXFILDDIALOG_H
#define LOTXFILDDIALOG_H

#include <QtGui/QDialog>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "ui_lotxfilddialog.h"

class LotxFildDialog : public QDialog
{
    Q_OBJECT

public:
    LotxFildDialog(QStandardItemModel *model, double lotSum, const QString &year = QString(), QWidget *parent = 0);
    ~LotxFildDialog();

    bool isAllAlloc();
    QSet<QStandardItem*> changedItems();

private slots:
	void model_itemChanged(QStandardItem *item);
	void lotInfo(const QModelIndex &current, const QModelIndex &previous);

private:
	void setupDelegates();
	double colSum(int col);
	double rowSum(int row);
	double fildSum();
	QStandardItemModel *m_model;
	QItemSelectionModel *m_selectionModel;
	double m_lotSum;
	QSet<QStandardItem*> m_changedItems;
    Ui::LotxFildDialogClass ui;
};

#endif // LOTXFILDDIALOG_H
