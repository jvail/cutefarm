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

#ifndef BASICDATADIALOG_H
#define BASICDATADIALOG_H

#include <QtGui/QDialog>
#include <QMenu>
#include <QDebug>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QHeaderView>

#include "ui_basicdatadialog.h"
#include "sqltablemodel.h"
#include "sqlrelationaltablemodel.h"
#include "settingsdialog.h"

class BasicDataDialog : public QDialog
{
    Q_OBJECT

public:
    BasicDataDialog(QWidget *parent = 0);
    ~BasicDataDialog();

signals:
    void statusMsg(const QString &msg, int timeout);

private slots:
    void onSettingsChanged();
	void changeCurrentIndex(bool checked);
	void on_actionAddUse_triggered();
	void on_actionAddCrop_triggered();
	void on_actionAddCropUse_triggered();
	void on_actionAddCultivar_triggered();
	void on_actionAddPltr_triggered();
	void on_actionEditPltr_triggered();
	void on_actionAddPltrTyp_triggered();
	void on_actionAddPltrSub_triggered();
	void on_actionAddFert_triggered();
	void on_actionAddAnim_triggered();
	void on_actionAddEvntPrg_triggered();

	void on_buttonBox_helpRequested();

	void on_toolButtonRemove_clicked();
	void on_toolButtonIsActive_clicked();

	void buttonClose();
	void buttonSave();
	void buttonReset();

private:
	void setupModels();
	QTableView* currentTable();
	bool pendingTransaction();
	QString submit(QAbstractItemModel *model);
	bool removeMsg(const QString &data);
	void identMsg(const QString &name);

	SqlTableModel *useModel;

	SqlTableModel *cropModel;
	SqlRelationalTableModel *cropUseModel;
	SqlRelationalTableModel *cultivarModel;

	SqlTableModel *pltrViewModel;
	QSortFilterProxyModel *pltrViewProxy;
	SqlTableModel *pltrModel;
	SqlTableModel *pltrTypModel;
	SqlTableModel *pltrSubModel;

	SqlRelationalTableModel *fertModel;
	QDataWidgetMapper *fertMapper;
	SqlTableModel *fertTypModel;

	SqlTableModel *animSpcModel;

	SqlTableModel *envtPrgModel;
	QDataWidgetMapper *envtPrgMapper;

	QMenu *useMenu;
	QMenu *cropMenu;
	QMenu *pltrMenu;
	QMenu *fertMenu;
	QMenu *animMenu;
	QMenu *ecoMenu;

    Ui::BasicDataDialogClass ui;
};

#endif // BASICDATADIALOG_H
