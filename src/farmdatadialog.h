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

#ifndef FARMDATADIALOG_H
#define FARMDATADIALOG_H

#include <QtGui/QDialog>
#include <QStandardItemModel>
#include <QDataWidgetMapper>

#include "ui_farmdatadialog.h"
#include "sqlquerymodel.h"
#include "sqltablemodel.h"
#include "sqlrelationaltablemodel.h"
#include "settingsdialog.h"

class StandardItemModel : public QStandardItemModel
{
public:
	StandardItemModel(int rows, int columns, QObject *parent = 0)
	: QStandardItemModel(rows, columns, parent)
	{

	}

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
	{
		if (role == Qt::ForegroundRole) {
			if (QStandardItemModel::data(index, Qt::DisplayRole).toDouble() == 0)
				return Qt::red;
			else
				return Qt::black;
		}
		else
			return QStandardItemModel::data(index, role);
	}
};

class FarmDataDialog : public QDialog
{
    Q_OBJECT

public:
    FarmDataDialog(QWidget *parent = 0);
    ~FarmDataDialog();

    static QString farmAddress();
    static QString farmName();

signals:
    void statusMsg(const QString &msg, int timeout);

private slots:
	void changeCurrentIndex(bool checked);

	void on_actionAddWork_triggered();
	void on_actionAddAnimGrp_triggered();
	void on_actionAddOprt_triggered();
	void on_actionAddProd_triggered();
	void on_actionAddFild_triggered();
	void on_actionAddLot_triggered();
	void on_actionLotxFild_triggered();
	void on_actionFildUnion_triggered();
	void on_actionFildSep_triggered();
	void on_actionAddNewHarvYear_triggered();
	void on_actionAddEnvSchema_triggered();

	void onYearComboCurrentChanged(const QString &text);
	void on_buttonBox_helpRequested();

	void on_toolButtonIsActive_clicked();
	void on_toolButtonRemove_clicked();

	void buttonClose();
	void buttonSave();
	void buttonReset();

private:
	void keyFigures();
	void setupModels();
	bool pendingTransaction();
	bool removeMsg(const QString &data);
	void identMsg(const QString &name);
	bool isWhitespace(const QString &text);

	bool m_noLots;

	//Models
	SqlTableModel *farmModel;
	QDataWidgetMapper *farmMapper;

	SqlTableModel *workModel;

	SqlTableModel *oprtModel;

	SqlRelationalTableModel *animGrpModel;

	SqlQueryModel *fildViewModel;
	SqlRelationalTableModel *fildModel;
	SqlRelationalTableModel *evntPrgModel;
	SqlTableModel *lotModel;
	SqlTableModel *lotXfildModel;
	QComboBox *yearComboBox;

	QSqlQueryModel *yearModel;
	QStandardItemModel *prodFildModel;
	SqlRelationalTableModel *prodModel;

	QMenu *workMenu;
	QMenu *oprtMenu;
	QMenu *animMenu;
	QMenu *fildMenu;

    Ui::FarmDataDialogClass ui;
};

#endif // FARMDATADIALOG_H
