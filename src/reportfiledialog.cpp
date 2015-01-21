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

#include "reportfiledialog.h"

#include <QHeaderView>
#include <QModelIndexList>
#include <QDesktopServices>
#include <QDebug>

ReportFileDialog::ReportFileDialog(QDirModel *dirModel, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	m_model = dirModel;
	m_selModel = new QItemSelectionModel(m_model, this);
	ui.treeView->setModel(m_model);
	ui.treeView->setRootIndex(m_model->index(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/Reports"));
	ui.treeView->hideColumn(1);
	ui.treeView->hideColumn(2);
	ui.treeView->hideColumn(3);
	ui.treeView->expandAll();
	ui.treeView->header()->hide();

	m_selModel = ui.treeView->selectionModel();

	connect(m_selModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(treeView_selectionChanged(const QItemSelection &, const QItemSelection &)));

}

void ReportFileDialog::treeView_selectionChanged(const QItemSelection &sel, const QItemSelection &desel)
{

	QModelIndexList idxs = m_selModel->selectedIndexes();

	if (idxs.isEmpty())
		return;


	if (QFileInfo(m_model->data(idxs.first(), QDirModel::FilePathRole).toString()).isFile()) {
		ui.lineEdit->setText(QFileInfo(m_model->data(idxs.first(), QDirModel::FileNameRole).toString()).fileName());
	}
	else {
		ui.lineEdit->clear();
	}

	qDebug() << m_model->data(idxs.first(), QDirModel::FilePathRole).toString();
}

QString ReportFileDialog::path() const
{
	QString path = m_model->data(ui.treeView->currentIndex(), QDirModel::FilePathRole).toString();
	if (QFileInfo(path).isFile())
		path = QFileInfo(path).absolutePath();
	return path;
}

QString ReportFileDialog::fileName() const
{
	return ui.lineEdit->text().simplified();
}

ReportFileDialog::~ReportFileDialog()
{

}
