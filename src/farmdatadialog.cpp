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
#include <QDebug>
#include <QMenu>
#include <QSqlRelationalDelegate>
#include <QSortFilterProxyModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QHeaderView>
#include <QFileDialog>
#include <QDate>
#include <QMessageBox>
#include <QInputDialog>

#include "farmdatadialog.h"
#include "workdialog.h"
#include "animgrpdialog.h"
#include "textinputdialog.h"
#include "proddialog.h"
#include "lotxfilddialog.h"
#include "newharvestyeardialog.h"
#include "filddialog.h"
#include "lotdialog.h"
#include "numberinputdialog.h"
#include "spinboxdelegate.h"
#include "sqlrelationaldelegate.h"

FarmDataDialog::FarmDataDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint)
{
	ui.setupUi(this);

	ui.label_2->setText(tr("Worker Units/%1:").arg(SettingsDialog::areaUnit()));
	ui.label_4->setText(tr("Animal Units/%1:").arg(SettingsDialog::areaUnit()));

	ui.toolButtonIsActive->setVisible(false);

	QVariant w = height() * 1.33;
	resize(w.toInt(), height());

	m_noLots = SettingsDialog::lotsDisabled();

	workMenu = new QMenu(this);
	workMenu->addAction(ui.actionAddWork);

	oprtMenu = new QMenu(this);
	oprtMenu->addAction(ui.actionAddOprt);

	animMenu = new QMenu(this);
	animMenu->addAction(ui.actionAddAnimGrp);

	fildMenu = new QMenu(this);
	fildMenu->addAction(ui.actionAddFild);
	if (!m_noLots)
		fildMenu->addAction(ui.actionAddLot);
	if (!m_noLots)
		fildMenu->addAction(ui.actionLotxFild);
	fildMenu->addSeparator();
	fildMenu->addAction(ui.actionAddProd);
	fildMenu->addAction(ui.actionAddEnvSchema);
    fildMenu->addSeparator();
    fildMenu->addAction(ui.actionAddNewHarvYear);

	QSqlDatabase::database().transaction(); //faster data loading
	keyFigures();
	setupModels();
	QSqlDatabase::database().commit();

	ui.pushButtonFarm->setChecked(true);
}

void FarmDataDialog::keyFigures()
{
	double area = 0;
	QString year;

	QSqlQuery q;
	q.exec("SELECT year_of_harvest, total FROM (SELECT year_of_harvest, total(lot_x_fild_size) AS total "
			" FROM fild, lot_x_fild"
			" WHERE fild.fild_id=lot_x_fild.fild_id"
			" GROUP BY year_of_harvest"
			" ORDER BY year_of_harvest DESC) LIMIT 1");
	while (q.next())
	{
		ui.lineEditLn->setText(q.value(1).toString() + " (" + q.value(0).toString() + ")");
		area = q.value(1).toDouble();
		year = q.value(0).toString();
	}

	q.exec("SELECT total(work_ak) "
			" FROM work"
			" WHERE work_is_actv='true'");
	while (q.next())
	{
		ui.lineEditAkh->setText(QString::number(q.value(0).toDouble()/area, 'g', 2) + " (" + year + ")");

	}

	q.exec("SELECT total(anim_unit*anim_grp_cont) "
			" FROM anim_grp"
			" WHERE anim_grp_is_actv='true'");
	while (q.next())
	{
		ui.lineEditGv->setText(QString::number(q.value(0).toDouble()/area, 'g', 2) + " (" + year + ")");

	}
}

void FarmDataDialog::setupModels()
{
	bool alternatingRowColors = SettingsDialog::alternatingRowColors();
	QHeaderView::ResizeMode resizeMode = SettingsDialog::resizeMode();
	bool showGrid = SettingsDialog::showGrid();

	if (m_noLots)
		ui.tabWidgetFieldLot->removeTab(ui.tabWidgetFieldLot->indexOf(ui.tabLot));

	foreach (QTableView *view, findChildren<QTableView*>()) {
        view->horizontalHeader()->setResizeMode(resizeMode);
        view->horizontalHeader()->setHighlightSections(false);
        view->horizontalHeader()->setStretchLastSection(true);
        view->verticalHeader()->setHighlightSections(false);
        view->setAlternatingRowColors(alternatingRowColors);
        view->setShowGrid(showGrid);
	}

	farmModel = new SqlTableModel(this);
	farmModel->setTable("farm");
	farmModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	farmModel->select();
	if(farmModel->rowCount(QModelIndex()) == 0){
		farmModel->insertRow(-1, QModelIndex());
		QSqlRecord rec = farmModel->record();
		rec.setValue(0, tr("My Name"));
		rec.setValue(1, tr("My Street"));
		rec.setValue(2, tr("My Town"));
		rec.setValue(3, tr("My Farm Ident. Number"));
		farmModel->insertRecord(-1, rec);
	}

	farmMapper = new QDataWidgetMapper(this);
	farmMapper->setModel(farmModel);
	farmMapper->addMapping(ui.lineEditFarmName, 0);
	farmMapper->addMapping(ui.lineEditFarmStr, 1);
	farmMapper->addMapping(ui.lineEditFarmCityCode, 2);
	farmMapper->addMapping(ui.lineEditFarmNumb, 3);
	farmMapper->toFirst();
	farmMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

	connect(farmModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(farmModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));



	workModel = new SqlTableModel(this);
	workModel->setTable("work");
	workModel->setSort(3, Qt::DescendingOrder);
	workModel->setBoolColumn(3);
	workModel->addTextColumn(1);
	workModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	workModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	workModel->setHeaderData(2, Qt::Horizontal, tr("Worker Units", "unit to compare worker's productivity"));
	workModel->select();
	ui.tableViewWorker->setModel(workModel);
	ui.tableViewWorker->setItemDelegateForColumn(2, new SpinBoxDelegate(ui.tableViewWorker, 0.1, 2, true));
	ui.tableViewWorker->hideColumn(0);
	ui.tableViewWorker->hideColumn(3);

	connect(workModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
    		ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
    connect(workModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
    		ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	oprtModel = new SqlTableModel(this);
	oprtModel->setTable("oprt");
	oprtModel->setSort(2, Qt::DescendingOrder);
	oprtModel->setBoolColumn(2);
	oprtModel->addTextColumn(1);
	oprtModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	oprtModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	oprtModel->select();
	ui.tableViewOprt->setModel(oprtModel);
	ui.tableViewOprt->hideColumn(0);
	ui.tableViewOprt->hideColumn(2);

	connect(oprtModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(oprtModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));



    animGrpModel = new SqlRelationalTableModel(this);
    animGrpModel->setTable("anim_grp");
    animGrpModel->setRelation(2, QSqlRelation("anim_spc", "anim_spc_id", "anim_spc_name"));
    animGrpModel->setBoolColumn(5);
    animGrpModel->addTextColumn(1);
    animGrpModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    animGrpModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    animGrpModel->setHeaderData(2, Qt::Horizontal, tr("Animal Species"));
    animGrpModel->setHeaderData(3, Qt::Horizontal, tr("Number"));
    animGrpModel->setHeaderData(4, Qt::Horizontal, tr("Unit/Animal"));
    animGrpModel->setSort(5, Qt::DescendingOrder);
    animGrpModel->select();
	ui.tableViewAnim->setModel(animGrpModel);
	ui.tableViewAnim->hideColumn(0);
	ui.tableViewAnim->hideColumn(5);
	ui.tableViewAnim->setItemDelegateForColumn(2, new SqlRelationalDelegate(ui.tableViewAnim));
	ui.tableViewAnim->setItemDelegateForColumn(3, new SpinBoxDelegate(ui.tableViewAnim, 1, 10000));
	ui.tableViewAnim->setItemDelegateForColumn(4, new SpinBoxDelegate(ui.tableViewAnim, 0.01, 2.00, true));

	connect(animGrpModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(animGrpModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


    yearModel = new QSqlQueryModel(this);
    yearModel->setQuery("SELECT DISTINCT year_of_harvest FROM lot ORDER BY year_of_harvest DESC");
    while(yearModel->canFetchMore())
    	yearModel->fetchMore();
    yearComboBox = new QComboBox(this);
    ui.tabWidgetFieldLot->setCornerWidget(yearComboBox, Qt::TopRightCorner);
    yearComboBox->setModel(yearModel);

    connect(yearComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onYearComboCurrentChanged(const QString&)));

    fildViewModel = new SqlQueryModel(this);
    QSortFilterProxyModel *fildViewProxy = new QSortFilterProxyModel(this);
    fildViewModel->setQuery(" SELECT * FROM v_fild "
    		                " WHERE year=" + yearComboBox->currentText() +
    		                " ORDER BY fild_name ASC ");
    fildViewModel->setHeaderData(1, Qt::Horizontal, tr("Farm"));
    fildViewModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    fildViewModel->setHeaderData(3, Qt::Horizontal, tr("Lots"));
    fildViewModel->setHeaderData(4, Qt::Horizontal, tr("Effective Farmland", "square units"));
    fildViewModel->setHeaderData(5, Qt::Horizontal, tr("Harvest Year"));
    fildViewProxy->setSourceModel(fildViewModel);
    ui.tableOverView->setModel(fildViewProxy);
    ui.tableOverView->hideColumn(0);
    ui.tableOverView->setColumnHidden(3, m_noLots);


    fildModel = new SqlRelationalTableModel(this);
    fildModel->setTable("fild");
    fildModel->setRelation(1, QSqlRelation("user", "user_id", "user_name"));
    fildModel->setRelation(4, QSqlRelation("fild_use", "fild_use_id", "fild_use_name"));
    fildModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    fildModel->setHeaderData(1, Qt::Horizontal, tr("Farm"));
    fildModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    fildModel->setHeaderData(3, Qt::Horizontal, tr("Field Number", "a unique number"));
    fildModel->setHeaderData(4, Qt::Horizontal, tr("Field Use", "e.g. arable or permanent grassland"));
    fildModel->setHeaderData(5, Qt::Horizontal, tr("Year of Harvest"));
    fildModel->setNoEditColumn(3);
    fildModel->setNoEditColumn(5);
    fildModel->addTextColumn(2);
    if (!yearComboBox->currentText().isEmpty())
    	fildModel->setFilter("year_of_harvest=" + yearComboBox->currentText());
    fildModel->select();
    ui.tableViewField->setModel(fildModel);
    ui.tableViewField->hideColumn(0);
    ui.tableViewField->setItemDelegateForColumn(1, new QSqlRelationalDelegate(ui.tableViewField));
    ui.tableViewField->setItemDelegateForColumn(4, new QSqlRelationalDelegate(ui.tableViewField));
    ui.tableViewField->setItemDelegateForColumn(3, new SpinBoxDelegate(ui.tableViewField, 0, 9999999));

	connect(fildModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(fildModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


    lotModel = new SqlTableModel(this);
    lotModel->setTable("lot");
    lotModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    lotModel->setHeaderData(1, Qt::Horizontal, tr("District", "the German term is 'Gemarkung'"));
    lotModel->setHeaderData(2, Qt::Horizontal, tr("Block", "the German term is 'Flur'"));
    lotModel->setHeaderData(3, Qt::Horizontal, tr("Lot Number", "a unique no"));
    lotModel->setHeaderData(4, Qt::Horizontal, tr("Lot Sub Number"));
    lotModel->setHeaderData(5, Qt::Horizontal, tr("Effective Farmland", "square units"));
    lotModel->setHeaderData(6, Qt::Horizontal, tr("Year of Harvest"));
    lotModel->setHeaderData(7, Qt::Horizontal, tr("Soil Type"));
    lotModel->setNoEditColumn(1);
    lotModel->setNoEditColumn(2);
    lotModel->setNoEditColumn(3);
    lotModel->setNoEditColumn(4);
    lotModel->setNoEditColumn(6);
    lotModel->setBoolColumn(8);
    lotModel->addTextColumn(1);
    lotModel->addTextColumn(2);
    lotModel->setFilter("year_of_harvest=" + yearComboBox->currentText());
    lotModel->select();
    ui.tableViewLot->setModel(lotModel);
    ui.tableViewLot->hideColumn(0);
    ui.tableViewLot->hideColumn(7);
    ui.tableViewLot->hideColumn(8);
    ui.tableViewLot->setItemDelegateForColumn(3, new SpinBoxDelegate(ui.tableViewLot, 0, 9999999));
    ui.tableViewLot->setItemDelegateForColumn(4, new SpinBoxDelegate(ui.tableViewLot, 0, 9999999));
    ui.tableViewLot->setItemDelegateForColumn(5, new SpinBoxDelegate(ui.tableViewLot, 0, 9999999, true, false, SettingsDialog::areaUnit(), 4));

	connect(lotModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(lotModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	prodModel = new SqlRelationalTableModel(this);
	prodModel->setTable("prod");
	prodModel->setRelation(1, QSqlRelation("fild", "fild_id", "fild_name"));
	prodModel->setRelation(2, QSqlRelation("crop_use", "crop_use_id", "crop_use_name"));
	prodModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	prodModel->setHeaderData(1, Qt::Horizontal, tr("Field"));
	prodModel->setHeaderData(2, Qt::Horizontal, tr("Crop Use"));
	prodModel->setNoEditColumn(1);
	prodModel->setSort(1, Qt::AscendingOrder);
	prodModel->setFilter("prod.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + yearComboBox->currentText() + ")");
	prodModel->relationModel(2)->setFilter("crop_use_is_actv='true' OR crop_use_id IN (SELECT DISTINCT crop_use_id FROM prod)");
	prodModel->select();
	ui.tableViewProd->setModel(prodModel);
	ui.tableViewProd->hideColumn(0);
	ui.tableViewProd->setItemDelegateForColumn(2, new QSqlRelationalDelegate(ui.tableViewProd));

	connect(prodModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(prodModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	evntPrgModel = new SqlRelationalTableModel(this);
	evntPrgModel->setTable("fild_x_evnt_prg");
	evntPrgModel->setRelation(1, QSqlRelation("fild", "fild_id", "fild_name"));
	evntPrgModel->setRelation(2, QSqlRelation("evnt_prg", "evnt_prg_id", "evnt_prg_name"));
	evntPrgModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	evntPrgModel->setHeaderData(1, Qt::Horizontal, tr("Field"));
	evntPrgModel->setHeaderData(2, Qt::Horizontal, tr("Environmental Program"));
	evntPrgModel->setHeaderData(3, Qt::Horizontal, tr("Program Expires ..."));
	evntPrgModel->setNoEditColumn(0);
	evntPrgModel->setNoEditColumn(1);
	evntPrgModel->setSort(1, Qt::AscendingOrder);
	evntPrgModel->setFilter("fild_x_evnt_prg.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + yearComboBox->currentText() + ")");
	evntPrgModel->select();
    ui.tableViewEnvSchemas->setModel(evntPrgModel);
    ui.tableViewEnvSchemas->hideColumn(0);
    ui.tableViewEnvSchemas->setItemDelegateForColumn(1, new QSqlRelationalDelegate(ui.tableViewEnvSchemas));
    ui.tableViewEnvSchemas->setItemDelegateForColumn(2, new QSqlRelationalDelegate(ui.tableViewEnvSchemas));
    ui.tableViewEnvSchemas->setItemDelegateForColumn(3, new SpinBoxDelegate(ui.tableViewEnvSchemas, 2000, 9999));

    connect(evntPrgModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
            ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
    connect(evntPrgModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
            ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

    connect(ui.buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(buttonClose()));
    connect(ui.buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()),
            this, SLOT(buttonSave()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()),
            this, SLOT(buttonReset()));

    ui.buttonBox->button(QDialogButtonBox::Save)->hide();
	ui.buttonBox->button(QDialogButtonBox::Save)->setDefault(true);
    ui.buttonBox->button(QDialogButtonBox::Reset)->hide();

}

void FarmDataDialog::changeCurrentIndex(bool checked)
{
	if (!checked)
		return;

	if (ui.pushButtonFarm->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(0);
		ui.toolButtonAdd->setDisabled(true);
		ui.toolButtonRemove->setDisabled(true);
		ui.toolButtonIsActive->setDisabled(true);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/home.png"));
//		ui.toolButtonAdd->setMenu(useMenu);
	}
	if (ui.pushButtonWork->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(1);
		ui.toolButtonAdd->setEnabled(true);
		ui.toolButtonRemove->setEnabled(true);
//		ui.toolButtonIsActive->setEnabled(true);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/worker.png"));
		ui.toolButtonAdd->setMenu(workMenu);
	}
	if (ui.pushButtonFild->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(2);
		ui.toolButtonAdd->setEnabled(true);
		ui.toolButtonRemove->setEnabled(true);
//		ui.toolButtonIsActive->setEnabled(false);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/field.png"));
		ui.toolButtonAdd->setMenu(fildMenu);
	}
	if (ui.pushButtonAnimGrp->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(0, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(3);
		ui.toolButtonAdd->setEnabled(true);
		ui.toolButtonRemove->setEnabled(true);
//		ui.toolButtonIsActive->setEnabled(true);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/cow.png"));
		ui.toolButtonAdd->setMenu(animMenu);

	}
	if (ui.pushButtonOprt->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(4);
		ui.toolButtonAdd->setEnabled(true);
		ui.toolButtonRemove->setEnabled(true);
//		ui.toolButtonIsActive->setEnabled(true);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/action.png"));
		ui.toolButtonAdd->setMenu(oprtMenu);
	}
}

void FarmDataDialog::on_actionAddWork_triggered()
{
	WorkDialog workDialog(this);
	start:
    if (workDialog.exec() != QDialog::Accepted)
         return;

    QString workName = workDialog.name();
    double ak = workDialog.ak();

    qDebug() << workName << ak;

    if (workName.isEmpty() || ak <= 0)
    	return;

	QModelIndex idx = workModel->index(0,1);
	if (!workModel->match(idx, Qt::DisplayRole, workName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(workName);
		goto start;
		}

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	QSqlRecord rec = workModel->record();
	rec.setNull(0);
	rec.setValue(1, workName);
	rec.setValue(2, ak);
	rec.setValue(3, QVariant(true));
	if (workModel->insertRecord(-1, rec)) {
		if (!workModel->submitAll()) {
			emit statusMsg(tr("Error: %1").arg(workModel->lastError().text()), 5000);
			workModel->revertAll();
		}
	}

	if (workModel->lastError().type() != QSqlError::NoError)
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
	else
		qDebug() << "commit" << QSqlDatabase::database().commit();

//	ui.tableViewWorker->resizeRowsToContents();
}

void FarmDataDialog::on_actionAddAnimGrp_triggered()
{
	AnimGrpDialog animGrpDialog(this);
	start:
    if (animGrpDialog.exec() != QDialog::Accepted)
         return;

    QString animGrpName = animGrpDialog.name();
    int animSpc = animGrpDialog.animSpc();
    int count = animGrpDialog.count();
    double gv = animGrpDialog.gv();

    qDebug() << animGrpName << animSpc << count << gv;

    if (animGrpName.isEmpty() || animSpc < 0 || count < 0 || gv < 0)
    	return;

	QModelIndex idx = animGrpModel->index(0,1);
	if (!animGrpModel->match(idx, Qt::DisplayRole, animGrpName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(animGrpName);
		goto start;
		}

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	QSqlRecord rec = animGrpModel->record();
	rec.setNull(0);
	rec.setValue(1, animGrpName);
	rec.setValue(2, animSpc);
	rec.setValue(3, count);
	rec.setValue(4, gv);
	rec.setValue(5, QVariant(true));
	if (animGrpModel->insertRecord(-1, rec)) {
		if (!animGrpModel->submitAll()) {
			emit statusMsg(tr("Error: %1").arg(animGrpModel->lastError().text()) , 5000);
			animGrpModel->revertAll();
		}
	}

	if (animGrpModel->lastError().type() != QSqlError::NoError)
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
	else
		qDebug() << "commit" << QSqlDatabase::database().commit();

//	ui.tableViewAnim->resizeRowsToContents();
}

void FarmDataDialog::on_actionAddOprt_triggered()
{
	TextInputDialog oprtDialog(tr("&Name:"), QIcon(":/pic/action.png"), tr("Add Operation"), this);
	start:
    if (oprtDialog.exec() != QDialog::Accepted)
         return;

    QString oprtName = oprtDialog.text();

    qDebug() << oprtName;

    if (oprtName.isEmpty())
    	return;

	QModelIndex idx = oprtModel->index(0,1);
	if (!oprtModel->match(idx, Qt::DisplayRole, oprtName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(oprtName);
		goto start;
		}

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	QSqlRecord rec = oprtModel->record();
	rec.setNull(0);
	rec.setValue(1, oprtName);
	rec.setValue(2, QVariant(true));
	if (oprtModel->insertRecord(-1, rec)) {
		if (!oprtModel->submitAll()) {
			emit statusMsg(tr("Error: %1").arg(oprtModel->lastError().text()), 5000);
			oprtModel->revertAll();
		}
	}

	if (oprtModel->lastError().type() != QSqlError::NoError)
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
	else
		qDebug() << "commit" << QSqlDatabase::database().commit();

//	ui.tableViewOprt->resizeRowsToContents();
}

void FarmDataDialog::on_actionAddProd_triggered()
{
	ProdDialog prodDialog(this);
//	start:
    if (prodDialog.exec() != QDialog::Accepted)
         return;

    QStandardItemModel *prodFildModel = prodDialog.prodFildModel();
    int cropUseId = prodDialog.cropUseId();

    if (cropUseId < 0)
    	return;

	QList<int> fildIds;
	for (int row = 0; row < prodFildModel->rowCount(); row++) {
		if (prodFildModel->item(row, 0)->checkState() == Qt::Checked) {
			fildIds << prodFildModel->item(row, 0)->data(Qt::UserRole+1).toInt();
		}
	}

    if (fildIds.isEmpty())
    	return;

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	int fildId;
	bool error = false;

	foreach (fildId, fildIds) {
		QSqlRecord rec = prodModel->record();
		rec.setNull(0);
		rec.setValue(1, fildId);
		rec.setValue(2, cropUseId);
		if (prodModel->insertRecord(-1, rec)) {
			if (!prodModel->submitAll()) {
				emit statusMsg(tr("Error: %1").arg(prodModel->lastError().text()), 5000);
				error = true;
				prodModel->revertAll();
			}
		}
		else {
			error = true;
			break;
		}
	}

	if (error) {
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
		prodModel->revertAll();
	}
	else
		qDebug() << "commit" << QSqlDatabase::database().commit();

//	ui.tableViewProd->resizeRowsToContents();
}

void FarmDataDialog::on_actionAddFild_triggered()
{
	int year = -1;

	if (yearComboBox->currentText().isEmpty() && !m_noLots) {
	    QMessageBox::information(this, tr("Field"), tr("At least one lot is required."));
		return;
	}
	else if (yearComboBox->count() > 0)
	    year = yearComboBox->currentText().toInt();

	FildDialog fildDialog(m_noLots, this, year);

    if (fildDialog.exec() != QDialog::Accepted)
         return;

    int userId = fildDialog.userId();
    int fildId = fildDialog.fildId();
    QString userName = fildDialog.userName();
    QString fildName = fildDialog.fildName();
    int fildNr = fildDialog.nr();
    int fildUseId = fildDialog.fildTyp();
    int year_of_harvest = fildDialog.year();
    int production = fildDialog.production();

    if (fildDialog.newUser()) {
    	QSqlQuery q;
    	q.prepare("INSERT INTO user (user_id, user_name) VALUES (:userId, :userName)");
    	q.bindValue(":userId", userId);
    	q.bindValue(":userName", userName);

    	q.exec();
    	if (q.lastError().type() != QSqlError::NoError) {
    		emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
    		return;
    	}
    }

//	QModelIndex idxName = fildModel->index(0,1);
//	if (!fildModel->match(idxName, Qt::DisplayRole, fildName, 1,
//			Qt::MatchFixedString).isEmpty()) {
//		identMsg(fildName);
//		goto start;
//	}
//
//	QModelIndex idxNr = fildModel->index(0,2);
//	if (!fildModel->match(idxNr, Qt::DisplayRole, QString::number(fildNr), 1,
//			Qt::MatchFixedString).isEmpty()) {
//		identMsg(QString::number(fildNr));
//		goto start;
//	}

	QSqlDatabase::database().transaction();
	QSqlQuery q;

	//insert fild
	q.prepare("INSERT INTO fild (fild_id, user_id, fild_name, fild_nr, fild_use_id, year_of_harvest) "
	        " VALUES(:fild_id, :user_id, :fild_name, :fild_nr, :fild_use_id, :year_of_harvest)");
	q.bindValue(":fild_id", fildId);
	q.bindValue(":user_id", userId);
	q.bindValue(":fild_name", fildName);
	q.bindValue(":fild_nr", fildNr);
	q.bindValue(":fild_use_id", fildUseId);
	q.bindValue(":year_of_harvest", year_of_harvest);
	q.exec();

    //insert production
    q.prepare("INSERT INTO prod (fild_id, crop_use_id) VALUES(:fild, :production)");
    q.bindValue(":fild", fildId);
    q.bindValue(":production", production);
    q.exec();

    // insert env. schemas
    QList<int> schemas = fildDialog.schemas();
    foreach(int schema, schemas) {
        q.prepare("INSERT INTO fild_x_evnt_prg (fild_id, evnt_prg_id, evnt_prg_exps) VALUES(:fild, :schema, :exps)");
        q.bindValue(":fild", fildId);
        q.bindValue(":schema", schema);
        q.bindValue(":exps", year_of_harvest+1);
        q.exec();
    }

    if (m_noLots) { //if true add a dummy lot of the same size and allocate the area to the field
        int lotId = -1;
        int lotNo = -1;
        q.exec("SELECT ifnull(max(lot_id)+1, 1), ifnull(max(lot_nr)+1, 1) FROM lot");
        while (q.next()) {
            lotId = q.value(0).toInt();
            lotNo = q.value(1).toInt();
        }

        q.prepare("INSERT INTO lot (lot_id, gmrk, flur, lot_nr, lot_sub_nr, lot_size, year_of_harvest, soil_typ_id, lot_is_actv) "
                "VALUES (:lotId, '', '', :lotNr, 0, :size, :year, NULL, 'true')");
        q.bindValue(":lotId", lotId);
        q.bindValue(":lotNr", lotNo);
        q.bindValue(":size", fildDialog.size());
        q.bindValue(":year", year_of_harvest);
        q.exec();

        q.prepare("INSERT INTO lot_x_fild (lot_id, fild_id, lot_x_fild_size) "
                "VALUES (:lotId, :fildId, :size)");
        q.bindValue(":lotId", lotId);
        q.bindValue(":fildId", fildId);
        q.bindValue(":size", fildDialog.size());
        q.exec();
    }

    if (!QSqlDatabase::database().commit()) {
        QString err = QSqlDatabase::database().lastError().text();
        QSqlDatabase::database().rollback();
        emit statusMsg(tr("Error: %1").arg(err) , 5000);
        QMessageBox::information(this, tr("Error"), err);
    }
    else {
        if (yearComboBox->count() == 0) {
            yearModel->setQuery(yearModel->query().lastQuery());
            while (yearModel->canFetchMore())
                yearModel->fetchMore();
        }
        else {
            fildModel->select();
            prodModel->select();
            evntPrgModel->select();
            fildViewModel->setQuery(fildViewModel->query().lastQuery());
        }
    }


//	QSqlRecord rec = fildModel->record();
//
//	rec.setValue(0, fildId);
//	rec.setValue(1, userId);
//	rec.setValue(2, fildName);
//	rec.setValue(3, fildNr);
//	rec.setValue(4, fildUseId);
//	rec.setValue(5, year_of_harvest);
//
//	if (fildModel->insertRecord(-1, rec)) {
//		if (!fildModel->submitAll()) {
//			emit statusMsg(tr("Error: %1").arg(fildModel->lastError().text()) , 5000);
//			QSqlDatabase::database().rollback();
//			fildModel->revertAll();
//			return;
//		}
//	}
//	else {
//        emit statusMsg(tr("Error: %1").arg(fildModel->lastError().text()) , 5000);
//        QSqlDatabase::database().rollback();
//        fildModel->revertAll();
//        return;
//	}
//
//	if (fildModel->lastError().type() != QSqlError::NoError) {
//		emit statusMsg(tr("Error: %1").arg(fildModel->lastError().text()), 5000);
//        QSqlDatabase::database().rollback();
//        fildModel->revertAll();
//        return;
//	}
//	else
//	{
//	    //insert production
//	    QSqlQuery q;
//	    q.prepare("INSERT INTO prod (fild_id, crop_use_id) VALUES(:fild, :production)");
//	    q.bindValue(":fild", fildId);
//	    q.bindValue(":production", production);
//	    q.exec();
//
//
//        // insert env. schemas
//         QList<int> schemas = fildDialog.schemas();
//         foreach(int schema, schemas) {
//             q.prepare("INSERT INTO fild_x_evnt_prg (fild_id, evnt_prg_id, evnt_prg_exps) VALUES(:fild, :schema, :exps)");
//             q.bindValue(":fild", fildId);
//             q.bindValue(":schema", schema);
//             q.bindValue(":exps", year_of_harvest+1);
//             q.exec();
//         }
//
//		qDebug() << "commit" << QSqlDatabase::database().commit();
//
//        if (!schemas.isEmpty())
//            evntPrgModel->select();
//
//	    if (m_noLots) //if true add a dummy lot of the same size and allocate the area to the field
//	    {
//	    	test = QSqlDatabase::database().transaction();
//
//	    	QSqlQuery q;
//	    	q.exec("SELECT ifnull(max(lot_id)+1, 1), ifnull(max(lot_nr)+1, 1) FROM lot");
//	    	while (q.next())
//	    	{
//	    		lotId = q.value(0).toInt();
//	    		lotNo = q.value(1).toInt();
//	    	}
//
//	    	q.prepare("INSERT INTO lot (lot_id, gmrk, flur, lot_nr, lot_sub_nr, lot_size, year_of_harvest, soil_typ_id, lot_is_actv) "
//	    			"VALUES (:lotId, '', '', :lotNr, 0, :size, :year, NULL, 'true')");
//	    	q.bindValue(":lotId", lotId);
//	    	q.bindValue(":lotNr", lotNo);
//	    	q.bindValue(":size", fildDialog.size());
//	    	q.bindValue(":year", year_of_harvest);
//	    	q.exec();
//
//	    	qDebug() << q.lastError().text();
//
//
//	    	q.prepare("INSERT INTO lot_x_fild (lot_id, fild_id, lot_x_fild_size) "
//	    			"VALUES (:lotId, :fildId, :size)");
//	    	q.bindValue(":lotId", lotId);
//	    	q.bindValue(":fildId", fildId);
//	    	q.bindValue(":size", fildDialog.size());
//	    	q.exec();
//
//	    	test = QSqlDatabase::database().commit();
//
//
//
//	    }
//
//	    if (yearComboBox->count() == 0)
//	        yearModel->setQuery(yearModel->query().lastQuery());
//	    else
//	        onYearComboCurrentChanged(QString::number(year_of_harvest));
//
//	}
//
//	QSqlDatabase::database().commit();

//	ui.tableViewField->resizeRowsToContents();
}

void FarmDataDialog::on_actionAddLot_triggered()
{
	int year = -1;
	if (yearComboBox->count() > 0)
	    year = yearComboBox->currentText().toInt();


	LotDialog lotDialog(this, year);
    if (lotDialog.exec() != QDialog::Accepted)
         return;

    QString gark = lotDialog.gark();
    QString flur = lotDialog.flur();
    int lotNr = lotDialog.lotNr();
    int lotSubNr = lotDialog.lotSubNr();
    double lotSize = lotDialog.lotSize();
    year = lotDialog.year();

    QSqlDatabase::database().transaction();
    QSqlQuery q;

    //insert lot
    q.prepare("INSERT INTO lot (gmrk, flur, lot_nr, lot_sub_nr, lot_size, year_of_harvest, soil_typ_id, lot_is_actv) "
            " VALUES(:gmrk, :flur, :lot_nr, :lot_sub_nr, :lot_size, :year_of_harvest, :soil_typ_id, :lot_is_actv)");
    q.bindValue(":gmrk", gark);
    q.bindValue(":flur", flur);
    q.bindValue(":lot_nr", lotNr);
    q.bindValue(":lot_sub_nr", lotSubNr);
    q.bindValue(":lot_size", lotSize);
    q.bindValue(":year_of_harvest", year);
    q.bindValue(":soil_typ_id", QVariant());
    q.bindValue(":lot_is_actv", "true");
    q.exec();

    if (!QSqlDatabase::database().commit()) {
        QString err = QSqlDatabase::database().lastError().text();
        QSqlDatabase::database().rollback();
        emit statusMsg(tr("Error: %1").arg(err) , 5000);
        QMessageBox::information(this, tr("Error"), err);
    }
    else {
        if (yearComboBox->count() == 0) {
            yearModel->setQuery(yearModel->query().lastQuery());
            while (yearModel->canFetchMore())
                yearModel->fetchMore();
        }
        else
            lotModel->select();
    }

//	if (empty)
//		lotModel->setFilter("");

//	QModelIndex idxNr = lotModel->index(0,3);
//	if (!lotModel->match(idxNr, Qt::DisplayRole, QString::number(lotNr), 1,
//			Qt::MatchFixedString).isEmpty()) {
//		identMsg(QString::number(lotNr));
//		goto start;
//	}

//	qDebug() << "transaction" << QSqlDatabase::database().transaction();
//
//	QSqlRecord rec = lotModel->record();
//
//	rec.setNull(0);
//	rec.setValue(1, gark);
//	rec.setValue(2, flur);
//	rec.setValue(3, lotNr);
//	rec.setValue(4, lotSubNr);
//	rec.setValue(5, lotSize);
//	rec.setValue(6, year);
//	rec.setNull(7);
//	rec.setValue(8, "true");
//
//	if (lotModel->insertRecord(-1, rec)) {
//		qDebug() << lotModel->lastError().text();
//		if (!lotModel->submitAll()) {
//			emit statusMsg(tr("Error: %1").arg(lotModel->lastError().text()), 5000);
//			lotModel->revertAll();
//		}
//	}
//
//	qDebug() << lotModel->query().lastQuery();
//
//	if (lotModel->lastError().type() != QSqlError::NoError) {
//		qDebug() << "rollback" << QSqlDatabase::database().rollback();
//		emit statusMsg(tr("Error: %1").arg(lotModel->lastError().text()), 5000);
//	}
//	else
//		qDebug() << "commit" << QSqlDatabase::database().commit();
//
//	if (empty) {
//		yearModel->setQuery(yearModel->query().lastQuery());
//		lotModel->setFilter("year_of_harvest=" + yearComboBox->currentText());
//	}

//	ui.tableViewLot->resizeRowsToContents();
}

void FarmDataDialog::on_actionLotxFild_triggered()
{
	int rows = 0;
	int row = 0;
	int cols = 0;
	int col = 0;
	QString year = yearComboBox->currentText();

	if(year.isEmpty())
		return;

	QLocale locale;
	StandardItemModel *model = new StandardItemModel(rows, cols, this);
	QStandardItem *protoItem = new QStandardItem;
	protoItem->setData("NULL", Qt::DisplayRole);
	protoItem->setData("NULL", Qt::UserRole);
	model->setItemPrototype(protoItem);

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	bool error = false;

	QSqlQuery q;

	QSet<QPair<int, int> > lotFild;
	q.exec(" SELECT lot_x_fild.lot_id, lot_x_fild.fild_id "
			" FROM lot_x_fild, fild, lot "
			" WHERE lot_x_fild.fild_id=fild.fild_id "
			" AND lot_x_fild.lot_id=lot.lot_id "
			" AND fild.year_of_harvest=" + year +
			" AND lot.year_of_harvest="  + year);
	while (q.next()) {
		QPair<int, int> pair;
		pair.first = q.value(0).toInt();
		pair.second = q.value(1).toInt();
		lotFild << pair;
	}


	q.exec("SELECT count(lot_id) FROM lot WHERE year_of_harvest=" + year);
	while (q.next())
		cols = q.value(0).toInt();

	if (q.lastError().type() != QSqlError::NoError)
		error = true;

	q.exec("SELECT count(fild_id) FROM fild WHERE year_of_harvest=" + year);
	while (q.next())
		rows = q.value(0).toInt();

	if (q.lastError().type() != QSqlError::NoError)
		error = true;

	qDebug() << rows << cols;

	if (rows == 0 || cols == 0)
		return;

	QProgressDialog progress(tr("Allocating..."), tr("Abort"), 0, rows+1, this);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);

	q.exec("SELECT lot_id, lot_nr, lot_sub_nr, lot_size FROM lot WHERE year_of_harvest=" + year + " ORDER BY lot_nr ASC");

	col = 0;
	double lotSum = 0;
	while (q.next()) {
		QStandardItem *item = new QStandardItem;
		item->setData(q.value(1).toString() + "/" + q.value(2).toString() + " (" + locale.toString(q.value(3).toDouble()) + ")", Qt::DisplayRole);
		item->setData(q.value(0), Qt::UserRole);
		item->setData(q.value(3), Qt::UserRole+1);
		item->setData(q.value(1), Qt::UserRole+2);
		item->setData(q.value(2), Qt::UserRole+3);
		model->setHorizontalHeaderItem(col, item);
//		qDebug() << item->data(Qt::DisplayRole) << item->data(Qt::UserRole);
		lotSum += q.value(3).toDouble();
		col++;
	}

	if (q.lastError().type() != QSqlError::NoError)
		error = true;

	q.exec("SELECT fild_id, fild_name, user_name FROM fild, user WHERE year_of_harvest=" + year + " "
			" AND fild.user_id=user.user_id ORDER BY fild_name ASC");
	row = 0;
	QString name;
	while (q.next()) {
		name = q.value(1).toString() + "-" + q.value(2).toString();
		QStandardItem *item = new QStandardItem;
		item->setData(name, Qt::DisplayRole);
		item->setData(q.value(0), Qt::UserRole);
		item->setData(name, Qt::UserRole+2);
		model->setVerticalHeaderItem(row, item);
//		qDebug() << item->data(Qt::DisplayRole) << item->data(Qt::UserRole);
		row++;
	}

	if (q.lastError().type() != QSqlError::NoError)
		error = true;

	double fildSize;
	int fildId;
	QModelIndex idx;
	QVariant zero = QVariant(0.00);
	QVariant null = QVariant("NULL");
	double size = 0.00;

	for (int i = 0; i < rows; i++) {
		progress.setValue(i);
		fildSize = 0;
		fildId = model->verticalHeaderItem(i)->data(Qt::UserRole).toInt();
		for (int j = 0; j < cols; j++) {
			QPair<int, int> pair;
			pair.first = model->horizontalHeaderItem(j)->data(Qt::UserRole).toInt();
			pair.second = fildId;
			if (lotFild.contains(pair)) {
				q.prepare("SELECT lot_x_fild_id, lot_x_fild_size FROM lot_x_fild WHERE fild_id=:fildId AND lot_id=:lotId");
				q.bindValue(":fildId", fildId);
				q.bindValue(":lotId", model->horizontalHeaderItem(j)->data(Qt::UserRole).toInt());
				q.exec();

				idx = model->index(i, j);

				while (q.next()) {
					size = q.value(1).toDouble();
					model->setData(idx, size, Qt::DisplayRole);
					model->setData(idx, q.value(0).toInt(), Qt::UserRole);
					fildSize += size;
				}
			}

//			if (!q.first()) {
////				qDebug() << "empty" << i << j;
//				model->setData(idx, null, Qt::DisplayRole);
//				model->setData(idx, null, Qt::UserRole);
//			}
		}
		model->verticalHeaderItem(i)->setData(locale.toString(fildSize), Qt::UserRole+1);
		model->verticalHeaderItem(i)->setData(model->verticalHeaderItem(i)->data(Qt::DisplayRole).toString() + " (" + locale.toString(fildSize) + ")        ", Qt::DisplayRole);

		if (progress.wasCanceled()) {
			delete model;
			return;
		}


	}


	qDebug() << "commit" << QSqlDatabase::database().commit();

	progress.setLabelText(tr("Calculating area..."));
	progress.setValue(rows);

	if (q.lastError().type() != QSqlError::NoError)
		error = true;

	if (error) {
		QMessageBox::critical(this, "CuteFarm", tr("Error: %1").arg(q.lastError().text()));
		return;
	}


	LotxFildDialog lotxFildDialog(model, lotSum,  year, this);
	progress.setValue(rows+1);
	start:
    if (lotxFildDialog.exec() != QDialog::Accepted)
         return;

    if (!lotxFildDialog.isAllAlloc())
    	if (QMessageBox::question(this, tr("Farmland Allocation"),
    			tr("Lots and Fields have a different total size.\nContinue anyway?"),
    			QMessageBox::Yes | QMessageBox::Retry) == QMessageBox::Retry)
    		goto start;

    QSet<QStandardItem*> items = lotxFildDialog.changedItems();
    QStandardItem *item;

    qDebug() << items;

    //3 Pos: Delete (wenn nicht 'NULL' aber 0,00), Update (wenn nicht 'NULL' und nicht 0,00), Insert (wenn 'NULL' und nicht 0,00)

    //3 Pos: Delete (wenn UserRole nicht 'NULL' und DisplayRole 'NULL'), Update (wenn nicht 'NULL' und nicht 'NULL'), Insert (wenn 'NULL' und nicht 'NULL')

    if (items.isEmpty())
    	return;

    qDebug() << "transaction" << QSqlDatabase::database().transaction();

    error = false;

    //test
    foreach (item, items) {

    	if (item->data(Qt::UserRole) != QVariant("NULL") && item->data(Qt::DisplayRole) != QVariant("NULL")) {
    		q.prepare(" UPDATE lot_x_fild "
    				  " SET lot_x_fild_size=:size "
    				  " WHERE lot_x_fild_id=:id ");
    		q.bindValue(":size", item->data(Qt::DisplayRole).toDouble());
    		q.bindValue(":id", item->data(Qt::UserRole).toInt());
    		q.exec();
    		qDebug() << "on_actionLotxFild_triggered" << "UPDATE lot_x_fild";
    	}
    	else if (item->data(Qt::UserRole) != QVariant("NULL") && item->data(Qt::DisplayRole) == QVariant("NULL")) {
    		q.prepare(" DELETE FROM lot_x_fild "
    				  " WHERE lot_x_fild_id=:id ");
    		q.bindValue(":id", item->data(Qt::UserRole).toInt());
    		q.exec();

    		qDebug() << "on_actionLotxFild_triggered" << "DELETE lot_x_fild";
    	}
    	else if (item->data(Qt::UserRole) == QVariant("NULL") && item->data(Qt::DisplayRole) != QVariant("NULL")) {
    		q.prepare(" INSERT INTO lot_x_fild (lot_id, fild_id, lot_x_fild_size) "
    				  " VALUES (:lotId, :fildId, :size) ");
    		q.bindValue(":lot_id", model->horizontalHeaderItem(item->column())->data(Qt::UserRole).toInt());
    		q.bindValue(":fild_id", model->verticalHeaderItem(item->row())->data(Qt::UserRole).toInt());
    		q.bindValue(":size", item->data(Qt::DisplayRole).toDouble());
    		q.exec();

    		qDebug() << "on_actionLotxFild_triggered" << "INSERT lot_x_fild";
    	}

    	if (q.lastError().type() != QSqlError::NoError) {
    		error = true;
    		emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
    	}

    }

//    foreach (item, items) {
//
//    	if (item->data(Qt::UserRole) != "NULL" && item->data(Qt::DisplayRole).toDouble() > 0) {
//    		q.prepare(" UPDATE lot_x_fild "
//    				  " SET lot_x_fild_size=:size "
//    				  " WHERE lot_x_fild_id=:id ");
//    		q.bindValue(":size", item->data(Qt::DisplayRole).toDouble());
//    		q.bindValue(":id", item->data(Qt::UserRole).toInt());
//    		q.exec();
//    	}
//    	else if (item->data(Qt::UserRole) != "NULL" && item->data(Qt::DisplayRole).toDouble() == 0) {
//    		q.prepare(" DELETE FROM lot_x_fild "
//    				  " WHERE lot_x_fild_id=:id ");
//    		q.bindValue(":id", item->data(Qt::UserRole).toInt());
//    		q.exec();
//    	}
//    	else if (item->data(Qt::UserRole) == "NULL" && item->data(Qt::DisplayRole).toDouble() > 0) {
//    		q.prepare(" INSERT INTO lot_x_fild (lot_x_fild_id, lot_id, fild_id, lot_x_fild_size) "
//    				  " VALUES (NULL, :lotId, :fildId, :size) ");
//    		q.bindValue(":lot_id", model->horizontalHeaderItem(item->column())->data(Qt::UserRole).toInt());
//    		q.bindValue(":fild_id", model->verticalHeaderItem(item->row())->data(Qt::UserRole).toInt());
//    		q.bindValue(":size", item->data(Qt::DisplayRole).toDouble());
//    		q.exec();
//    	}
//
//    	if (q.lastError().type() != QSqlError::NoError) {
//    		error = true;
//    		emit statusMsg(tr("Error: ") + q.lastError().text(), 5000);
//    	}
//
//    }

    if (error) {
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
		QMessageBox::critical(this, tr("Database Error"), tr("Error: %1").arg(q.lastError().text()));

		if (!q.lastError().text().isEmpty())
			emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
    }
    else {
    	qDebug() << "commit" << QSqlDatabase::database().commit();
    	fildViewModel->setQuery(fildViewModel->query().lastQuery());
//    	ui.tableOverView->resizeRowsToContents();
    }

    delete model;
}

void FarmDataDialog::on_actionFildUnion_triggered()
{
	int parentFildId = -1;
	QList<int> childFildIds;
	if(ui.tableViewField->currentIndex().isValid()){
		QList<QModelIndex> idxList;
		idxList = ui.tableViewField->selectionModel()->selectedRows();
		if(idxList.count() > 1) {

			parentFildId = fildModel->data(fildModel->index(idxList.first().row(), 0), Qt::DisplayRole).toInt();
			idxList.removeFirst();
			for (int i = 0; i < idxList.size(); i++) {
				childFildIds << fildModel->data(fildModel->index(idxList.at(i).row(), 0), Qt::DisplayRole).toInt();
			}

			qDebug() << parentFildId << childFildIds;

			qDebug() << "transaction" << QSqlDatabase::database().transaction();

			QSqlQuery q;
			for (int i = 0; i < childFildIds.size(); i++) {
				q.prepare(" UPDATE fild SET fild_prnt_id=:parentFildId WHERE fild_id=:childFildId");
				q.bindValue(":parentFildId", parentFildId);
				q.bindValue(":childFildId", childFildIds.at(i));
				q.exec();
				qDebug() << q.lastError().text();
				if (q.lastError().type() != QSqlError::NoError) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					emit statusMsg(tr("Error: %1").arg(fildModel->lastError().text()), 5000);
					return;
				}
			}
			qDebug() << "commit" << QSqlDatabase::database().commit();
			fildModel->select();
//			ui.tableViewField->resizeRowsToContents();
		}
	}
}

void FarmDataDialog::on_actionFildSep_triggered()
{
	QList<int> fildIds;
	int parentFildId = -1;
	if (ui.tableViewField->currentIndex().isValid()) {
		parentFildId = fildModel->data(fildModel->index(ui.tableViewField->currentIndex().row(), 0), Qt::DisplayRole).toInt();
		QList<QModelIndex> idxList;
		idxList = ui.tableViewField->selectionModel()->selectedRows();
		if (idxList.count() > 1) {
			for (int i = 0; i < idxList.size(); i++) {
				fildIds << fildModel->data(fildModel->index(idxList.at(i).row(), 0), Qt::DisplayRole).toInt();
			}

			qDebug() << fildIds;
			qDebug() << "transaction" << QSqlDatabase::database().transaction();

			QSqlQuery q;
			for (int i = 0; i < fildIds.size(); i++) {
				q.prepare(" UPDATE fild SET fild_prnt_id=:prntFildId WHERE fild_id=:fildId ");
				q.bindValue(":prntFildId", fildIds.at(i));
				q.bindValue(":fildId", fildIds.at(i));
				q.exec();
				qDebug() << q.lastError().text();
				if (q.lastError().type() != QSqlError::NoError) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
					return;
				}
			}
			qDebug() << "commit" << QSqlDatabase::database().commit();
			fildModel->select();
//			ui.tableViewField->resizeRowsToContents();
		}
		else {
			QString info = "Für temporäre Teilschläge wird eine vierstellige Schlagnummer erzeugt.";
			NumberInputDialog numberInputDialog(this, "Teilschläge", info, "Anzahl:");
		    if (numberInputDialog.exec() != QDialog::Accepted)
		         return;

		    int number = numberInputDialog.number();
		    int maxNr = 1000;
			qDebug() << "transaction" << QSqlDatabase::database().transaction();

			QSqlQuery q;
			q.exec(" SELECT max(fild_nr) from fild ");
			while (q.next()) {
				if (q.value(0).toInt() >= 1000)
					maxNr = q.value(0).toInt() +1;
			}

			int fildUseId = -1;
			QString fildUse = fildModel->data(fildModel->index(ui.tableViewField->currentIndex().row(), 4), Qt::DisplayRole).toString();
			QString fildName = fildModel->data(fildModel->index(ui.tableViewField->currentIndex().row(), 1), Qt::DisplayRole).toString();
			qDebug() << fildUse;
			q.exec(" SELECT fild_use_id from fild_use WHERE fild_use_name='" + fildUse +"'"); //<-schlecht
			while (q.next())
				fildUseId = q.value(0).toInt();

			if (fildUseId < 0)
				return;

			for (int i = 0; i < number; i++) {
				q.prepare(" INSERT INTO fild "
						  " VALUES( NULL, :fildName ,:fildNr, :parentFildNr, :fildUseId, :year)");
				q.bindValue(":fildName", fildName + "_" + QString::number(i));
				q.bindValue(":fildNr", maxNr);
				q.bindValue(":parentFildNr", maxNr);
				q.bindValue(":fildUseId", fildUseId);
				q.bindValue(":year", yearComboBox->currentText().toInt());
				q.exec();
				qDebug() << q.lastError().text();
				if (q.lastError().type() != QSqlError::NoError) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
					return;
				}
				maxNr++;
			}

//			QMap<int, double> lots;
//			q.exec(" SELECT lot_id, lot_size FROM lot_x_fild WHERE fild_id=" + QString::number(parentFildId));
//			while (q.next())
//				lots.insert(q.value(0).toInt(), q.value(1).toDouble());
//
//			if (lots.isEmpty()) {
//				qDebug() << "rollback" << QSqlDatabase::database().rollback();
//				emit statusMsg(tr("Error: Für den Schlag sind noch keine Flächen eingetragen"));
//				return;
//			}
//
//			for (int i = 0; i < number; i++) {
//
//			}

			qDebug() << "commit" << QSqlDatabase::database().commit();
			fildModel->select();
//			ui.tableViewField->resizeRowsToContents();


		}
	}
}

void FarmDataDialog::on_actionAddNewHarvYear_triggered()
{
	if (yearComboBox->currentText().isEmpty())
		return;

	int currentYear = -1;
	int newYear = -1;

	QSqlQuery q;
	q.exec("SELECT max(year_of_harvest) FROM lot");
	while (q.next())
	    currentYear = q.value(0).toInt();

	if (currentYear < 0) {
		return;
	}

	newYear = currentYear+1;

	NewHarvestYearDialog nhyD(currentYear, this);
	nhyD.setWindowTitle(nhyD.windowTitle().append(" " + QString::number(currentYear) + "->" + QString::number(newYear)));
    if (nhyD.exec() != QDialog::Accepted)
         return;

    QSet<int> fildUseIds = nhyD.fildUseIds();

    qDebug() << fildUseIds;

    QSqlDatabase::database().transaction();

	q.prepare(" INSERT INTO fild (fild_id, user_id, fild_name, fild_nr, fild_use_id, year_of_harvest) "
			  " SELECT NULL, user_id, fild_name, fild_nr, fild_use_id, :newYear "
			  " FROM fild "
			  " WHERE year_of_harvest=:currentYear ");
	q.bindValue(":newYear", newYear);
	q.bindValue(":currentYear", currentYear);
	q.exec();

    if (q.lastError().type() != QSqlError::NoError) {
        qDebug() << "rollback" << QSqlDatabase::database().rollback();
        emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
        return;
    }

    q.prepare("INSERT INTO fild_x_evnt_prg (fild_id, evnt_prg_id, evnt_prg_exps) "
            " select new_fild, evnt_prg_id, evnt_prg_exps "
            " FROM fild_x_evnt_prg "
            " JOIN ( "
            " select a.fild_id AS old_fild , b.fild_id AS new_fild from fild as a "
            " join fild as b using(fild_nr) "
            " where a.year_of_harvest = :currentYear "
            " AND b.year_of_harvest = :newYear ) as c ON fild_x_evnt_prg.fild_id=c.old_fild "
            " WHERE evnt_prg_exps >= :newYearScnd");
    q.bindValue(":currentYear", currentYear);
    q.bindValue(":newYear", newYear);
    q.bindValue(":newYearScnd", newYear);
    q.exec();

	if (q.lastError().type() != QSqlError::NoError) {
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
		emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
		return;
	}

	q.prepare(" INSERT INTO lot (lot_id, gmrk, flur, lot_nr, lot_sub_nr, lot_size, year_of_harvest, soil_typ_id, lot_is_actv) "
			  " SELECT NULL, gmrk, flur, lot_nr, lot_sub_nr, lot_size, :newYear, soil_typ_id, lot_is_actv "
			  " FROM lot "
			  " WHERE year_of_harvest=:currentYear ");
	q.bindValue(":newYear", newYear);
	q.bindValue(":currentYear", currentYear);
	q.exec();

	if (q.lastError().type() != QSqlError::NoError) {
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
		emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
		return;
	}

	q.prepare(" INSERT INTO lot_x_fild (lot_x_fild_id, lot_id, fild_id, lot_x_fild_size) "
			  " SELECT NULL, lot_id, fild_id, old.lot_x_fild_size "
	          " FROM lot, fild, ( SELECT gmrk, flur, lot_nr, lot_sub_nr, user_id, fild_nr, lot_x_fild_size "
	                            " FROM lot, fild, lot_x_fild "
	                            " WHERE lot.lot_id=lot_x_fild.lot_id "
	                            " AND fild.fild_id=lot_x_fild.fild_id "
	                            " AND lot.year_of_harvest=:currentYear1 "
	                            " AND fild.year_of_harvest=:currentYear2 ) AS old "
				" WHERE lot.gmrk=old.gmrk "
				" AND lot.flur=old.flur "
				" AND lot.lot_nr=old.lot_nr "
				" AND lot.lot_sub_nr=old.lot_sub_nr "
				" AND lot.year_of_harvest=:newYear1 "
				" AND fild.user_id=old.user_id "
				" AND fild.fild_nr=old.fild_nr "
				" AND fild.year_of_harvest=:newYear2 ");
	q.bindValue(":currentYear1", currentYear);
	q.bindValue(":currentYear2", currentYear);
	q.bindValue(":newYear1", newYear);
	q.bindValue(":newYear2", newYear);
	q.exec();

	if (q.lastError().type() != QSqlError::NoError) {
		qDebug() << "rollback" << QSqlDatabase::database().rollback();
		emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
		return;
	}

	else {
		qDebug() << "commit" << QSqlDatabase::database().commit();
	}

	QSqlDatabase::database().transaction();

	foreach (int fildUseId, fildUseIds) {
		q.prepare(" INSERT INTO prod (prod_id, fild_id, crop_use_id) "
				  " SELECT NULL, fild.fild_id, old.crop_use_id "
				  " FROM fild,"
				  " (SELECT user_id, fild_nr, fild_use_id, crop_use_id FROM fild, prod "
				  "  WHERE year_of_harvest=:currentYear"
				  "  AND fild.fild_id=prod.fild_id "
				  "  AND fild_use_id=:fildUseId) AS old "
				  " WHERE fild.year_of_harvest=:newYear "
				  " AND fild.user_id=old.user_id "
				  " AND fild.fild_nr=old.fild_nr ");
		q.bindValue(":fildUseId", fildUseId);
		q.bindValue(":currentYear", currentYear);
		q.bindValue(":newYear", newYear);
		q.exec();

		qDebug() << q.lastQuery();
		qDebug() << fildUseId;
		qDebug() << currentYear;
		qDebug() << newYear;

		if (q.lastError().type() != QSqlError::NoError) {
			qDebug() << "rollback" << QSqlDatabase::database().rollback();
			emit statusMsg(tr("Error: %1").arg(q.lastError().text()), 5000);
			return;
		}
	}

	QSqlDatabase::database().commit();

    yearModel->setQuery(yearModel->query().lastQuery());
    while(yearModel->canFetchMore())
    	yearModel->fetchMore();
}

void FarmDataDialog::on_actionAddEnvSchema_triggered()
{
    if (fildModel->rowCount() <= 0)
        return;

    QMap<int, QString> fields;

    QSqlQuery q;
    q.prepare("SELECT fild_id, fild_name FROM fild WHERE year_of_harvest=:year ORDER BY fild_name");
    q.bindValue(":year", yearComboBox->currentText().toInt());
    q.exec();
    while (q.next())
        fields.insert(q.value(0).toInt(), q.value(1).toString());

    if (fields.isEmpty())
        return;

    int envId = -1;
    q.exec("SELECT evnt_prg_id FROM evnt_prg LIMIT 1");
    while (q.next())
        envId = q.value(0).toInt();

    if (envId < 0)
        return;

    bool ok = false;
    QString field = QInputDialog::getItem(this, tr("Add Environmental Program"), tr("Select Field:"), fields.values(), 0, false, &ok);

    if (!ok)
        return;

    QSqlRecord rec = evntPrgModel->record();
    rec.setValue(1, fields.key(field));
    rec.setValue(2, envId);
    rec.setValue(3, yearComboBox->currentText().toInt()+1);

    if (evntPrgModel->insertRecord(-1, rec))
        evntPrgModel->setPendingTransaction(true);

    ui.buttonBox->button(QDialogButtonBox::Save)->setVisible(true);
    ui.buttonBox->button(QDialogButtonBox::Reset)->setVisible(true);

}

void FarmDataDialog::onYearComboCurrentChanged(const QString &text)
{
	if (text.isEmpty())
		return;

	if (pendingTransaction())
		buttonReset();

	qDebug() << "year" << text;

	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	fildModel->setFilter("year_of_harvest=" + text);
	lotModel->setFilter("year_of_harvest=" + text);
	prodModel->setFilter("prod.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + text +")");
	evntPrgModel->setFilter("fild_x_evnt_prg.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + text + ")");
	prodModel->select();
	fildModel->select();
	lotModel->select();
	evntPrgModel->select();

    fildViewModel->setQuery(" SELECT * FROM v_fild "
    		                " WHERE year=" + text +
    		                " ORDER BY fild_name ASC ");

    qDebug() << "commit" << QSqlDatabase::database().commit();

//    ui.tableOverView->resizeRowsToContents();
}

void FarmDataDialog::on_buttonBox_helpRequested()
{
    QString message;

    switch (ui.stackedWidget->currentIndex()) {
    case 0:
        message = tr("General data about your farm. They will be used e.g. in the header of the print report.");
        break;
    case 1:
        message = tr("Add your farm worker here.");
        break;
    case 2:
        message = tr("By default the following steps are necessary: Create a lot, create a field, allocate area from the lot to the field. "
                "And finally assign a production (crop use) to the field.\n"
        		"If you are not interested in managing your farmland in such detail you can tick the option 'No Lots' in the settings dialog. "
        		"By default lots are disabled.");
        break;
    case 3:
        message = tr("Add you animal groups here.");
        break;
    case 4:
        message = tr("Add your operations here. E.g. a tractor plus towed equipment");
        break;
    case 5:
        message = tr("In order to add actions you need to specify a production for each field and harvest year.");
        break;
    default:
        message = "No help available.";
    }

    QMessageBox::information(this, tr("Farm Data"), message);
}

void FarmDataDialog::on_toolButtonIsActive_clicked()
{
//	QTableView *view = currentTable();
//	if (view) {
//		SqlTableModel *model = qobject_cast<SqlTableModel*>(view->model());
//		qDebug() << model;
//		if (model) {
//			if (model->boolColumn() < 0)
//				return;
//			QList<QModelIndex> idxList;
//			idxList = view->selectionModel()->selectedRows();
//			if (idxList.isEmpty())
//				return;
//		    for (int i = 0; i < idxList.size(); ++i) {
//		    	bool data = model->data(idxList.at(i).sibling(idxList.at(i).row(), model->boolColumn())).toBool();
//		    	if (data)
//		    		data = false;
//		    	else
//		    		data = true;
//		    	qDebug() << model->setData(idxList.at(i).sibling(idxList.at(i).row(), model->boolColumn()), QVariant(data));
//		    }
//		    model->setPendingTransaction(true);
//		}
//		else {
//			SqlRelationalTableModel *rmodel = qobject_cast<SqlRelationalTableModel*>(view->model());
//			qDebug() << rmodel << rmodel->boolColumn();
//			if (rmodel) {
//				if (rmodel->boolColumn() < 0)
//					return;
//				QList<QModelIndex> idxList;
//				idxList = view->selectionModel()->selectedRows();
//				if (idxList.isEmpty())
//					return;
//			    for (int i = 0; i < idxList.size(); ++i) {
//			    	bool data = rmodel->data(idxList.at(i).sibling(idxList.at(i).row(), rmodel->boolColumn())).toBool();
//			    	if (data)
//			    		data = false;
//			    	else
//			    		data = true;
//			    	qDebug() << rmodel->setData(idxList.at(i).sibling(idxList.at(i).row(), rmodel->boolColumn()), QVariant(data));
//			    }
//			    rmodel->setPendingTransaction(true);
//			}
//		}
//	}
}

void FarmDataDialog::on_toolButtonRemove_clicked()
{
	QTableView *view = qobject_cast<QTableView*>(focusWidget());
	if (view) {
		SqlTableModel *model = qobject_cast<SqlTableModel*>(view->model());
		qDebug() << model;
		if (model) {
			if (view->currentIndex().isValid()) {
			    QString data = view->currentIndex().sibling(view->currentIndex().row(), 1).data().toString();
				qDebug() << data;
				QList<QModelIndex> idxList;
				qDebug() << idxList;
				idxList = view->selectionModel()->selectedRows();
				if (idxList.isEmpty())
					return;
				if (idxList.count() > 1)
					data = QString();
				if (removeMsg(data)) {
			         for (int i = 0; i < idxList.size(); ++i) {
			        	 if (model->removeRow(idxList.at(i).row(), QModelIndex())) {
			        		 ui.buttonBox->button(QDialogButtonBox::Reset)->show();
							 ui.buttonBox->button(QDialogButtonBox::Save)->show();
						 }
			         }
			         model->setPendingTransaction(true);
				}
			}
		}
		else {
			SqlRelationalTableModel *rmodel = qobject_cast<SqlRelationalTableModel*>(view->model());
			if (rmodel) {
				if (view->currentIndex().isValid()) {
				    QString data = view->currentIndex().sibling(view->currentIndex().row(), 1).data().toString();
					QList<QModelIndex> idxList;
					idxList = view->selectionModel()->selectedRows();
					if (idxList.isEmpty())
						return;
					if (idxList.count() > 1)
						data = QString();
					if (removeMsg(data)) {
				         for (int i = 0; i < idxList.size(); ++i) {
				        	 if (rmodel->removeRow(idxList.at(i).row(), QModelIndex())) {
				        		ui.buttonBox->button(QDialogButtonBox::Reset)->show();
								ui.buttonBox->button(QDialogButtonBox::Save)->show();
							}
				        }
				        rmodel->setPendingTransaction(true);
					}
				}
			}
		}
	}
}

bool FarmDataDialog::pendingTransaction()
{
	if ( workModel->pendingTransaction() || farmModel->pendingTransaction() ||
			oprtModel->pendingTransaction() || animGrpModel->pendingTransaction() || prodModel->pendingTransaction() ||
			lotModel->pendingTransaction() || fildModel->pendingTransaction() || lotModel->pendingTransaction()
			|| evntPrgModel->pendingTransaction())
		return true;
	else
		return false;

}

void FarmDataDialog::buttonSave()
{
//	qDebug() << "FarmDataDialog::buttonSave()transaction()" << QSqlDatabase::database().transaction();
	//in einzelne transaktionen zerlegen.
//	bool error = false;
	QString err;

	if (farmModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		if(!farmModel->submitAll()) {
		    err.append(QString("%1\n").arg(farmModel->lastError().text()));
			QSqlDatabase::database().rollback();
			farmModel->revertAll();
			farmModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            farmModel->select();
		}
		farmMapper->toFirst();
	}

	if (workModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		if (!workModel->submitAll()) {
		    err.append(QString("%1\n").arg(workModel->lastError().text()));
			QSqlDatabase::database().rollback();
			workModel->revertAll();
			workModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
		    err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            workModel->select();
        }
	}

	if (oprtModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		if ( !oprtModel->submitAll()) {
		    err.append(QString("%1\n").arg(oprtModel->lastError().text()));
			QSqlDatabase::database().rollback();
			oprtModel->revertAll();
			oprtModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            oprtModel->select();
        }
	}

	if (animGrpModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		if (!animGrpModel->submitAll()) {
		    err.append(QString("%1\n").arg(animGrpModel->lastError().text()));
			QSqlDatabase::database().rollback();
			animGrpModel->revertAll();
			animGrpModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            animGrpModel->select();
        }
	}

	if (fildModel->pendingTransaction()) {
	    qDebug() << "transaction" << QSqlDatabase::database().transaction();
	    int countBefore = fildModel->rowCount();
		if (!fildModel->submitAll()) {
		    err.append(QString("%1\n").arg(fildModel->lastError().text()));
			QSqlDatabase::database().rollback();
			fildModel->revertAll();
			fildModel->setPendingTransaction(false);
			qDebug() << "fildModel" << "submitAll failed";
		}
		else if (!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            fildModel->select();
        }
        if (countBefore > fildModel->rowCount() && m_noLots) { //automat. del unallocated lots if m_noLots
             QSqlQuery q;
             q.exec("DELETE FROM lot WHERE lot_id NOT IN (SELECT lot_id FROM lot_x_fild)");
             onYearComboCurrentChanged(yearComboBox->currentText());
        }
        else if (countBefore > fildModel->rowCount())
            onYearComboCurrentChanged(yearComboBox->currentText());

	}

	if (lotModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		lotModel->submitAll();
		if (lotModel->lastError().type() != QSqlError::NoError) {
		    err.append(QString("%1\n").arg(lotModel->lastError().text()));
			QSqlDatabase::database().rollback();
			lotModel->revertAll();
			lotModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            lotModel->select();
        }
        onYearComboCurrentChanged(yearComboBox->currentText());
	}

	if (prodModel->pendingTransaction()) {
	    QSqlDatabase::database().transaction();
		if (!prodModel->submitAll()) {
		    err.append(QString("%1\n").arg(prodModel->lastError().text()));
		    QSqlDatabase::database().rollback();
			prodModel->revertAll();
			prodModel->setPendingTransaction(false);
		}
		else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            prodModel->select();
        }
	}

    if (evntPrgModel->pendingTransaction()) {
        QSqlDatabase::database().transaction();
        if (!evntPrgModel->submitAll()) {
            err.append(QString("%1\n").arg(evntPrgModel->lastError().text()));
            QSqlDatabase::database().rollback();
            evntPrgModel->revertAll();
            evntPrgModel->setPendingTransaction(false);
        }
        else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            evntPrgModel->select();
        }
    }

	ui.buttonBox->button(QDialogButtonBox::Save)->hide();
	ui.buttonBox->button(QDialogButtonBox::Reset)->hide();

	if (!err.trimmed().isEmpty())
	    QMessageBox::warning(this, tr("Error"), err);
}

void FarmDataDialog::buttonReset()
{
	if ( farmModel->pendingTransaction() ) {

		farmModel->revertAll();
		farmModel->setPendingTransaction(false);
		farmMapper->toFirst();
	}


	if ( workModel->pendingTransaction() ) {

		workModel->revertAll();
		workModel->setPendingTransaction(false);

	}


	if ( oprtModel->pendingTransaction() ) {

		oprtModel->revertAll();
		oprtModel->setPendingTransaction(false);

	}


	if ( animGrpModel->pendingTransaction() ) {

		animGrpModel->revertAll();
		animGrpModel->setPendingTransaction(false);

	}

	if ( fildModel->pendingTransaction() ) {

		fildModel->revertAll();
		fildModel->setPendingTransaction(false);

	}

	if ( lotModel->pendingTransaction() ) {

		lotModel->revertAll();
		lotModel->setPendingTransaction(false);

	}

	if ( prodModel->pendingTransaction() ) {

		prodModel->revertAll();
		prodModel->setPendingTransaction(false);

	}

    if ( evntPrgModel->pendingTransaction() ) {

        evntPrgModel->revertAll();
        evntPrgModel->setPendingTransaction(false);

    }

	ui.buttonBox->button(QDialogButtonBox::Save)->hide();
	ui.buttonBox->button(QDialogButtonBox::Reset)->hide();

}

void FarmDataDialog::buttonClose()
{
	if(ui.buttonBox->button(QDialogButtonBox::Save)->isVisible()){
		if(QMessageBox::question(0, "CuteFarm",
	                   tr("Apply changes?"),
	                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			buttonSave();
			accept();
		}
		else{
			buttonReset();
			accept();
		}
	}
	else{
		buttonReset();
		accept();
	}
}

bool FarmDataDialog::removeMsg(const QString &data)
{
	if(!data.isEmpty()){
		if(QMessageBox::question(this, "CuteFarm",
				tr("If this entry is connected to e.g. an action\n"
					                	"it is not possible to delete it.\n"
					                      "\n"
					                      "Delete '%1'?").arg(data),
	                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			return true;
		}else{
			return false;
		}
	}
	else{
		if(QMessageBox::question(this, "CuteFarm",
				tr("If this entry is connected to e.g. an action\n"
					                	"it is not possible to delete it.\n"
					                      "\n"
					                      "Delete '%1'?").arg(data),
	                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			return true;
		}else{
			return false;
		}
	}
}

void FarmDataDialog::identMsg(const QString &name)
{
	QMessageBox::information(this, "CuteFarm",
	                   tr("The name\n"
	                		   "'%1'\n"
	                				   "is already used.").arg(name.simplified()),
	                   QMessageBox::Ok);
}

bool FarmDataDialog::isWhitespace(const QString &text)
{
	if(text.size() == text.count(" ")){
		return true;
	}
	else{
		return false;
	}
}

QString FarmDataDialog::farmAddress()
{
	QString add;

	QSqlQuery q;
	q.exec("SELECT * FROM farm LIMIT 1");
	while (q.next()){
		add.append(q.value(0).toString().append(", "));
		add.append(q.value(1).toString().append(", "));
		add.append(q.value(2).toString().append(", "));
		add.append(q.value(3).toString());

	}
	return add;
}

QString FarmDataDialog::farmName()
{
	QString name;

	QSqlQuery q;
	q.exec("SELECT farm_name FROM farm LIMIT 1");
	while (q.next()){
		name = q.value(0).toString();
	}
	return name;
}

FarmDataDialog::~FarmDataDialog()
{

}
