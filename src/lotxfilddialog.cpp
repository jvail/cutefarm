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

#include <QDebug>
#include <QHeaderView>
#include <QSqlQuery>

#include "lotxfilddialog.h"
#include "spinboxdelegate.h"

LotxFildDialog::LotxFildDialog(QStandardItemModel *model, double lotSum, const QString &year, QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint)
{
	ui.setupUi(this);
	if (!year.isEmpty())
		setWindowTitle(windowTitle().append(' ').append(tr("Harvest Year:%1").arg(year)));
	m_model = model;
	m_model->setParent(this);
	m_lotSum = lotSum;
	QLocale locale;
	ui.lineEditLotSum->setText(locale.toString(m_lotSum));
	ui.tableView->setModel(model);
	ui.tableView->setItemDelegate(new SpinBoxDelegate(ui.tableView, -0.0001, 999.9999, true, true, QString(), 4, true));
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->resizeRowsToContents();
	setupDelegates();
	ui.lineEditFildSum->setText(locale.toString(fildSum()));

	m_selectionModel = ui.tableView->selectionModel();
	m_selectionModel->select(m_model->index(0, 0), QItemSelectionModel::Select);

	connect(m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(model_itemChanged(QStandardItem*)));
	connect(m_selectionModel, SIGNAL(currentColumnChanged(const QModelIndex&, const QModelIndex&)),
			this, SLOT(lotInfo(const QModelIndex&, const QModelIndex&)));
}

void LotxFildDialog::lotInfo(const QModelIndex &current, const QModelIndex &previous)
{
	int lotId = m_model->horizontalHeaderItem(current.column())->data(Qt::UserRole).toInt();
	QSqlQuery q;
	q.prepare(" SELECT gmrk, flur "
			  " FROM lot "
			  " WHERE lot_id=:lotId ");
	q.bindValue(":lotId", lotId);
	q.exec();

	while (q.next()) {
		ui.lineEditGark->setText(q.value(0).toString());
		ui.lineEditLot->setText(q.value(1).toString());
	}
}

bool LotxFildDialog::isAllAlloc()
{
	if (m_lotSum == fildSum())
		return true;

	return false;
}

QSet<QStandardItem*> LotxFildDialog::changedItems()
{
	return m_changedItems;
}

void LotxFildDialog::setupDelegates()
{
	for (int i = 0; i < m_model->rowCount(); i++) {
		for (int j = 0; j < m_model->columnCount(); j++) {

			double left = m_model->horizontalHeaderItem(j)->data(Qt::UserRole+1).toDouble() - colSum(j);
			m_model->setData(m_model->index(i, j), m_model->data(m_model->index(i, j), Qt::DisplayRole).toDouble() + left, Qt::UserRole+1);
		}
	}
}

double LotxFildDialog::colSum(int col)
{
	double sum = 0;
	for (int i = 0; i < m_model->rowCount(); i++) {
		sum += m_model->data(m_model->index(i, col), Qt::DisplayRole).toDouble();
	}
//	qDebug() << "colSum" << sum;
	return sum;
}

double LotxFildDialog::rowSum(int row)
{
	double sum = 0;
	for (int i = 0; i < m_model->columnCount(); i++) {
		sum += m_model->data(m_model->index(row, i), Qt::DisplayRole).toDouble();
	}
	return sum;
}

double LotxFildDialog::fildSum()
{
	double sum = 0;
	for (int i = 0; i < m_model->columnCount(); i++) {
		sum += colSum(i);
	}
	return sum;
}

void LotxFildDialog::model_itemChanged(QStandardItem *item)
{
//	qDebug() << item->data(Qt::DisplayRole) << item->data(Qt::UserRole);
	double left = 0;
	m_model->disconnect(this);
	QLocale locale;

	qDebug() << "Qt::UserRole+1" << m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+1).toDouble();
	qDebug() << "colSum" << colSum(item->column());

	for (int i = 0; i < m_model->rowCount(); i++) {
		left = m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+1).toDouble() - colSum(item->column());
		m_model->setData(m_model->index(i, item->column()), m_model->data(m_model->index(i, item->column()), Qt::DisplayRole).toDouble() + left, Qt::UserRole+1);
	}

//	qDebug() << "left" << left;


	if (left >= 0.0001) {
		m_model->horizontalHeaderItem(item->column())->setData(
				m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+2).toString() + "/"
				+ m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+3).toString() + " (" +
				locale.toString(m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+1).toDouble(), 'g', 4) + ") -" +
				locale.toString(left, 'g', 4), Qt::DisplayRole);
	}
	else
		m_model->horizontalHeaderItem(item->column())->setData(
				m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+2).toString()  + "/"
				+ m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+3).toString() + " (" +
				locale.toString(m_model->horizontalHeaderItem(item->column())->data(Qt::UserRole+1).toDouble()) + ")", Qt::DisplayRole);

	m_model->verticalHeaderItem(item->row())->setData(rowSum(item->row()), Qt::UserRole+1);
	m_model->verticalHeaderItem(item->row())->setData(
			m_model->verticalHeaderItem(item->row())->data(Qt::UserRole+2).toString() + " (" +
							locale.toString(m_model->verticalHeaderItem(item->row())->data(Qt::UserRole+1).toDouble()) + ")", Qt::DisplayRole);

	connect(m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(model_itemChanged(QStandardItem*)));

	ui.lineEditFildSum->setText(locale.toString(fildSum()));

	m_changedItems << item;

//	qDebug() << m_changedItems;

//	ui.tableView->verticalHeader()->setResizeMode(QHeaderView::Interactive);

//	ui.tableView->horizontalHeader()->setMinimumWidth(100);
//	ui.tableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
}

LotxFildDialog::~LotxFildDialog()
{

}
