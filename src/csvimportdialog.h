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

#ifndef CSVIMPORTDIALOG_H
#define CSVIMPORTDIALOG_H

#include <QtGui/QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include "settingsdialog.h"
#include "ui_csvimportdialog.h"

class CsvImportDialog : public QDialog
{
    Q_OBJECT

public:
    CsvImportDialog(QWidget *parent = 0, const QString &yearOfHarvest = "2008");
    ~CsvImportDialog();

private slots:
	void on_pushButtonLoad_clicked();
	void on_pushButtonOk_clicked();
	void on_pushButtonHelp_clicked();
	void on_filePushButton_clicked();

private:
	void check();
	bool selectionIsUnique();
	QSqlTableModel *qm;
	QList<QString> list;
    Ui::CsvImportDialogClass ui;
};

#endif // CSVIMPORTDIALOG_H
