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

#ifndef REPORTFILEDIALOG_H
#define REPORTFILEDIALOG_H

#include <QtGui/QDialog>
#include <QDirModel>
#include "ui_reportfiledialog.h"

class ReportFileDialog : public QDialog
{
    Q_OBJECT

public:
    ReportFileDialog(QDirModel *dirModel, QWidget *parent = 0);
    ~ReportFileDialog();

    QString path()const;
    QString fileName() const;

private slots:
	void treeView_selectionChanged(const QItemSelection &sel, const QItemSelection &desel);

private:
	QDirModel *m_model;
	QItemSelectionModel *m_selModel;
    Ui::ReportFileDialogClass ui;
};

#endif // REPORTFILEDIALOG_H
