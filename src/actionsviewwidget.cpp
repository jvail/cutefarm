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

#include <QProgressDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QToolBar>
#include <QDateEdit>
#include <QLabel>
#include <QDebug>

#include "actionsviewwidget.h"
#include "actionsviewmodel.h"

ActionsViewWidget::ActionsViewWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	mDateEditStart = new QDateEdit(this);
	mDateEditEnd = new QDateEdit(this);
	mDateEditStart->setCalendarPopup(true);
	mDateEditEnd->setCalendarPopup(true);
	mDateEditStart->setDate(QDate::currentDate().addDays(-7));
	mDateEditEnd->setDate(QDate::currentDate());
	mDateEditStart->setMinimumHeight(32);
	mDateEditEnd->setMinimumHeight(32);

	QWidget *dateFilterWidget = new QWidget(this);
	QHBoxLayout *l = new QHBoxLayout(dateFilterWidget);
	l->addWidget(mDateEditStart);
	l->addWidget(new QLabel(tr("to", "from date TO date")));
	l->addWidget(mDateEditEnd);
	l->setContentsMargins(0, 4, 9, 4);

	connect(ui.filterApplyAct, SIGNAL(triggered()), this, SLOT(refresh()));

    mFilterBar = new QToolBar(this);
    mFilterBar->setIconSize(QSize(32, 32));
    mFilterBar->addWidget(dateFilterWidget);
    mFilterBar->addSeparator();
    mFilterBar->addAction(ui.filterTillAct);
    mFilterBar->addAction(ui.filterMedwAct);
    mFilterBar->addAction(ui.filterDrilAct);
    mFilterBar->addAction(ui.filterFertAct);
    mFilterBar->addAction(ui.filterPltrAct);
    mFilterBar->addAction(ui.filterHarvAct);
    mFilterBar->addAction(ui.filterAnimGrpAct);
    mFilterBar->addAction(ui.filterSaplAct);
    mFilterBar->addAction(ui.filterMiscAct);
    mFilterBar->addSeparator();
    mFilterBar->addAction(ui.filterApplyAct);

    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
    lay->insertWidget(lay->count(), mFilterBar);
}

ActionsViewWidget::~ActionsViewWidget()
{

}

void ActionsViewWidget::onDatabaseChanged()
{
    ActionsViewModel *model = qobject_cast<ActionsViewModel*>(ui.treeView->model());
    if (model)
        model->clearModel();
}

void ActionsViewWidget::onActionFilterToggled(bool on)
{
    mFilterBar->setVisible(on);
}

void ActionsViewWidget::onExpandAllTriggered()
{
    QModelIndex index = ui.treeView->currentIndex();
    ui.treeView->expandAll();
    if (index.isValid())
        ui.treeView->scrollTo(index, QAbstractItemView::PositionAtCenter);
}

void ActionsViewWidget::onCollapseAllTriggered()
{
    QModelIndex index = ui.treeView->currentIndex();
    while (index.isValid())
        index = index.parent();
    ui.treeView->collapseAll();
    if (index.isValid())
        ui.treeView->scrollTo(index, QAbstractItemView::PositionAtCenter);
}

QString ActionsViewWidget::htmlTable() const
{
    QString table = "<table width=\"100%\" border=\"0\">\n";
    ActionsViewModel *model = qobject_cast<ActionsViewModel*>(ui.treeView->model());
    if (model)
        table.append(model->toHtmlRows());

    table.append("</table>\n");

    return table;
}

void ActionsViewWidget::refresh()
{
    QList<ActionsViewModel::ItemType> types;
    if (ui.filterAnimGrpAct->isChecked())
        types << ActionsViewModel::Type_Anim_Grp;
    if (ui.filterDrilAct->isChecked())
        types << ActionsViewModel::Type_Dril;
    if (ui.filterFertAct->isChecked())
        types << ActionsViewModel::Type_Fert;
    if (ui.filterHarvAct->isChecked())
        types << ActionsViewModel::Type_Harv;
    if (ui.filterMedwAct->isChecked())
        types << ActionsViewModel::Type_Medw;
    if (ui.filterMiscAct->isChecked())
        types << ActionsViewModel::Type_Misc;
    if (ui.filterPltrAct->isChecked())
        types << ActionsViewModel::Type_Pltr;
    if (ui.filterTillAct->isChecked())
        types << ActionsViewModel::Type_Soil;
    if (ui.filterSaplAct->isChecked())
        types << ActionsViewModel::Type_Soil_Test;

    QProgressDialog progress(tr("Loading..."), tr("Abort"), 0, 0, QApplication::topLevelWidgets().first());
    progress.setWindowModality(Qt::WindowModal);

    ActionsViewModel *model = qobject_cast<ActionsViewModel*>(ui.treeView->model());
    if (model) {
        model->setFilter(types, mDateEditStart->date().toString("yyyy-MM-dd"),
                mDateEditEnd->date().toString("yyyy-MM-dd"), &progress);
    }

    for (int row = 0; row < model->rowCount(); ++row)
        ui.treeView->expand(model->index(row, 0));

    ui.treeView->resizeColumnToContents(0);
    ui.treeView->resizeColumnToContents(1);
}

